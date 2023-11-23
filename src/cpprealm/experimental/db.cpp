#include <cpprealm/experimental/db.hpp>
#include <cpprealm/flex_sync.hpp>

namespace realm::experimental {
    ::realm::sync_subscription_set db::subscriptions() {
        return ::realm::sync_subscription_set(m_realm);
    }

    bool db::is_frozen() const {
        return m_realm.is_frozen();
    }
    db db::freeze() {
        return db(m_realm.freeze());
    }

    db db::thaw() {
        return db(m_realm.thaw());
    }

    void db::invalidate() {
        m_realm.invalidate();
    }

    bool operator==(const db& lhs, const db& rhs) {
        return lhs.m_realm == rhs.m_realm;
    }
    bool operator!=(const db& lhs, const db& rhs) {
        return lhs.m_realm != rhs.m_realm;
    }

} // namespace realm::experimental
