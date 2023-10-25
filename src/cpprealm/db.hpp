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

#include <cpprealm/asymmetric_object.hpp>
#include <cpprealm/flex_sync.hpp>
#include <cpprealm/internal/bridge/async_open_task.hpp>
#include <cpprealm/internal/bridge/schema.hpp>
#include <cpprealm/internal/bridge/sync_session.hpp>
#include <cpprealm/logger.hpp>
#include <cpprealm/results.hpp>
#include <cpprealm/scheduler.hpp>
#include <cpprealm/thread_safe_reference.hpp>
#include <utility>

namespace realm {
    using sync_config = internal::bridge::realm::sync_config;
    using db_config = internal::bridge::realm::config;
    using sync_session = internal::bridge::sync_session;
    using status = internal::bridge::status;

template <typename ...Ts>
struct db {
    using config = db_config;
    explicit db(config&& config)
    {
        std::vector<internal::bridge::object_schema> schema;
        (schema.push_back(Ts::schema.to_core_schema()), ...);
        config.set_schema(schema);
        m_realm = internal::bridge::realm(config);
    }

    sync_subscription_set subscriptions()
    {
        return sync_subscription_set(m_realm);
    }

    void write(std::function<void()>&& block) const
    {
        m_realm.begin_transaction();
        block();
        m_realm.commit_transaction();
    }

    template<template<typename> typename ObjectBase, typename T>
    using ObjectPredicate = std::enable_if_t<std::is_base_of_v<ObjectBase<T>, T> && (std::is_same_v<T, Ts> || ...)>;

    template <typename T>
    ObjectPredicate<object, T>
    add(std::vector<T>& objects)
    {
        auto actual_schema = m_realm.schema().find(T::schema.name);
        auto group = m_realm.read_group();
        auto table = group.get_table(actual_schema.table_key());
        for (auto& object : objects)
            object.manage(table, m_realm);
    }

    template <typename T>
    ObjectPredicate<object, T>
    add(T& object)
    {
        auto actual_schema = m_realm.schema().find(T::schema.name);
        auto group = m_realm.read_group();
        auto table = group.get_table(actual_schema.table_key());
        object.manage(table, m_realm);
    }

    template <typename T>
    ObjectPredicate<object, T>
    add(T&& object)
    {
        add(object);
    }

    template <typename T>
    ObjectPredicate<asymmetric_object, T>
    add(T& object)
    {
        auto actual_schema = m_realm.schema().find(T::schema.name);
        auto group = m_realm.read_group();
        auto table = group.get_table(actual_schema.table_key());
        object.manage(table, m_realm);
    }

    template <typename T>
    void remove(T& object)
    {
        REALM_ASSERT(object.is_managed());
        auto group = m_realm.read_group();
        auto schema = object.m_object->get_object_schema();
        auto table = group.get_table(schema.table_key());
        table.remove_object(object.m_object->get_obj().get_key());
    }

    template <typename T>
    results<T, void> objects()
    {
        return results<T, void>(internal::bridge::results(m_realm, m_realm.read_group().get_table(T::schema.name)));
    }

    template <typename T>
    T object(const typename decltype(T::schema)::PrimaryKeyProperty::Result& primary_key) {
        auto table = m_realm.read_group().get_table(internal::bridge::table_name_for_object_type(T::schema.name));
        return T::schema.create(table->get_object_with_primary_key(primary_key),
                                 m_realm);
    }

    template <typename T>
    T resolve(thread_safe_reference<T>&& tsr)
    {
        T cls;
        cls.assign_accessors(internal::bridge::resolve<internal::bridge::object>(m_realm, std::move(tsr.m_tsr)));
        return cls;
    }

    [[maybe_unused]] bool refresh()
    {
        return m_realm.refresh();
    }

    /**
     An object encapsulating an Atlas App Services "session". Sessions represent the
     communication between the client (and a local Realm file on disk), and the server

     Sessions are always created by the SDK and vended out through various APIs. The
     lifespans of sessions associated with Realms are managed automatically.
    */
    std::optional<sync_session> get_sync_session() const {
        return m_realm.get_sync_session();
    }

private:
    db(internal::bridge::realm realm) //NOLINT
    : m_realm(std::move(realm)) { }
    template <typename>
    friend struct object;
    template <typename T, typename>
    friend struct thread_safe_reference;
    internal::bridge::realm m_realm;
};

template <typename ...Ts>
static db<Ts...> open(db_config&& config)
{
    return db<Ts...>(std::move(config));
}

template <typename ...Ts>
struct async_open_promise : public std::promise<thread_safe_reference<db<Ts...>>> {
private:
    async_open_promise(internal::bridge::async_open_task&& task) // NOLINT(google-explicit-constructor)
    : async_open_task(std::move(task)) {}
    internal::bridge::async_open_task async_open_task;
    template <typename ...Vs>
    friend inline async_open_promise<Vs...> async_open(const typename db<Vs...>::config& config);
};

template <typename ...Ts>
[[nodiscard]] static inline async_open_promise<Ts...> async_open(const typename db<Ts...>::config& config) {
    // TODO: Add these flags to core
    auto c = config;
    std::vector<internal::bridge::object_schema> schema;
    (schema.push_back(Ts::schema.to_core_schema()), ...);
    c.set_schema(schema);
    async_open_promise<Ts...> p = {internal::bridge::realm::get_synchronized_realm(c)};
    p.async_open_task.start([&p](internal::bridge::thread_safe_reference tsr, auto ex) {
        if (ex) {
            p.set_exception(ex);
        } else {
            p.set_value(thread_safe_reference<db<Ts...>>(std::move(tsr)));
        }
    });
    return p;
}
}
#endif /* realm_realm_hpp */
