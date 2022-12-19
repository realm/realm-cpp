#ifndef CPP_REALM_BRIDGE_SYNC_MANAGER_HPP
#define CPP_REALM_BRIDGE_SYNC_MANAGER_HPP

#include <memory>
#include <cpprealm/internal/bridge/realm.hpp>

namespace realm {
    class SyncManager;

    namespace internal::bridge {

        struct sync_manager {
            sync_manager(const std::shared_ptr<SyncManager> &);
            [[nodiscard]] std::string path_for_realm(const realm::sync_config&) const;
        private:
            std::shared_ptr<SyncManager> m_manager;
        };
    }
}

#endif //CPP_REALM_BRIDGE_SYNC_MANAGER_HPP
