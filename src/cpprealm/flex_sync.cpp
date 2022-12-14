#include <realm/sync/subscriptions.hpp>
#include <cpprealm/flex_sync.hpp>
#include <realm/object-store/shared_realm.hpp>

namespace realm {
    std::string_view SyncSubscription::object_class_name() {
        return reinterpret_cast<sync::Subscription*>(m_subscription)->object_class_name();
    }

    SyncSubscription::SyncSubscription(const sync::Subscription &v) {
        new (m_subscription) sync::Subscription(v);
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
        auto it = set->find(name);
        if (it != set->end()) {
            set->erase(it);
            return;
        }
        throw std::logic_error("Subscription cannot be found");
    }

    // Finds a subscription for a given name. Will return `std::nullopt` is subscription does
    // not exist.
    std::optional<SyncSubscription> MutableSyncSubscriptionSet::find(const std::string& name) {
        auto* set = reinterpret_cast<sync::MutableSubscriptionSet *>(m_subscription_set);
        auto it = set->find(name);
        if (it != set->end()) {
            return SyncSubscription(*it);
        }
        return std::nullopt;
    }

    size_t SyncSubscriptionSet::size() const {
        return reinterpret_cast<const sync::SubscriptionSet *>(m_subscription_set)->size();
    }

    std::optional<SyncSubscription> SyncSubscriptionSet::find(const std::string& name) {
        auto* set = reinterpret_cast<sync::SubscriptionSet *>(m_subscription_set);
        auto it = set->find(name);
        if (it != set->end()) {
            return SyncSubscription(*it);
        }
        return std::nullopt;
    }

    std::promise<bool> SyncSubscriptionSet::update(std::function<void(MutableSyncSubscriptionSet&)>&& fn) {
        auto* set = reinterpret_cast<sync::SubscriptionSet *>(m_subscription_set);
        auto mutable_set = MutableSyncSubscriptionSet(m_realm, set->make_mutable_copy());
        fn(mutable_set);
        new (&m_subscription_set) sync::SubscriptionSet(mutable_set.get_subscription_set().commit());
        set = reinterpret_cast<sync::SubscriptionSet *>(m_subscription_set);
        std::promise<bool> p;
        set->get_state_change_notification(realm::sync::SubscriptionSet::State::Complete)
                .get_async([&p](const realm::StatusWith<realm::sync::SubscriptionSet::State>& state) mutable noexcept {
                    p.set_value(state == sync::SubscriptionSet::State::Complete);
                });
        return p;
    }

    SyncSubscriptionSet::SyncSubscriptionSet(internal::bridge::realm &realm)
    : m_realm(realm)
    {
        new (&m_subscription_set) sync::SubscriptionSet(static_cast<SharedRealm>(realm)->get_active_subscription_set());
    }
}