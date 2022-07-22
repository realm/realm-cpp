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

namespace realm {

struct SyncSubscriptionSet {
public:

    struct Subscription {
        const std::string identifier() const {
            return m_subscription->id().to_string();
        }

        std::string_view name() const {
            return m_subscription->name();
        }

        const std::chrono::time_point<std::chrono::system_clock> created_at() const {
            return m_subscription->created_at().get_time_point();
        }

        const std::chrono::time_point<std::chrono::system_clock> updated_at() const {
            return m_subscription->updated_at().get_time_point();
        }

        std::string_view query_string() const {
            return m_subscription->query_string();
        }

        std::string_view object_class_name() const {
            return m_subscription->object_class_name();
        }

        std::unique_ptr<sync::Subscription> m_subscription;
    };

    struct MutableSyncSubscriptionSet {
    public:
        template <type_info::ObjectPersistable T>
        void add(const std::string& name, std::optional<std::function<rbool(T&)>> query_fn = std::nullopt) {
            auto schema = *m_realm->schema().find(T::schema::name);
            auto& group = m_realm->read_group();
            auto table_ref = group.get_table(schema.table_key);
            auto builder = Query(table_ref);

            if (query_fn) {
                auto q = query<T>(builder, std::move(schema));
                auto full_query = (*query_fn)(q).q;
                m_subscription_set.insert_or_assign(name, full_query);
            } else {
                m_subscription_set.insert_or_assign(name, builder);
            }
        }

        void remove(const std::string& name) {
            auto it = m_subscription_set.find(name);
            if (it != m_subscription_set.end()) {
                m_subscription_set.erase(it);
                return;
            }
            throw std::logic_error("Subscription cannot be found");
        }

        std::optional<Subscription> find(const std::string& name) {
            auto it = m_subscription_set.find(name);
            if (it != m_subscription_set.end()) {
                return Subscription {std::make_unique<realm::sync::Subscription>(*it)};
            }
            return std::nullopt;
        }

        template <type_info::ObjectPersistable T>
        void update_subscription(const std::string& name, std::optional<std::function<rbool(T&)>> query_fn = std::nullopt) {
            remove(name);
            add(name, query_fn);
        }

        void clear() {
            m_subscription_set.clear();
        }

        sync::MutableSubscriptionSet get_subscription_set()
        {
            return m_subscription_set;
        }

        MutableSyncSubscriptionSet(sync::MutableSubscriptionSet subscription_set, SharedRealm realm) :
        m_subscription_set(subscription_set)
        , m_realm(realm) {}
    private:
        sync::MutableSubscriptionSet m_subscription_set;
        SharedRealm m_realm;
    };

    size_t size() const {
        return m_subscription_set.size();
    }

    std::optional<Subscription> find(const std::string& name) {
        auto it = m_subscription_set.find(name);
        if (it != m_subscription_set.end()) {
            return Subscription {std::make_unique<realm::sync::Subscription>(*it)};
        }
        return std::nullopt;
    }

    task<bool> update(std::function<void(MutableSyncSubscriptionSet&)> fn) {
        auto mutable_set = MutableSyncSubscriptionSet(m_subscription_set.make_mutable_copy(), m_realm);
        fn(mutable_set);
        m_subscription_set = mutable_set.get_subscription_set().commit();

        auto success = co_await make_awaitable<bool>([&] (auto cb) {
            m_subscription_set
                .get_state_change_notification(realm::sync::SubscriptionSet::State::Complete)
                .get_async([cb = std::move(cb)](realm::StatusWith<realm::sync::SubscriptionSet::State> state) mutable noexcept {
                    cb(state == sync::SubscriptionSet::State::Complete);
            });
        });
        co_return success;
    }
private:
    template <type_info::ObjectPersistable ...Ts>
    friend struct db;
    SyncSubscriptionSet(sync::SubscriptionSet&& subscription_set, SharedRealm realm) : m_subscription_set(std::move(subscription_set)), m_realm(realm) {}
    sync::SubscriptionSet m_subscription_set;
    SharedRealm m_realm;
};

} // namespace realm

#endif /* flex_sync_hpp */
