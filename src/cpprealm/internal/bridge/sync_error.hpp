#ifndef CPP_REALM_BRIDGE_SYNC_ERROR_HPP
#define CPP_REALM_BRIDGE_SYNC_ERROR_HPP

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

#endif //CPP_REALM_BRIDGE_SYNC_ERROR_HPP
