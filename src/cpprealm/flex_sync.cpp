#include <cpprealm/flex_sync.hpp>
#include <realm/object-store/shared_realm.hpp>
#include <realm/sync/subscriptions.hpp>

namespace realm::internal::bridge {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
    static_assert(LayoutCheck<storage::SyncSubscriptionSet, sync::SubscriptionSet>{});
    static_assert(LayoutCheck<storage::MutableSyncSubscriptionSet, sync::MutableSubscriptionSet>{});
#elif __i386__
    static_assert(SizeCheck<60, sizeof(sync::SubscriptionSet)>{});
    static_assert(SizeCheck<4, alignof(sync::SubscriptionSet)>{});
    static_assert(SizeCheck<116, sizeof(sync::MutableSubscriptionSet)>{});
    static_assert(SizeCheck<4, alignof(sync::MutableSubscriptionSet)>{});
#elif __x86_64__
    #if defined(__clang__)
    static_assert(SizeCheck<96, sizeof(sync::SubscriptionSet)>{});
    static_assert(SizeCheck<184, sizeof(sync::MutableSubscriptionSet)>{});
    #elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<104, sizeof(sync::SubscriptionSet)>{});
    static_assert(SizeCheck<192, sizeof(sync::MutableSubscriptionSet)>{});
    #endif
    static_assert(SizeCheck<8, alignof(sync::SubscriptionSet)>{});
    static_assert(SizeCheck<8, alignof(sync::MutableSubscriptionSet)>{});
#elif __arm__
    static_assert(SizeCheck<64, sizeof(sync::SubscriptionSet)>{});
    static_assert(SizeCheck<8, alignof(sync::SubscriptionSet)>{});
    static_assert(SizeCheck<136, sizeof(sync::MutableSubscriptionSet)>{});
    static_assert(SizeCheck<8, alignof(sync::MutableSubscriptionSet)>{});
#elif __aarch64__
#if defined(__clang__)
    static_assert(SizeCheck<96, sizeof(sync::SubscriptionSet)>{});
    static_assert(SizeCheck<184, sizeof(sync::MutableSubscriptionSet)>{});
#elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<104, sizeof(sync::SubscriptionSet)>{});
    static_assert(SizeCheck<192, sizeof(sync::MutableSubscriptionSet)>{});
#endif
    static_assert(SizeCheck<8, alignof(sync::SubscriptionSet)>{});
    static_assert(SizeCheck<8, alignof(sync::MutableSubscriptionSet)>{});
#elif _WIN32
    static_assert(SizeCheck<8, alignof(sync::SubscriptionSet)>{});
    static_assert(SizeCheck<8, alignof(sync::MutableSubscriptionSet)>{});

