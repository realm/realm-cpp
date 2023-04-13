#include <realm/sync/subscriptions.hpp>
#include <cpprealm/flex_sync.hpp>
#include <realm/object-store/shared_realm.hpp>

namespace realm {
    sync_subscription::sync_subscription(const sync::Subscription &v)
    {
        identifier = v.id.to_string();
        name = v.name;
        created_at = v.created_at.get_time_point();
        updated_at = v.updated_at.get_time_point();
        query_string = v.query_string;
        object_class_name = v.object_class_name;
    }
    mutable_sync_subscription_set::mutable_sync_subscription_set(internal::bridge::realm& realm,
                                                                 const sync::MutableSubscriptionSet &subscription_set)
            : m_realm(realm)
    {
        m_subscription_set = std::make_unique<sync::MutableSubscriptionSet>(sync::MutableSubscriptionSet(subscription_set));
    }
    void mutable_sync_subscription_set::insert_or_assign(const std::string &name, const internal::bridge::query &query) {
        m_subscription_set->insert_or_assign(name, query);
    }

    // Removes all subscriptions.
    void mutable_sync_subscription_set::clear() {
        m_subscription_set->clear();
    }

    sync::MutableSubscriptionSet mutable_sync_subscription_set::get_subscription_set() {
        return *m_subscription_set;
    }

    // Removes a subscription for a given name. Will throw if subscription does
    // not exist.
    void mutable_sync_subscription_set::remove(const std::string& name) {
        if (m_subscription_set->erase(name))
            return;
        throw std::logic_error("Subscription cannot be found");
    }

    // Finds a subscription for a given name. Will return `std::nullopt` is subscription does
    // not exist.
    std::optional<sync_subscription> mutable_sync_subscription_set::find(const std::string& name) {
        if (auto it = m_subscription_set->find(name)) {
            return sync_subscription(*it);
        }
        return std::nullopt;
    }

    size_t sync_subscription_set::size() const {
        return m_subscription_set->size();
    }

    std::optional<sync_subscription> sync_subscription_set::find(const std::string& name) {
        if (auto it = m_subscription_set->find(name)) {
            return sync_subscription(*it);
        }
        return std::nullopt;
    }

    std::promise<bool> sync_subscription_set::update(std::function<void(mutable_sync_subscription_set&)>&& fn) {
        auto mutable_set = mutable_sync_subscription_set(m_realm, m_subscription_set->make_mutable_copy());
        fn(mutable_set);
        m_subscription_set.reset(new sync::SubscriptionSet(mutable_set.get_subscription_set().commit()));
        std::promise<bool> p;
        m_subscription_set->get_state_change_notification(realm::sync::SubscriptionSet::State::Complete)
                .get_async([&p](const realm::StatusWith<realm::sync::SubscriptionSet::State>& state) mutable noexcept {
                    p.set_value(state == sync::SubscriptionSet::State::Complete);
                });
        return p;
    }

    sync_subscription_set::sync_subscription_set(internal::bridge::realm& realm)
            : m_realm(realm)
    {
        m_subscription_set = std::make_unique<sync::SubscriptionSet>(static_cast<SharedRealm>(realm)->get_active_subscription_set());
    }
}
