#include <cpprealm/client_reset.hpp>

#include <cpprealm/db.hpp>

namespace realm {

    namespace client_reset {
        manual::manual() {
            m_mode = client_reset_mode::manual;
        }

        discard_unsynced_changes::discard_unsynced_changes(std::function<void(db local)> before,
                                                           std::function<void(db local, db remote)> after) {
            m_before = std::move(before);
            m_after = std::move(after);
            m_mode = client_reset_mode::discard_unsynced;
        }

        recover_unsynced_changes::recover_unsynced_changes(std::function<void(db local)> before,
                                                           std::function<void(db local, db remote)> after) {
            m_before = std::move(before);
            m_after = std::move(after);
            m_mode = client_reset_mode::recover;
        }

        recover_or_discard_unsynced_changes::recover_or_discard_unsynced_changes(std::function<void(db local)> before,
                                                                                 std::function<void(db local, db remote)> after) {
            m_before = std::move(before);
            m_after = std::move(after);
            m_mode = client_reset_mode::recover_or_discard;
        }
    }

} //namespace realm