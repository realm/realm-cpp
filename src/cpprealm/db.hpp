////////////////////////////////////////////////////////////////////////////
//
// Copyright 2022 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef realm_realm_hpp
#define realm_realm_hpp

#include <filesystem>
#include <iostream>

#if !defined(REALM_DISABLE_ANALYTICS)
#include <cpprealm/analytics.hpp>
#endif
#include <cpprealm/type_info.hpp>
#include <cpprealm/results.hpp>
#include <cpprealm/task.hpp>
#include <cpprealm/flex_sync.hpp>

#include <realm/object-store/binding_context.hpp>
#include <realm/object-store/object_schema.hpp>
#include <realm/object-store/object_store.hpp>
#include <realm/object-store/shared_realm.hpp>
#include <realm/object-store/sync/async_open_task.hpp>
#include <realm/object-store/util/scheduler.hpp>
#include <utility>
#include "thread_safe_reference.hpp"

#ifdef QT_CORE_LIB
#include <QStandardPaths>
#include <QMetaObject>
#include <QTimer>
#include <QThread>
#include <QtWidgets/QApplication>
#endif

namespace realm {
struct object;
#if QT_CORE_LIB
namespace util {
struct QtMainLoopScheduler : public QObject, public realm::util::Scheduler {

    bool can_invoke() const noexcept override
    {
        return true;
    }
    void invoke(UniqueFunction<void()>&& fn) override
    {
        QMetaObject::invokeMethod(this, std::move(fn));
    }
    bool is_on_thread() const noexcept override
    {
        return m_id == std::this_thread::get_id();
    }
    bool is_same_as(const Scheduler* other) const noexcept override
    {
        auto o = dynamic_cast<const QtMainLoopScheduler*>(other);
        return (o && (o->m_id == m_id));
    }
    bool can_deliver_notifications() const noexcept
    {
        return QThread::currentThread()->eventDispatcher();
    }
private:
    std::function<void()> m_callback;
    std::thread::id m_id = std::this_thread::get_id();
};
static std::shared_ptr<realm::util::Scheduler> make_qt()
{
    return std::make_shared<QtMainLoopScheduler>();
}
}
#endif

struct db_config {

    std::string path = std::filesystem::current_path();

    std::shared_ptr<SyncConfig> sync_config;
private:
    friend struct User;
    template <typename ...Ts>
    friend struct db;
};

#if QT_CORE_LIB
static std::function<std::shared_ptr<util::Scheduler>()> scheduler = &util::make_qt;
#else
static std::function<std::shared_ptr<util::Scheduler>()> scheduler = &util::Scheduler::make_default;
#endif

template <typename ...Ts>
struct db {
    explicit db(db_config config = {}) : config(std::move(config))
    {
        std::vector<ObjectSchema> schema;

        (schema.push_back(Ts::schema.to_core_schema()), ...);

        m_realm = Realm::get_shared_realm({
            .path = this->config.path.append(".realm"),
            .schema_mode = SchemaMode::AdditiveExplicit,
            .schema = Schema(schema),
            .schema_version = 0,
            .scheduler = scheduler(),
            .sync_config = this->config.sync_config
        });

        static bool initialized;
        if (!initialized) {
            realm_analytics::send();
            initialized = true;
        }
    }

    SyncSubscriptionSet subscriptions()
    {
        return SyncSubscriptionSet(m_realm->get_active_subscription_set(), m_realm);
    }

    void write(std::function<void()>&& block) const
    {
        m_realm->begin_transaction();
        block();
        m_realm->commit_transaction();
    }

    template <typename T>
    void add(T& object)
    {
        static_assert(type_info::ObjectPersistableConcept<T>::value && (std::is_same_v<T, Ts> || ...));
        auto actual_schema = *m_realm->schema().find(T::schema.name);
        auto& group = m_realm->read_group();
        auto table = group.get_table(actual_schema.table_key);
        T::schema.add(object, table, m_realm);
    }

    template <typename T>
    void add(T&& object)
    {
        add(object);
    }

