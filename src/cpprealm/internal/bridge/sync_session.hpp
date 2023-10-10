#ifndef CPP_REALM_BRIDGE_SYNC_SESSION_HPP
#define CPP_REALM_BRIDGE_SYNC_SESSION_HPP

#include <memory>
#include <functional>
#include <future>
#include <system_error>

namespace realm {
    class SyncSession;
    namespace internal::bridge {
        struct status;

        struct sync_session {
            enum class state {
                active,
                dying,
                inactive,
                waiting_for_access_token,
            };

            sync_session(const std::shared_ptr<SyncSession> &);
            enum state state() const;
            // Register a callback that will be called when all pending uploads have completed.
            // The callback is run asynchronously, and upon whatever thread the underlying sync client
            // chooses to run it on.
            void wait_for_upload_completion(std::function<void(status)>&& callback);

            // Register a callback that will be called when all pending downloads have been completed.
            // Works the same way as `wait_for_upload_completion()`.
            void wait_for_download_completion(std::function<void(status)>&& callback);
            // Register a callback that will be called when all pending uploads have completed.
            // The callback is run asynchronously, and upon whatever thread the underlying sync client
            // chooses to run it on.
            std::future<void> wait_for_upload_completion();
            // Register a callback that will be called when all pending downloads have been completed.
            std::future<void> wait_for_download_completion();
        private:
            std::weak_ptr<SyncSession> m_session;
        };
    }
}

#endif //CPP_REALM_BRIDGE_SYNC_SESSION_HPP
