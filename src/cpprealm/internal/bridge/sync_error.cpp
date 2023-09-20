#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/sync_error.hpp>
#include <cpprealm/internal/bridge/status.hpp>

#include <realm/sync/config.hpp>

namespace realm::internal::bridge {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
    static_assert(LayoutCheck<storage::SyncError, SyncError>{});
#elif __i386__
    static_assert(SizeCheck<68, sizeof(SyncError)>{});
    static_assert(SizeCheck<4, alignof(SyncError)>{});
#elif __x86_64__
    #if defined(__clang__)
    static_assert(SizeCheck<120, sizeof(SyncError)>{});
    #elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<136, sizeof(SyncError)>{});
    #endif
    static_assert(SizeCheck<8, alignof(SyncError)>{});
#elif __arm__
    static_assert(SizeCheck<64, sizeof(SyncError)>{});
    static_assert(SizeCheck<4, alignof(SyncError)>{});
#elif __aarch64__
#if defined(__clang__)
    static_assert(SizeCheck<120, sizeof(SyncError)>{});
#elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<136, sizeof(SyncError)>{});
#endif
    static_assert(SizeCheck<8, alignof(SyncError)>{});
#elif _WIN32
    #if _DEBUG
    static_assert(SizeCheck<168, sizeof(SyncError)>{});
    #else
    static_assert(SizeCheck<144, sizeof(SyncError)>{});
    #endif
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
        return reinterpret_cast<const SyncError*>(&m_error)->simple_message;
    }

    bool sync_error::is_client_reset_requested() const {
        return reinterpret_cast<const SyncError*>(&m_error)->is_client_reset_requested();
    }

    bool sync_error::is_fatal() const {
        return reinterpret_cast<const SyncError*>(&m_error)->is_fatal;
    }

    sync_error::sync_error(realm::SyncError &&v) {
        new (&m_error) SyncError(std::move(v));
    }

    std::unordered_map<std::string, std::string> sync_error::user_info() const {
        return reinterpret_cast<const SyncError*>(&m_error)->user_info;
    }

    std::vector<compensating_write_error_info> sync_error::compensating_writes_info() const {
        std::vector<compensating_write_error_info> ret;
        for (auto& v : reinterpret_cast<const SyncError*>(&m_error)->compensating_writes_info) {
            compensating_write_error_info info;
            info.primary_key = v.primary_key;
            info.reason = v.reason;
            info.object_name = v.object_name;
            ret.push_back(std::move(info));
        }
        return ret;
    };

    status sync_error::get_status() const {
        return reinterpret_cast<const SyncError*>(&m_error)->status;
    }
}
