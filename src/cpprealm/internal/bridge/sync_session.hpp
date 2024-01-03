////////////////////////////////////////////////////////////////////////////
//
// Copyright 2024 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef CPPREALM_BRIDGE_SYNC_SESSION_HPP
#define CPPREALM_BRIDGE_SYNC_SESSION_HPP

#include <memory>
#include <functional>
#include <future>
#include <system_error>

namespace realm {
    class SyncSession;
    namespace internal::bridge {
        struct status;
        struct realm;

        struct sync_session {

            enum class state {
                active,
                dying,
                inactive,
                waiting_for_access_token,
                paused
            };

            enum class connection_state {
                disconnected,
                connecting,
                connected
            };

            // The session's current state.
            enum state state() const;

            // The session's current connection state.
            enum connection_state connection_state() const;

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

            /**
            Temporarily suspend syncronization and disconnect from the server.

            The session will not attempt to connect to Atlas App Services until `resume`
            is called or the Realm file is closed and re-opened.
             */
            void pause();

            /**
            Resume syncronization and reconnect to Atlas App Services after suspending.

            This is a no-op if the session was already active or if the session is invalid.
            Newly created sessions begin in the Active state and do not need to be resumed.
             */
            void resume();

            /**
            Request an immediate reconnection to the server if the session is disconnected.

            Realm automatically reconnects after disconnects with an exponential backoff,
            which is reset when the reachability handler reports a network status change.
            In some scenarios an application may wish to skip the reconnect delay, such as
            when an application receives a wake from background notification, which can be
            done by calling this method. Calling this method is never required.

            This method is asynchronous and merely skips the current reconnect delay, so
            the connection state will still normally be disconnected immediately after
            calling it.

            Has no effect if the session is currently connected.
             */
            void reconnect();

            /**
             Registers a callback that is invoked when the underlying sync session changes
             its connection state
             */
            uint64_t observe_connection_change(std::function<void(enum connection_state old_state, enum connection_state new_state)>&& callback);

            /**
             Unregister a previously registered notifier. If the token is invalid,
             this method does nothing.
             */
            void unregister_connection_change_observer(uint64_t token);

            operator std::weak_ptr<SyncSession>();
        private:
            std::weak_ptr<SyncSession> m_session;
            sync_session(const std::shared_ptr<SyncSession> &);
            friend struct internal::bridge::realm;
        };
    }
}

#endif //CPPREALM_BRIDGE_SYNC_SESSION_HPP
