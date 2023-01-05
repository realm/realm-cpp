#include <realm/sync/subscriptions.hpp>
#include <cpprealm/flex_sync.hpp>
#include <realm/object-store/shared_realm.hpp>

namespace realm {
#ifdef __i386__
    static_assert(internal::bridge::SizeCheck<60, sizeof(sync::SubscriptionSet)>{});
    static_assert(internal::bridge::SizeCheck<4, alignof(sync::SubscriptionSet)>{});
    static_assert(internal::bridge::SizeCheck<116, sizeof(sync::MutableSubscriptionSet)>{});
    static_assert(internal::bridge::SizeCheck<4, alignof(sync::MutableSubscriptionSet)>{});
#elif __x86_64__
    #if defined(__clang__)
    static_assert(internal::bridge::SizeCheck<96, sizeof(sync::SubscriptionSet)>{});
    static_assert(internal::bridge::SizeCheck<184, sizeof(sync::MutableSubscriptionSet)>{});
    #elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(internal::bridge::SizeCheck<104, sizeof(sync::SubscriptionSet)>{});
    static_assert(internal::bridge::SizeCheck<192, sizeof(sync::MutableSubscriptionSet)>{});
    #endif
    static_assert(internal::bridge::SizeCheck<8, alignof(sync::SubscriptionSet)>{});
    static_assert(internal::bridge::SizeCheck<8, alignof(sync::MutableSubscriptionSet)>{});
#elif __arm__
    static_assert(internal::bridge::SizeCheck<64, sizeof(sync::SubscriptionSet)>{});
    static_assert(internal::bridge::SizeCheck<8, alignof(sync::SubscriptionSet)>{});
    static_assert(internal::bridge::SizeCheck<136, sizeof(sync::MutableSubscriptionSet)>{});
    static_assert(internal::bridge::SizeCheck<8, alignof(sync::MutableSubscriptionSet)>{});
#elif __aarch64__
    static_assert(internal::bridge::SizeCheck<96, sizeof(sync::SubscriptionSet)>{});
    static_assert(internal::bridge::SizeCheck<8, alignof(sync::SubscriptionSet)>{});
    static_assert(internal::bridge::SizeCheck<184, sizeof(sync::MutableSubscriptionSet)>{});
    static_assert(internal::bridge::SizeCheck<8, alignof(sync::MutableSubscriptionSet)>{});
#else
    static_assert(internal::bridge::SizeCheck<60, sizeof(sync::SubscriptionSet)>{});
    static_assert(internal::bridge::SizeCheck<4, alignof(sync::SubscriptionSet)>{});
    static_assert(internal::bridge::SizeCheck<116, sizeof(sync::MutableSubscriptionSet)>{});
    static_assert(internal::bridge::SizeCheck<4, alignof(sync::MutableSubscriptionSet)>{});
#endif
    SyncSubscription::SyncSubscription(const sync::Subscription &v)
    {
        identifier = v.id.to_string();
        name = v.name;
        created_at = v.created_at.get_time_point();
        updated_at = v.updated_at.get_time_point();
        query_string = v.query_string;
        object_class_name = v.object_class_name;
    }
    MutableSyncSubscriptionSet::MutableSyncSubscriptionSet(internal::bridge::realm& realm,
                                                           const sync::MutableSubscriptionSet &subscription_set)
            : m_realm(realm)
    {
        new (m_subscription_set) sync::MutableSubscriptionSet(subscription_set);
    }
    void MutableSyncSubscriptionSet::insert_or_assign(const std::string &name, const internal::bridge::query &query) {
        reinterpret_cast<sync::MutableSubscriptionSet *>(m_subscription_set)->insert_or_assign(name, query);
    }

    // Removes all subscriptions.
    void MutableSyncSubscriptionSet::clear() {
        reinterpret_cast<sync::MutableSubscriptionSet *>(m_subscription_set)->clear();
    }

    sync::MutableSubscriptionSet MutableSyncSubscriptionSet::get_subscription_set() {
        return *reinterpret_cast<sync::MutableSubscriptionSet *>(m_subscription_set);
    }

    // Removes a subscription for a given name. Will throw if subscription does
    // not exist.
    void MutableSyncSubscriptionSet::remove(const std::string& name) {
        auto* set = reinterpret_cast<sync::MutableSubscriptionSet *>(m_subscription_set);
        if (set->erase(name))
            return;
        throw std::logic_error("Subscription cannot be found");
    }

    // Finds a subscription for a given name. Will return `std::nullopt` is subscription does
    // not exist.
    std::optional<SyncSubscription> MutableSyncSubscriptionSet::find(const std::string& name) {
        auto* set = reinterpret_cast<sync::MutableSubscriptionSet *>(m_subscription_set);
        if (auto it = set->find(name)) {
            return SyncSubscription(*it);
        }
        return std::nullopt;
    }

    size_t SyncSubscriptionSet::size() const {
        return reinterpret_cast<const sync::SubscriptionSet *>(m_subscription_set)->size();
    }

    std::optional<SyncSubscription> SyncSubscriptionSet::find(const std::string& name) {
        auto* set = reinterpret_cast<sync::SubscriptionSet *>(m_subscription_set);
        if (auto it = set->find(name)) {
            return SyncSubscription(*it);
        }
        return std::nullopt;
    }

    std::promise<bool> SyncSubscriptionSet::update(std::function<void(MutableSyncSubscriptionSet&)>&& fn) {
        auto* set = reinterpret_cast<sync::SubscriptionSet *>(m_subscription_set);
        auto mutable_set = MutableSyncSubscriptionSet(m_realm, set->make_mutable_copy());
        fn(mutable_set);
        new (&m_subscription_set) sync::SubscriptionSet(mutable_set.get_subscription_set().commit());
        std::promise<bool> p;
        reinterpret_cast<sync::SubscriptionSet *>(m_subscription_set)->get_state_change_notification(realm::sync::SubscriptionSet::State::Complete)
                .get_async([&p](const realm::StatusWith<realm::sync::SubscriptionSet::State>& state) mutable noexcept {
                    p.set_value(state == sync::SubscriptionSet::State::Complete);
                });
        return p;
    }

    SyncSubscriptionSet::SyncSubscriptionSet(internal::bridge::realm& realm)
            : m_realm(realm)
    {
        new (&m_subscription_set) sync::SubscriptionSet(static_cast<SharedRealm>(realm)->get_active_subscription_set());
    }
}