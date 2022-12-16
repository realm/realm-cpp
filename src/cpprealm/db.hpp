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
#include <cpprealm/thread_safe_reference.hpp>
#include <cpprealm/scheduler.hpp>
#include <cpprealm/internal/bridge/schema.hpp>
#include <utility>

namespace realm {

#if QT_CORE_LIB
static std::function<std::shared_ptr<util::Scheduler>()> scheduler = &util::make_qt;
#endif

    using sync_config = internal::bridge::realm::sync_config;
    using db_config = internal::bridge::realm::config;

template <typename ...Ts>
struct db {
    using config = db_config;
    explicit db(config&& config = {})
    {
        std::vector<internal::bridge::object_schema> schema;
        (schema.push_back(Ts::schema.to_core_schema()), ...);
        config.set_schema(schema);
        m_realm = internal::bridge::realm(config);
        static bool initialized;
        if (!initialized) {
            realm_analytics::send();
            initialized = true;
        }
    }

    SyncSubscriptionSet subscriptions()
    {
        return SyncSubscriptionSet(m_realm);
    }

    void write(std::function<void()>&& block) const
    {
        m_realm.begin_transaction();
        block();
        m_realm.commit_transaction();
    }

    template <typename T>
    void add(T& object)
    {
        static_assert(std::is_base_of_v<realm::object, T> && (std::is_same_v<T, Ts> || ...));
        auto actual_schema = m_realm.schema().find(T::schema.name);
        auto group = m_realm.read_group();
        auto table = group.get_table(actual_schema.table_key());
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
        REALM_ASSERT(object.is_managed());
        auto group = m_realm.read_group();
        auto schema = object.m_object->get_object_schema();
        auto table = group.get_table(schema.table_key());
        table.remove_object(object.m_object->obj().get_key());
    }

    template <typename T>
    results<T> objects()
    {
        return results<T>(internal::bridge::results(m_realm,
                                                    m_realm.read_group().get_table(T::schema.name)));
    }

    template <typename T>
    T object(const typename decltype(T::schema)::PrimaryKeyProperty::Result& primary_key) {
        auto table = m_realm.read_group().get_table(internal::bridge::table_name_for_object_type(T::schema.name));
        return T::schema.create(table->get_object_with_primary_key(primary_key),
                                 m_realm);
    }

    template <typename T>
    T resolve(thread_safe_reference<T>&& tsr) const
    {
//        Object object = tsr.m_tsr->template resolve<Object>(m_realm);
//        return T::schema.create(object.obj(), object.realm());
abort();
    }

    [[maybe_unused]] bool refresh()
    {
        return m_realm->refresh();
    }
#endif

private:
    db(internal::bridge::realm realm) //NOLINT
    : m_realm(std::move(realm))
    {

        static bool initialized;
        if (!initialized) {
            realm_analytics::send();
            initialized = true;
        }
    }
    friend struct object;
    template <typename T, typename>
    friend struct thread_safe_reference;
    internal::bridge::realm m_realm;
};

template <typename ...Ts>
static db<Ts...> open(std::string&& path = std::filesystem::current_path().append("default.realm"),
                      std::shared_ptr<scheduler>&& scheduler = scheduler::make_default(),
                      std::optional<sync_config> sync_config = std::nullopt)
{
#if QT_CORE_LIB
    util::Scheduler::set_default_factory(util::make_qt);
#endif
    return db<Ts...>(db_config(path, scheduler, sync_config));
}

template <typename ...Ts>
static inline std::promise<thread_safe_reference<db<Ts...>>> async_open(const typename db<Ts...>::config& config) {
    // TODO: Add these flags to core
#if QT_CORE_LIB
    util::Scheduler::set_default_factory(util::make_qt);
#endif
//    std::vector<object_schema> schema;
//    (schema.push_back(Ts::schema.to_core_schema()), ...);
    std::promise<thread_safe_reference<db<Ts...>>> p;
//
//    auto async_open_task =
//            internal::get_synchronized_realm(config, schema, [&p](std::unique_ptr<ThreadSafeReference> tsr, auto ex) {
//                if (ex) p.set_exception(ex);
//                else p.set_value(thread_safe_reference<db<Ts...>>(std::move(tsr)));
//            });
//
//    static bool initialized;
//    if (!initialized) {
//        realm_analytics::send();
//        initialized = true;
//    }
    return p;
}

}
#endif /* realm_realm_hpp */
