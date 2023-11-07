#include <cpprealm/internal/bridge/sync_error.hpp>
#include <cpprealm/internal/bridge/status.hpp>

#include <realm/sync/config.hpp>

namespace realm::internal::bridge {
    sync_error::sync_error(const sync_error& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_error) SyncError(*reinterpret_cast<const SyncError*>(&other.m_error));
#else
        std::shared_ptr<SyncError> m_error;
#endif
    }

    sync_error& sync_error::operator=(const sync_error& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<SyncError*>(&m_error) = *reinterpret_cast<const SyncError*>(&other.m_error);
        }
#else
        m_error = other.m_error;
#endif
        return *this;
    }

    sync_error::sync_error(sync_error&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_error) SyncError(std::move(*reinterpret_cast<SyncError*>(&other.m_error)));
#else
        m_error = std::move(other.m_error);
#endif
    }

    sync_error& sync_error::operator=(sync_error&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<SyncError*>(&m_error) = std::move(*reinterpret_cast<SyncError*>(&other.m_error));
        }
#else
        m_error = std::move(other.m_error);
#endif
        return *this;
    }

    sync_error::~sync_error() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<SyncError*>(&m_error)->~SyncError();
#endif
    }

    std::string_view sync_error::message() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const SyncError*>(&m_error)->simple_message;
#else
        return m_error->simple_message;
#endif
    }

    bool sync_error::is_client_reset_requested() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const SyncError*>(&m_error)->is_client_reset_requested();
#else
        return m_error->is_client_reset_requested();
#endif
    }

    bool sync_error::is_fatal() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const SyncError*>(&m_error)->is_fatal;
#else
        return m_error->is_fatal;
#endif
    }

    sync_error::sync_error(realm::SyncError &&v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_error) SyncError(std::move(v));
#else
        m_error = std::make_shared<SyncError>(std::move(v));
#endif
    }

    std::unordered_map<std::string, std::string> sync_error::user_info() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const SyncError*>(&m_error)->user_info;
#else
        return m_error->user_info;
#endif
    }

    std::vector<compensating_write_error_info> sync_error::compensating_writes_info() const {
        std::vector<compensating_write_error_info> ret;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        auto info = reinterpret_cast<const SyncError*>(&m_error)->compensating_writes_info;
#else
        auto info = m_error->compensating_writes_info;
#endif
        for (auto& v : info) {
            compensating_write_error_info info;
            info.primary_key = v.primary_key;
            info.reason = v.reason;
            info.object_name = v.object_name;
            ret.push_back(std::move(info));
        }
        return ret;
    };

    status sync_error::get_status() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const SyncError*>(&m_error)->status;
#else
        abort();
//        return reinterpret_cast<const SyncError*>(&m_error)->m_status;
//        return m_error->m_status;
#endif
    }
}
