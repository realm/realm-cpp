#include <cpprealm/internal/bridge/sync_session.hpp>
#include <cpprealm/internal/bridge/status.hpp>

#include <realm/object-store/sync/sync_session.hpp>

#include <future>

namespace realm::internal::bridge {

    enum sync_session::state sync_session::state() const {
        if (auto session = m_session.lock()) {
            return static_cast<enum sync_session::state>(session->state());
        }
        throw std::runtime_error("Realm: Error accessing sync_session which has been destroyed.");
    }

    void sync_session::wait_for_download_completion(std::function<void(status)> &&callback) {
        if (auto session = m_session.lock()) {
            session->wait_for_download_completion([cb = std::move(callback)](::realm::Status s) {
                cb(std::move(s));
            });
        } else {
            throw std::runtime_error("Realm: Error accessing sync_session which has been destroyed.");
        }
    }

    void sync_session::wait_for_upload_completion(std::function<void(status)> &&callback) {
        if (auto session = m_session.lock()) {
            session->wait_for_upload_completion([cb = std::move(callback)](::realm::Status s) {
                cb(std::move(s));
            });
        } else {
            throw std::runtime_error("Realm: Error accessing sync_session which has been destroyed.");
        }
    }

    std::future<void> sync_session::wait_for_upload_completion() {
        std::promise<void> p;
        std::future<void> f = p.get_future();

        if (auto session = m_session.lock()) {
            session->wait_for_upload_completion([p = std::move(p)](::realm::Status s) mutable {
                if (s.is_ok()) {
                    p.set_value();
                }
                else {
                    p.set_exception(std::make_exception_ptr(s.code()));
                }
            });
        } else {
            p.set_exception(std::make_exception_ptr(std::runtime_error("Realm: Error accessing sync_session which has been destroyed.")));
        }

        return f;
    }

    std::future<void> sync_session::wait_for_download_completion() {
        std::promise<void> p;
        std::future<void> f = p.get_future();
        if (auto session = m_session.lock()) {
            session->wait_for_download_completion([p = std::move(p)](::realm::Status s) mutable {
                if (s.is_ok()) {
                    p.set_value();
                }
                else {
                    p.set_exception(std::make_exception_ptr(s.code()));
                }
            });
        } else {
            p.set_exception(std::make_exception_ptr(std::runtime_error("Realm: Error accessing sync_session which has been destroyed.")));
        }
        return f;
    }

    sync_session::sync_session(const std::shared_ptr<SyncSession> &v) {
        m_session = v;
    }
}