    template <typename T>
    void remove(T& object)
    {
        static_assert(type_info::ObjectPersistableConcept<T>::value && (std::is_same_v<T, Ts> || ...));
        REALM_ASSERT(object.is_managed());
        auto& group = m_realm->read_group();
        auto schema = object.m_object->get_object_schema();
        auto table = group.get_table(schema.table_key);
        table->remove_object(object.m_object->obj().get_key());
    }

    template <typename T>
    results<T> objects()
    {
        static_assert(type_info::ObjectPersistableConcept<T>::value && (std::is_same_v<T, Ts> || ...),
                "Object type not available in this realm");
        return results<T>(Results(m_realm, m_realm->read_group().get_table(ObjectStore::table_name_for_object_type(T::schema.name))));
    }

    template <typename T>
    T object(const typename decltype(T::schema)::PrimaryKeyProperty::Result& primary_key) {
        static_assert(type_info::ObjectPersistableConcept<T>::value && (std::is_same_v<T, Ts> || ...));
        auto table = m_realm->read_group().get_table(ObjectStore::table_name_for_object_type(T::schema.name));
        return T::schema.create(table->get_object_with_primary_key(primary_key),
                                 m_realm);
    }

    template <typename T>
    T resolve(thread_safe_reference<T>&& tsr) const
    {
        Object object = tsr.m_tsr.template resolve<Object>(m_realm);
        return T::schema.create(object.obj(), object.realm());
    }

    [[maybe_unused]] bool refresh()
    {
        return m_realm->refresh();
    }

    db_config config;
private:
    db(const SharedRealm& realm)
    : m_realm(realm)
    {
        config.path = realm->config().path;
        config.sync_config = realm->config().sync_config;

        static bool initialized;
        if (!initialized) {
            realm_analytics::send();
            initialized = true;
        }
    }
    friend struct object;
    template <typename T, typename>
    friend struct thread_safe_reference;
    SharedRealm m_realm;
};

template <typename ...Ts>
static db<Ts...> open(db_config config = {})
{
    // TODO: Add these flags to core
#if QT_CORE_LIB
    util::Scheduler::set_default_factory(util::make_qt);
#endif
    return db<Ts...>(std::move(config));
}

template <typename ...Ts>
static inline std::promise<thread_safe_reference<db<Ts...>>> async_open(const db_config& config) {
    // TODO: Add these flags to core
#if QT_CORE_LIB
    util::Scheduler::set_default_factory(util::make_qt);
#endif
    std::vector<ObjectSchema> schema;
    (schema.push_back(Ts::schema.to_core_schema()), ...);

    std::promise<thread_safe_reference<db<Ts...>>> p;

    std::shared_ptr<AsyncOpenTask> async_open_task = Realm::get_synchronized_realm({
                                                                                           .path = config.path,
                                                                                           .schema_mode = SchemaMode::AdditiveExplicit,
                                                                                           .schema = Schema(schema),
                                                                                           .schema_version = 0,
                                                                                           .sync_config = config.sync_config
                                                                                   });
    async_open_task->start([&p](auto tsr, auto ex) {
        if (ex) p.set_exception(ex);
        else p.set_value(thread_safe_reference<db<Ts...>>(std::move(tsr)));
    });

    static bool initialized;
    if (!initialized) {
        realm_analytics::send();
        initialized = true;
    }
    return p;
}

template <typename ...Ts>
struct async_open_realm {
    async_open_realm(const db_config& config, util::UniqueFunction<void(thread_safe_reference<db<Ts...>>, std::exception_ptr e)> &&callback) {
        // TODO: Add these flags to core
#if QT_CORE_LIB
        util::Scheduler::set_default_factory(util::make_qt);
#endif
        std::vector<ObjectSchema> schema;
        (schema.push_back(Ts::schema.to_core_schema()), ...);
        std::promise<thread_safe_reference<db<Ts...>>> p;

        std::shared_ptr<AsyncOpenTask> async_open_task = Realm::get_synchronized_realm({
            .path = config.path,
            .schema_mode = SchemaMode::AdditiveExplicit,
            .schema = Schema(schema),
            .schema_version = 0,
            .sync_config = config.sync_config
        });

        static bool initialized;
        if (!initialized) {
            realm_analytics::send();
            initialized = true;
        }
        async_open_task->start(std::move(callback));
    }
};
}
#endif /* realm_realm_hpp */
