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

#ifndef flex_sync_hpp
#define flex_sync_hpp

#include <future>

#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/bridge/query.hpp>
#include <cpprealm/internal/bridge/schema.hpp>
#include <cpprealm/internal/bridge/obj.hpp>

#include <cpprealm/results.hpp>

namespace realm {
    template <typename>
    struct object;
    struct rbool;

    namespace sync {
        class MutableSubscriptionSet;
        class SubscriptionSet;
        class SubscriptionStore;
        class Subscription;
    }

// A SyncSubscription represents a single query that may be OR'd with other queries on the same object class to be
// send to the server in a QUERY or IDENT message.
    struct SyncSubscription {
        // Returns the unique ID for this subscription.
        std::string identifier;
        // The name representing this subscription.
        std::optional<std::string> name;
        // Returns the timestamp of when this subscription was originally created.
        std::chrono::time_point<std::chrono::system_clock> created_at;
        // Returns the timestamp of the last time this subscription was updated by calling update_query.
        std::chrono::time_point<std::chrono::system_clock> updated_at;
        // Returns a stringified version of the query associated with this subscription.
        std::string_view query_string;
        // Returns the name of the object class of the query for this subscription.
        std::string_view object_class_name;
    private:
        SyncSubscription(const sync::Subscription&);

        friend struct SyncSubscriptionSet;
        friend struct MutableSyncSubscriptionSet;
    };

// A MutableSyncSubscriptionSet represents a single query that may be OR'd with other queries on the same object class to be
// send to the server in a QUERY or IDENT message.
    struct MutableSyncSubscriptionSet {
    private:
        void insert_or_assign(const std::string& name, const internal::bridge::query&);
    public:
        // Inserts a new subscription into the set if one does not exist already.
        // If the `query_fn` parameter is left empty, the subscription will sync *all* objects
        // for the templated class type.
        template <typename T>
        std::enable_if_t<std::is_base_of_v<object<T>, T>>
        add(const std::string& name,
            std::optional<std::function<rbool(T&)>>&& query_fn = std::nullopt) {
            auto schema = m_realm.get().schema().find(T::schema.name);
            auto group = m_realm.get().read_group();
            auto table_ref = group.get_table(schema.table_key());
            auto builder = internal::bridge::query(table_ref);

            if (query_fn) {
                auto q = realm::query<T>(builder, std::move(schema));
                auto full_query = (*query_fn)(q).q;
                insert_or_assign(name, full_query);
            } else {
                insert_or_assign(name, builder);
            }
        }

        // Removes a subscription for a given name. Will throw if subscription does
        // not exist.
        void remove(const std::string& name);

        // Finds a subscription for a given name. Will return `std::nullopt` is subscription does
        // not exist.
        std::optional<SyncSubscription> find(const std::string& name);

        // Updates a subscription for a given name.
        // Will throw if subscription does not exist.
        // If the `query_fn` parameter is left empty, the subscription will sync *all* objects
        // for the templated class type.
        template <typename T>
        std::enable_if_t<std::is_base_of_v<object<T>, T>>
        update_subscription(const std::string& name, std::optional<std::function<rbool(T&)>>&& query_fn = std::nullopt) {
            remove(name);
            add(name, std::move(query_fn));
        }

        // Removes all subscriptions.
        void clear();


    private:
        MutableSyncSubscriptionSet(internal::bridge::realm&, const sync::MutableSubscriptionSet& subscription_set);
#ifdef __i386__
        std::aligned_storage<116, 4>::type m_subscription_set[1];
#elif __x86_64__
    #if defined(__clang__)
        std::aligned_storage<184, 8>::type m_subscription_set[1];
    #elif defined(__GNUC__) || defined(__GNUG__)
        std::aligned_storage<192, 8>::type m_subscription_set[1];
    #endif
#elif __arm__
    std::aligned_storage<136, 8>::type m_subscription_set[1];
#elif __aarch64__
    std::aligned_storage<184, 8>::type m_subscription_set[1];
#endif
        std::reference_wrapper<internal::bridge::realm> m_realm;
        friend struct SyncSubscriptionSet;
        sync::MutableSubscriptionSet get_subscription_set();
    };

    struct SyncSubscriptionSet {
    public:
        /// The total number of subscriptions in the set.
        [[nodiscard]] size_t size() const;

        // Finds a subscription for a given name. Will return `std::nullopt` is subscription does
        // not exist.
        std::optional<SyncSubscription> find(const std::string& name);

        std::promise<bool> update(std::function<void(MutableSyncSubscriptionSet&)>&& fn);

        explicit SyncSubscriptionSet(internal::bridge::realm& realm);
    private:
        template <typename ...Ts>
        friend struct db;
#ifdef __i386__
        std::aligned_storage<60, 4>::type m_subscription_set[1];
#elif __x86_64__
    #if defined(__clang__)
        std::aligned_storage<96, 8>::type m_subscription_set[1];
    #elif defined(__GNUC__) || defined(__GNUG__)
        std::aligned_storage<104, 8>::type m_subscription_set[1];
    #endif
#elif __arm__
        std::aligned_storage<64, 8>::type m_subscription_set[1];
#elif __aarch64__
        std::aligned_storage<96, 8>::type m_subscription_set[1];
#endif
        std::reference_wrapper<internal::bridge::realm> m_realm;
    };

} // namespace realm

#endif /* flex_sync_hpp */
