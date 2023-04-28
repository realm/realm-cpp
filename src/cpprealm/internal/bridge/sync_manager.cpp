#include <cpprealm/internal/bridge/sync_manager.hpp>
#include <realm/object-store/sync/sync_manager.hpp>

namespace realm::internal::bridge {
    std::string sync_manager::path_for_realm(const realm::sync_config &v) const {
        return m_manager->path_for_realm(*static_cast<std::shared_ptr<SyncConfig>>(v));
    }

    sync_manager::sync_manager(const std::shared_ptr<SyncManager> &v) {
        m_manager = v;
    }

    void sync_manager::set_log_level(logger::level level) {
        m_manager->set_log_level(static_cast<util::Logger::Level>(level));
    }
}