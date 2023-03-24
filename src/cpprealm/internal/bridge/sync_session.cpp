#include <cpprealm/internal/bridge/sync_session.hpp>

#include <realm/object-store/sync/sync_session.hpp>

namespace realm::internal::bridge {
    enum sync_session::state sync_session::state() const {
        if (auto session = m_session.lock()) {
            return static_cast<enum sync_session::state>(session->state());
        }
        throw std::runtime_error("Realm: Error accessing sync_session which has been destroyed.");
    }

    void sync_session::wait_for_download_completion(std::function<void(std::error_code)> &&callback) {
        if (auto session = m_session.lock()) {
            session->wait_for_download_completion([cb = std::move(callback)](auto ec) {
                cb(ec.get_std_error_code());
            });
        } else {
            throw std::runtime_error("Realm: Error accessing sync_session which has been destroyed.");
        }
    }

    void sync_session::wait_for_upload_completion(std::function<void(std::error_code)> &&callback) {
        if (auto session = m_session.lock()) {
            session->wait_for_upload_completion([cb = std::move(callback)](auto ec) {
                cb(ec.get_std_error_code());
            });
        } else {
            throw std::runtime_error("Realm: Error accessing sync_session which has been destroyed.");
        }
    }

    std::promise<void> sync_session::wait_for_upload_completion() {
        std::promise<void> p;
        wait_for_upload_completion([&p](std::error_code ec) {
            if (ec) {
                p.set_exception(std::make_exception_ptr(ec));
            }
            else {
                p.set_value();
            }
        });
        return p;
    }

    std::promise<void> sync_session::wait_for_download_completion() {
        std::promise<void> p;
        wait_for_download_completion([&p](std::error_code ec) {
            if (ec) {
                p.set_exception(std::make_exception_ptr(ec));
            }
            else {
                p.set_value();
            }
        });
        return p;
    }

    sync_session::sync_session(const std::shared_ptr<SyncSession> &v) {
        m_session = v;
    }
}