#include <cpprealm/internal/bridge/sync_session.hpp>

#include <realm/object-store/sync/sync_session.hpp>

namespace realm::internal::bridge {
    enum sync_session::state sync_session::state() const {
        return static_cast<enum sync_session::state>(m_session->state());
    }

    void sync_session::wait_for_download_completion(std::function<void(std::error_code)> &&callback) {
        m_session->wait_for_download_completion([cb = std::move(callback)](auto ec) {
            cb(ec);
        });
    }

    void sync_session::wait_for_upload_completion(std::function<void(std::error_code)> &&callback) {
        m_session->wait_for_upload_completion([cb = std::move(callback)](auto ec) {
            cb(ec);
        });
    }

    sync_session::sync_session(const std::shared_ptr<SyncSession> &v) {
        m_session = v;
    }
}