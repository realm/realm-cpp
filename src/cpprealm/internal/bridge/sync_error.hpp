#ifndef CPP_REALM_BRIDGE_SYNC_ERROR_HPP
#define CPP_REALM_BRIDGE_SYNC_ERROR_HPP

#include <system_error>
#include <string>

namespace realm {
    struct SyncError;
}

namespace realm::internal::bridge {
    struct sync_error {
        sync_error(const sync_error& other) ;
        sync_error& operator=(const sync_error& other) ;
        sync_error(sync_error&& other);
        sync_error& operator=(sync_error&& other);
        ~sync_error();
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
#ifdef __i386__
        std::aligned_storage<68, 4>::type m_error[1];
#elif __x86_64__
    #if defined(__clang__)
        std::aligned_storage<128, 8>::type m_error[1];
    #elif defined(__GNUC__) || defined(__GNUG__)
        std::aligned_storage<144, 8>::type m_error[1];
    #endif
#elif __arm__
        std::aligned_storage<80, 4>::type m_error[1];
#elif __aarch64__
#if defined(__clang__)
        std::aligned_storage<128, 8>::type m_error[1];
#elif defined(__GNUC__) || defined(__GNUG__)
        std::aligned_storage<144, 8>::type m_error[1];
#endif
#elif _WIN32
        std::aligned_storage<192, 8>::type m_error[1];
#endif
    };
}

#endif //CPP_REALM_BRIDGE_SYNC_ERROR_HPP
