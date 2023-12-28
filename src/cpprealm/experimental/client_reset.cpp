#include <cpprealm/experimental/client_reset.hpp>

#include <cpprealm/experimental/db.hpp>

namespace realm {
    namespace experimental {
        struct db;
    }

    namespace client_reset {
        manual::manual() {
            m_mode = internal::bridge::realm::client_reset_mode::manual;
        };

        discard_unsynced_changes::discard_unsynced_changes(std::function<void(experimental::db local)> before,
                                                           std::function<void(experimental::db local, experimental::db remote)> after) {
            m_before = std::move(before);
            m_after = std::move(after);
            m_mode = internal::bridge::realm::client_reset_mode::discard_local;
        }

        recover_unsynced_changes::recover_unsynced_changes(std::function<void(experimental::db local)> before,
                                                           std::function<void(experimental::db local, experimental::db remote)> after) {
            m_before = std::move(before);
            m_after = std::move(after);
            m_mode = internal::bridge::realm::client_reset_mode::recover;
        }

        recover_or_discard_unsynced_changes::recover_or_discard_unsynced_changes(std::function<void(experimental::db local)> before,
                                                                                 std::function<void(experimental::db local, experimental::db remote)> after) {
            m_before = std::move(before);
            m_after = std::move(after);
            m_mode = internal::bridge::realm::client_reset_mode::recover_or_discard;
        }
    }
}