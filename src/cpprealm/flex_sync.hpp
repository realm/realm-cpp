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
    const std::string identifier;
    // The name representing this subscription, or std::nullopt if not set.
    std::optional<std::string> name;
    // Returns the timestamp of when this subscription was originally created.
    const std::chrono::time_point<std::chrono::system_clock> created_at;
    // Returns the timestamp of the last time this subscription was updated by calling update_query.
    const std::chrono::time_point<std::chrono::system_clock> updated_at;
    // Returns a stringified version of the query associated with this subscription.
    const std::string query_string;
    // Returns the name of the object class of the query for this subscription.
    const std::string object_class_name;
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
    void remove(const std::string& name) {
        bool success = m_subscription_set.erase(name);
        if (success)
            return;
        throw std::logic_error("Subscription cannot be found");
    }

    // Finds a subscription for a given name. Will return `std::nullopt` is subscription does
    // not exist.
    std::optional<SyncSubscription> find(const std::string& name) {
        const sync::Subscription* sub = m_subscription_set.find(name);
        if (sub != nullptr) {
            return SyncSubscription {
                .identifier = sub->id.to_string(),
                .name = sub->name,
                .created_at = sub->created_at.get_time_point(),
                .updated_at = sub->updated_at.get_time_point(),
                .query_string = sub->query_string,
                .object_class_name = sub->object_class_name
            };
        }
        return std::nullopt;
    }

    // Updates a subscription for a given name.
    // Will throw if subscription does not exist.
    // If the `query_fn` parameter is left empty, the subscription will sync *all* objects
    // for the templated class type.
    template <typename T>
    std::enable_if_t<std::is_base_of_v<object<T>, T>>
    update_subscription(const std::string& name, std::optional<std::function<rbool(T&)>>&& query_fn = std::nullopt) {
        remove(name);
        if (query_fn)
            add(name, std::move(query_fn));
    }

    // Removes all subscriptions.
    void clear();


private:
    MutableSyncSubscriptionSet(internal::bridge::realm&, const sync::MutableSubscriptionSet& subscription_set);
    unsigned char m_subscription_set[32]{};
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
    std::optional<SyncSubscription> find(const std::string& name) {
        const sync::Subscription* sub = m_subscription_set.find(name);
        if (sub != nullptr) {
            return SyncSubscription {
                    .identifier = sub->id.to_string(),
                    .name = sub->name,
                    .created_at = sub->created_at.get_time_point(),
                    .updated_at = sub->updated_at.get_time_point(),
                    .query_string = sub->query_string,
                    .object_class_name = sub->object_class_name
            };
        }
        return std::nullopt;
    }

    std::promise<bool> update(std::function<void(MutableSyncSubscriptionSet&)>&& fn);

    explicit SyncSubscriptionSet(internal::bridge::realm& realm);
private:
    template <typename ...Ts>
    friend struct db;
    unsigned char m_subscription_set[32]{};
    std::reference_wrapper<internal::bridge::realm> m_realm;
};

} // namespace realm

#endif /* flex_sync_hpp */
