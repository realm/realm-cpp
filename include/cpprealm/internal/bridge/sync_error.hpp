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

#ifndef CPPREALM_BRIDGE_SYNC_ERROR_HPP
#define CPPREALM_BRIDGE_SYNC_ERROR_HPP

#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/status.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <unordered_map>

namespace realm {
    struct SyncError;
}

namespace realm::internal::bridge {

    struct compensating_write_error_info {
        std::string object_name;
        internal::bridge::mixed primary_key;
        std::string reason;
    };

    struct sync_error {
        sync_error(const sync_error& other) ;
        sync_error& operator=(const sync_error& other) ;
        sync_error(sync_error&& other);
        sync_error& operator=(sync_error&& other);
        ~sync_error();
        sync_error(SyncError&&);

        /// A dictionary of extra user information associated with this error.
        /// If this is a client reset error, the keys for c_original_file_path_key and c_recovery_file_path_key will be
        /// populated with the relevant filesystem paths.
        [[nodiscard]] std::unordered_map<std::string, std::string> user_info() const;
        /// If this error resulted from a compensating write, this vector will contain information about each object
        /// that caused a compensating write and why the write was illegal.
        [[nodiscard]] std::vector<compensating_write_error_info> compensating_writes_info() const;

        [[nodiscard]] status get_status() const;

        [[nodiscard]] bool is_fatal() const;
        /// A consolidated explanation of the error, including a link to the server logs if applicable.
        [[nodiscard]] std::string_view message() const;
        /// The error indicates a client reset situation.
        [[nodiscard]] bool is_client_reset_requested() const;
    private:
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::SyncError m_error[1];
#else
        std::shared_ptr<SyncError> m_error;
#endif
    };
}

#endif //CPPREALM_BRIDGE_SYNC_ERROR_HPP
