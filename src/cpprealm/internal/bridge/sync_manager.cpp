#include <cpprealm/internal/bridge/sync_manager.hpp>
#include <realm/object-store/sync/sync_manager.hpp>


namespace realm::internal::bridge {

    sync_manager::sync_manager(const std::shared_ptr<SyncManager> &v) {
        m_manager = v;
    }

    void sync_manager::set_log_level(logger::level level) {
        m_manager->set_log_level(static_cast<util::Logger::Level>(level));
    }
}