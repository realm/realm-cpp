#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/sync_error.hpp>

#include <realm/sync/config.hpp>

namespace realm::internal::bridge {
#ifdef __i386__
    static_assert(SizeCheck<68, sizeof(SyncError)>{});
    static_assert(SizeCheck<4, alignof(SyncError)>{});
#elif __x86_64__
    #if defined(__clang__)
    static_assert(SizeCheck<128, sizeof(SyncError)>{});
    #elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<144, sizeof(SyncError)>{});
    #endif
    static_assert(SizeCheck<8, alignof(SyncError)>{});
#elif __arm__
    static_assert(SizeCheck<68, sizeof(SyncError)>{});
    static_assert(SizeCheck<4, alignof(SyncError)>{});
#elif __aarch64__
#if defined(__clang__)
    static_assert(SizeCheck<128, sizeof(SyncError)>{});
#elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<144, sizeof(SyncError)>{});
#endif
    static_assert(SizeCheck<8, alignof(SyncError)>{});
#elif _WIN32
    static_assert(SizeCheck<192, sizeof(SyncError)>{});
    static_assert(SizeCheck<8, alignof(SyncError)>{});
#endif

    sync_error::sync_error(const sync_error& other) {
        new (&m_error) SyncError(*reinterpret_cast<const SyncError*>(&other.m_error));
    }

    sync_error& sync_error::operator=(const sync_error& other) {
        if (this != &other) {
            *reinterpret_cast<SyncError*>(&m_error) = *reinterpret_cast<const SyncError*>(&other.m_error);
        }
        return *this;
    }

    sync_error::sync_error(sync_error&& other) {
        new (&m_error) SyncError(std::move(*reinterpret_cast<SyncError*>(&other.m_error)));
    }

    sync_error& sync_error::operator=(sync_error&& other) {
        if (this != &other) {
            *reinterpret_cast<SyncError*>(&m_error) = std::move(*reinterpret_cast<SyncError*>(&other.m_error));
        }
        return *this;
    }

    sync_error::~sync_error() {
        reinterpret_cast<SyncError*>(&m_error)->~SyncError();
    }

    std::string_view sync_error::message() const {
        return reinterpret_cast<const SyncError*>(&m_error)->reason();
    }

    bool sync_error::is_client_error() const {
        return reinterpret_cast<const SyncError*>(&m_error)->is_client_error();
    }

    bool sync_error::is_client_reset_requested() const {
        return reinterpret_cast<const SyncError*>(&m_error)->is_client_reset_requested();
    }

    bool sync_error::is_connection_level_protocol_error() const {
        return reinterpret_cast<const SyncError*>(&m_error)->is_connection_level_protocol_error();
    }

    bool sync_error::is_fatal() const {
        return reinterpret_cast<const SyncError*>(&m_error)->is_fatal;
    }

    bool sync_error::is_session_level_protocol_error() const {
        return reinterpret_cast<const SyncError*>(&m_error)->is_session_level_protocol_error();
    }

    sync_error::sync_error(realm::SyncError &&v) {
        new (&m_error) SyncError(std::move(v));
    }
}
