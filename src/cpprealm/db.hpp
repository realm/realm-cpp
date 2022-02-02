////////////////////////////////////////////////////////////////////////////
//
// Copyright 2021 Realm Inc.
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

#include <cpprealm/type_info.hpp>
#include <cpprealm/object.hpp>
#include <cpprealm/results.hpp>
#include <cpprealm/task.hpp>
#include <cpprealm/thread_safe_reference.hpp>

#include <realm/object-store/object_schema.hpp>
#include <realm/object-store/object_store.hpp>
#include <realm/object-store/shared_realm.hpp>
#include <realm/object-store/sync/async_open_task.hpp>
#include <realm/object-store/util/scheduler.hpp>
#include <utility>

#ifdef QT_CORE_LIB
#include <QStandardPaths>
#include <QMetaObject>
#include <QTimer>
#include <QThread>
#include <QtWidgets/QApplication>
#endif

namespace realm {

#if QT_CORE_LIB

namespace util {
struct QtMainLoopScheduler : public QObject, public realm::util::Scheduler {

    bool is_on_thread() const noexcept override
    {
        return m_id == std::this_thread::get_id();
    }
    bool is_same_as(const Scheduler* other) const noexcept override
    {
        auto o = dynamic_cast<const QtMainLoopScheduler*>(other);
        return (o && (o->m_id == m_id));
    }
    bool can_deliver_notifications() const noexcept override
    {
        return QThread::currentThread()->eventDispatcher();
    }

    void set_notify_callback(std::function<void()> fn) override
    {
        m_callback = std::move(fn);
    }

    void notify() override
    {
        schedule(m_callback);
    }

    void schedule(std::function<void()> fn) {
        QMetaObject::invokeMethod(this, fn);
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
//    db_config() = default;
//    db_config(std::string path) : path(std::move(path)) {}

    std::string path = std::filesystem::current_path().append("default.realm");

    std::shared_ptr<SyncConfig> sync_config;
private:
    friend struct User;
    template <type_info::ObjectPersistable ...Ts>
    friend struct db;
};

#if QT_CORE_LIB
static std::function<std::shared_ptr<util::Scheduler>()> scheduler = &util::make_qt;
#else
static std::function<std::shared_ptr<util::Scheduler>()> scheduler = &util::Scheduler::make_default;
#endif

template <type_info::ObjectPersistable ...Ts>
struct db {
    db(db_config config = {}) : config(std::move(config))
    {
        std::vector<ObjectSchema> schema;

        (schema.push_back(Ts::schema::to_core_schema()), ...);

        m_realm = Realm::get_shared_realm({
            .path = this->config.path,
	        .schema_mode = SchemaMode::AdditiveExplicit,
            .schema = Schema(schema),
            .schema_version = 0,
            .sync_config = this->config.sync_config,
            .scheduler = scheduler()
        });
    }

    void write(std::function<void()>&& block) const
    {
        m_realm->begin_transaction();
        block();
        m_realm->commit_transaction();
    }

    template <type_info::ObjectPersistable T>
    void add(T& object) requires (std::is_same_v<T, Ts> || ...)
    {
        auto actual_schema = *m_realm->schema().find(T::schema::name);
        auto& group = m_realm->read_group();
        auto table = group.get_table(actual_schema.table_key);
        T::schema::add(object, table, m_realm);
    }

    template <type_info::ObjectPersistable T>
    void add(T&& object) requires (std::is_same_v<T, Ts> || ...)
    {
        add(object);
    }

    template <type_info::ObjectPersistable T>
    void remove(T& object) requires (std::is_same_v<T, Ts> || ...)
    {
        auto actual_schema = *m_realm->schema().find(T::schema::name);
        std::vector<FieldValue> dict;
        auto& group = m_realm->read_group();
        auto table = group.get_table(actual_schema.table_key);
        table->remove_object(object.m_obj->get_key());
    }

    template <type_info::ObjectPersistable T>
    results<T> objects() requires (std::is_same_v<T, Ts> || ...)
    {
        return results<T>(Results(m_realm,
                                         m_realm->read_group().get_table(ObjectStore::table_name_for_object_type(T::schema::name))));
    }

    template <type_info::ObjectPersistable T>
    T object(const typename T::schema::PrimaryKeyProperty::Result& primary_key) requires (std::is_same_v<T, Ts> || ...) {
        auto table = m_realm->read_group().get_table(ObjectStore::table_name_for_object_type(T::schema::name));
        return T::schema::create(table->get_object_with_primary_key(primary_key),
                                 m_realm);
    }

    template <type_info::ObjectPersistable T>
    T* object_new(const typename T::schema::PrimaryKeyProperty::Result& primary_key) requires (std::is_same_v<T, Ts> || ...) {
        auto table = m_realm->read_group().get_table(ObjectStore::table_name_for_object_type(T::schema::name));
        return T::schema::create_new(table->get_object_with_primary_key(primary_key),
                                 m_realm);
    }

    template <type_info::ObjectPersistable T>
    T resolve(thread_safe_reference<T>&& tsr) const requires (std::is_same_v<T, Ts> || ...)
    {
        Object object = tsr.m_tsr.template resolve<Object>(m_realm);
        return T::schema::create(object.obj(), object.realm());
    }
    template <type_info::ObjectPersistable T>
    T* resolve_new(thread_safe_reference<T>&& tsr) const requires (std::is_same_v<T, Ts> || ...)
    {
        Object object = tsr.m_tsr.template resolve<Object>(m_realm);
        return T::schema::create_new(object.obj(), object.realm());
    }

#if QT_CORE_LIB
    void schedule(std::function<void()>&& fn)
    {
        static_cast<util::QtMainLoopScheduler&>(*m_realm->scheduler()).schedule(std::move(fn));
    }
#endif
    db_config config;
private:
    db(SharedRealm realm)
    : m_realm(realm)
    {
        config.path = realm->config().path;
        config.sync_config = realm->config().sync_config;
    }
    friend class object;
    template <typename ...Vs>
    friend task<thread_safe_reference<db<Vs...>>> async_open(db_config config);
    template <typename T>
    friend struct thread_safe_reference;
    SharedRealm m_realm;
};

template <type_info::ObjectPersistable ...Ts>
static db<Ts...> open(db_config config = {})
{
    // TODO: Add these flags to core
#if QT_CORE_LIB
    util::Scheduler::set_default_factory(util::make_qt);
#endif
    return db<Ts...>(std::move(config));
}

template <type_info::ObjectPersistable ...Ts>
static task<thread_safe_reference<db<Ts...>>> async_open(db_config config) {
    // TODO: Add these flags to core
#if QT_CORE_LIB
    util::Scheduler::set_default_factory(util::make_qt);
#endif
    std::vector<ObjectSchema> schema;
    (schema.push_back(Ts::schema::to_core_schema()), ...);

    std::shared_ptr<AsyncOpenTask> async_open_task = Realm::get_synchronized_realm({
        .path = config.path,
	    .schema_mode = SchemaMode::AdditiveExplicit,
        .schema = Schema(schema),
        .schema_version = 0,
        .sync_config = config.sync_config
    });
    co_return thread_safe_reference<db<Ts...>>(co_await make_awaitable<ThreadSafeReference>([&async_open_task](auto cb) {
        async_open_task->start(cb);
    }));
}

}
#endif /* Header_h */