    #if _DEBUG
    static_assert(SizeCheck<120, sizeof(sync::SubscriptionSet)>{});
    static_assert(SizeCheck<208, sizeof(sync::MutableSubscriptionSet)>{});
    #else
    static_assert(SizeCheck<104, sizeof(sync::SubscriptionSet)>{});
    static_assert(SizeCheck<192, sizeof(sync::MutableSubscriptionSet)>{});
    #endif
#endif
} // namespace realm::internal::bridge

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
    mutable_sync_subscription_set& mutable_sync_subscription_set::operator=(const mutable_sync_subscription_set& other) {
        if (this != &other) {
            *reinterpret_cast<sync::MutableSubscriptionSet*>(&m_subscription_set) = *reinterpret_cast<const sync::MutableSubscriptionSet*>(&other.m_subscription_set);
        }
        return *this;
    }

    mutable_sync_subscription_set& mutable_sync_subscription_set::operator=(mutable_sync_subscription_set&& other) {
        if (this != &other) {
            *reinterpret_cast<sync::MutableSubscriptionSet*>(&m_subscription_set) = std::move(*reinterpret_cast<sync::MutableSubscriptionSet*>(&other.m_subscription_set));
        }
        return *this;
    }

    mutable_sync_subscription_set::~mutable_sync_subscription_set() {
        reinterpret_cast<sync::MutableSubscriptionSet*>(&m_subscription_set)->~MutableSubscriptionSet();
    }
    mutable_sync_subscription_set::mutable_sync_subscription_set(internal::bridge::realm& realm,
                                                                 const sync::MutableSubscriptionSet &subscription_set)
            : m_realm(realm)
    {
        new (m_subscription_set) sync::MutableSubscriptionSet(subscription_set);
    }
    void mutable_sync_subscription_set::insert_or_assign(const std::string &name, const internal::bridge::query &query) {
        reinterpret_cast<sync::MutableSubscriptionSet *>(&m_subscription_set)->insert_or_assign(name, query);
    }

    // Removes all subscriptions.
    void mutable_sync_subscription_set::clear() {
        reinterpret_cast<sync::MutableSubscriptionSet *>(&m_subscription_set)->clear();
    }

    sync::MutableSubscriptionSet mutable_sync_subscription_set::get_subscription_set() {
        return *reinterpret_cast<sync::MutableSubscriptionSet *>(&m_subscription_set);
    }

    // Removes a subscription for a given name. Will throw if subscription does
    // not exist.
    void mutable_sync_subscription_set::remove(const std::string& name) {
        auto* set = reinterpret_cast<sync::MutableSubscriptionSet *>(&m_subscription_set);
        if (set->erase(name))
            return;
        throw std::logic_error("Subscription cannot be found");
    }

    // Finds a subscription for a given name. Will return `std::nullopt` is subscription does
    // not exist.
    std::optional<sync_subscription> mutable_sync_subscription_set::find(const std::string& name) {
        auto* set = reinterpret_cast<sync::MutableSubscriptionSet *>(&m_subscription_set);
        if (auto it = set->find(name)) {
            return sync_subscription(*it);
        }
        return std::nullopt;
    }

    sync_subscription_set& sync_subscription_set::operator=(const sync_subscription_set& other) {
        if (this != &other) {
            *reinterpret_cast<sync::SubscriptionSet*>(&m_subscription_set) = *reinterpret_cast<const sync::SubscriptionSet*>(&other.m_subscription_set);
        }
        return *this;
    }

    sync_subscription_set& sync_subscription_set::operator=(sync_subscription_set&& other) {
        if (this != &other) {
            *reinterpret_cast<sync::SubscriptionSet*>(&m_subscription_set) = std::move(*reinterpret_cast<sync::SubscriptionSet*>(&other.m_subscription_set));
        }
        return *this;
    }

    sync_subscription_set::~sync_subscription_set() {
        reinterpret_cast<sync::SubscriptionSet*>(&m_subscription_set)->~SubscriptionSet();
    }

    size_t sync_subscription_set::size() const {
        return reinterpret_cast<const sync::SubscriptionSet *>(&m_subscription_set)->size();
    }

    std::optional<sync_subscription> sync_subscription_set::find(const std::string& name) {
        auto* set = reinterpret_cast<sync::SubscriptionSet *>(&m_subscription_set);
        if (auto it = set->find(name)) {
            return sync_subscription(*it);
        }
        return std::nullopt;
    }

    std::future<bool> sync_subscription_set::update(std::function<void(mutable_sync_subscription_set&)>&& fn) {
        auto* set = reinterpret_cast<sync::SubscriptionSet *>(&m_subscription_set);
        auto mutable_set = mutable_sync_subscription_set(m_realm, set->make_mutable_copy());
        fn(mutable_set);
        reinterpret_cast<sync::SubscriptionSet*>(&m_subscription_set)->~SubscriptionSet();
        new (&m_subscription_set) sync::SubscriptionSet(mutable_set.get_subscription_set().commit());
        std::promise<bool> p;
        std::future<bool> f = p.get_future();

        reinterpret_cast<sync::SubscriptionSet *>(&m_subscription_set)->get_state_change_notification(realm::sync::SubscriptionSet::State::Complete)
                .get_async([p = std::move(p)](const realm::StatusWith<realm::sync::SubscriptionSet::State>& state) mutable noexcept {
                    p.set_value(state == sync::SubscriptionSet::State::Complete);
                });
        return f;
    }

    sync_subscription_set::sync_subscription_set(internal::bridge::realm& realm)
            : m_realm(realm)
    {
        new (&m_subscription_set) sync::SubscriptionSet(static_cast<SharedRealm>(realm)->get_active_subscription_set());
    }
}
