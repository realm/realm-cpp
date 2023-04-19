#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/sync_error.hpp>

#include <realm/sync/config.hpp>

namespace realm::internal::bridge {
    std::string_view sync_error::message() const {
        return m_error->reason();
    }

    bool sync_error::is_client_error() const {
        return m_error->is_client_error();
    }

    bool sync_error::is_client_reset_requested() const {
        return m_error->is_client_reset_requested();
    }

    bool sync_error::is_connection_level_protocol_error() const {
        return m_error->is_connection_level_protocol_error();
    }

    bool sync_error::is_fatal() const {
        return m_error->is_fatal;
    }

    bool sync_error::is_session_level_protocol_error() const {
        return m_error->is_session_level_protocol_error();
    }

    sync_error::sync_error(realm::SyncError &&v) {
        m_error = std::make_unique<realm::SyncError>(std::move(v));
    }
}
