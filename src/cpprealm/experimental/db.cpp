#include <cpprealm/experimental/db.hpp>
#include <cpprealm/flex_sync.hpp>

namespace realm::experimental {
    ::realm::sync_subscription_set db::subscriptions() {
        return ::realm::sync_subscription_set(m_realm);
    }
} // namespace realm::experimental
