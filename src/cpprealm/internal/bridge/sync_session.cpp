#include <cpprealm/internal/bridge/sync_session.hpp>
#include <cpprealm/internal/bridge/status.hpp>

#include <realm/object-store/sync/sync_session.hpp>

#include <future>

namespace realm::internal::bridge {

    static_assert(static_cast<int>(::realm::SyncSession::State::Active) == static_cast<int>(sync_session::state::active));
    static_assert(static_cast<int>(::realm::SyncSession::State::Dying) == static_cast<int>(sync_session::state::dying));
    static_assert(static_cast<int>(::realm::SyncSession::State::Inactive) == static_cast<int>(sync_session::state::inactive));
    static_assert(static_cast<int>(::realm::SyncSession::State::WaitingForAccessToken) == static_cast<int>(sync_session::state::waiting_for_access_token));
    static_assert(static_cast<int>(::realm::SyncSession::State::Paused) == static_cast<int>(sync_session::state::paused));

    static_assert(static_cast<int>(::realm::SyncSession::ConnectionState::Disconnected) == static_cast<int>(sync_session::connection_state::disconnected));
    static_assert(static_cast<int>(::realm::SyncSession::ConnectionState::Connecting) == static_cast<int>(sync_session::connection_state::connecting));
    static_assert(static_cast<int>(::realm::SyncSession::ConnectionState::Connected) == static_cast<int>(sync_session::connection_state::connected));

    enum sync_session::state sync_session::state() const {
        if (auto session = m_session.lock()) {
            return static_cast<enum sync_session::state>(session->state());
        }
        throw std::runtime_error("Realm: Error accessing sync_session which has been destroyed.");
    }

    enum sync_session::connection_state sync_session::connection_state() const {
        if (auto session = m_session.lock()) {
            return static_cast<enum sync_session::connection_state>(session->connection_state());
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

    sync_session::operator std::weak_ptr<SyncSession>() {
        return m_session;
    }

    sync_session::sync_session(const std::shared_ptr<SyncSession> &v) {
        m_session = v;
    }

    void sync_session::pause() {
        if (auto session = m_session.lock()) {
            session->pause();
        } else {
            throw std::runtime_error("Realm: Error accessing sync_session which has been destroyed.");
        }
    }

    void sync_session::resume() {
        if (auto session = m_session.lock()) {
            session->resume();
        } else {
            throw std::runtime_error("Realm: Error accessing sync_session which has been destroyed.");
        }
    }

    void sync_session::reconnect() {
        if (auto session = m_session.lock()) {
            session->handle_reconnect();
        } else {
            throw std::runtime_error("Realm: Error accessing sync_session which has been destroyed.");
        }
    }

    uint64_t sync_session::observe_connection_change(std::function<void(enum connection_state old_state, enum connection_state new_state)>&& callback) {
        if (auto session = m_session.lock()) {
            return session->register_connection_change_callback([fn = std::move(callback)](::realm::SyncSession::ConnectionState old_state,
                                                            ::realm::SyncSession::ConnectionState new_state) {
                fn(static_cast<enum sync_session::connection_state>(old_state), static_cast<enum sync_session::connection_state>(new_state));
            });
        } else {
            throw std::runtime_error("Realm: Error accessing sync_session which has been destroyed.");
        }
    }
    void sync_session::unregister_connection_change_observer(uint64_t token) {
        if (auto session = m_session.lock()) {
            session->unregister_connection_change_callback(token);
        } else {
            throw std::runtime_error("Realm: Error accessing sync_session which has been destroyed.");
        }
    }
}