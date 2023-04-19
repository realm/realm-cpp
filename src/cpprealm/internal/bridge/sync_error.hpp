#ifndef CPP_REALM_BRIDGE_SYNC_ERROR_HPP
#define CPP_REALM_BRIDGE_SYNC_ERROR_HPP

#include <memory>
#include <system_error>

namespace realm {
    struct SyncError;
}

namespace realm::internal::bridge {
    struct sync_error {
        sync_error(SyncError&&);

        [[nodiscard]] bool is_fatal() const;
        /// A consolidated explanation of the error, including a link to the server logs if applicable.
        [[nodiscard]] std::string_view message() const;

        /// The error is a client error, which applies to the client and all its sessions.
        [[nodiscard]] bool is_client_error() const;

        /// The error is a protocol error, which may either be connection-level or session-level.
        [[nodiscard]] bool is_connection_level_protocol_error() const;

        /// The error is a connection-level protocol error.
        [[nodiscard]] bool is_session_level_protocol_error() const;

        /// The error indicates a client reset situation.
        [[nodiscard]] bool is_client_reset_requested() const;
    private:
        std::unique_ptr<SyncError> m_error;
    };
}

#endif //CPP_REALM_BRIDGE_SYNC_ERROR_HPP
