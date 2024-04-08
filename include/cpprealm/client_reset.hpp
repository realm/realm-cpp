////////////////////////////////////////////////////////////////////////////
//
// Copyright 2022 Realm Inc.
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

#ifndef CPPREALM_CLIENT_RESET_HPP
#define CPPREALM_CLIENT_RESET_HPP

#include <cpprealm/internal/bridge/realm.hpp>

namespace realm {
    struct db;

    namespace client_reset {
        /// The manual client reset mode must have the error handler set in the sync_config error handler like so:
        /// `sync_config().set_error_handler`.
        ///
        /// - If no callback is set in `sync_config().set_error_handler`, the client reset will not be handled.
        ///   The application will likely need to be restarted and unsynced local changes may be lost.
        struct manual : internal::bridge::client_reset_mode_base<db> {
            explicit manual();
        };

        /// All unsynchronized local changes are automatically discarded and the local state is
        /// automatically reverted to the most recent state from the server. Unsynchronized changes
        /// can then be recovered in the post-client-reset callback block.
        ///
        /// If ``discard_unsynced_changes`` is enabled but the client reset operation is unable to complete
        /// then the client reset process reverts to manual mode. Example: During a destructive schema change this
        /// mode will fail and invoke the manual client reset handler.
        ///
        /// - parameter before: A callback which passes a frozen copy of the local Realm state prior to client reset.
        /// - parameter after: A callback which passes a frozen copy of the local Realm and a live instance of the remote Realm after client reset.
        struct discard_unsynced_changes : internal::bridge::client_reset_mode_base<db> {
            explicit discard_unsynced_changes(std::function<void(db local)> before,
                                              std::function<void(db local, db remote)> after);
        };

        /// The client device will download a realm which reflects the latest
        /// state of the server after a client reset. A recovery process is run locally in
        /// an attempt to integrate the server version with any local changes from
        /// before the client reset occurred.
        ///
        /// The changes are integrated with the following rules:
        /// 1. Objects created locally that were not synced before client reset will be integrated.
        /// 2. If an object has been deleted on the server, but was modified on the client, the delete takes precedence and the update is discarded
        /// 3. If an object was deleted on the client, but not the server, then the client delete instruction is applied.
        /// 4. In the case of conflicting updates to the same field, the client update is applied.
        ///
        /// If the recovery integration fails, the client reset behavior falls back to `manual` mode.
        /// The recovery integration will fail if the "Client Recovery" setting is not enabled on the server.
        /// Integration may also fail in the event of an incompatible schema change.
        ///
        /// - parameter before: A callback which passes a frozen copy of the local Realm state prior to client reset.
        /// - parameter after: A callback which passes a frozen copy of the local Realm and a live instance of the remote Realm after client reset.
        struct recover_unsynced_changes : internal::bridge::client_reset_mode_base<db> {
            explicit recover_unsynced_changes(std::function<void(db local)> before,
                                              std::function<void(db local, db remote)> after);
        };

        /// The client device will download a realm with objects reflecting the latest version of the server. A recovery
        /// process is run locally in an attempt to integrate the server version with any local changes from before the
        /// client reset occurred.
        ///
        /// The changes are integrated with the following rules:
        /// 1. Objects created locally that were not synced before client reset will be integrated.
        /// 2. If an object has been deleted on the server, but was modified on the client, the delete takes precedence and the update is discarded
        /// 3. If an object was deleted on the client, but not the server, then the client delete instruction is applied.
        /// 4. In the case of conflicting updates to the same field, the client update is applied.
        ///
        /// If the recovery integration fails, the client reset behavior falls back to `discard_unsynced_changes`.
        /// The recovery integration will fail if the "Client Recovery" setting is not enabled on the server.
        /// Integration may also fail in the event of an incompatible schema change.
        ///
        /// - parameter before: A callback which passes a frozen copy of the local Realm state prior to client reset.
        /// - parameter after: A callback which passes a frozen copy of the local Realm and a live instance of the remote Realm after client reset.
        struct recover_or_discard_unsynced_changes : internal::bridge::client_reset_mode_base<db> {
            explicit recover_or_discard_unsynced_changes(std::function<void(db local)> before,
                                                         std::function<void(db local, db remote)> after);
        };
    }

} // namespace realm


#endif//CPPREALM_CLIENT_RESET_HPP
