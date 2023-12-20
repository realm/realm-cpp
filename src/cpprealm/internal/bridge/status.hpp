#ifndef CPP_REALM_BRIDGE_REALM_STATUS_HPP
#define CPP_REALM_BRIDGE_REALM_STATUS_HPP

#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    class Status;
    struct ErrorCategory;
}
namespace realm::internal::bridge {

    struct error_category {
        enum type {
            logic_error,
            runtime_error,
            invalid_argument,
            file_access,
            system_error,
            app_error,
            client_error,
            json_error,
            service_error,
            http_error,
            custom_error,
            websocket_error,
            sync_error,
        };
        error_category() = default;
        error_category(error_category&&) = default;
        error_category(const error_category&) = default;
        error_category& operator=(const error_category&) = default;
        error_category& operator=(error_category&&) = default;
        ~error_category() = default;
        bool test(type cat);
        error_category& set(type cat);
        void reset(type cat);
        bool operator==(const error_category& other) const;
        bool operator!=(const error_category& other) const;
        int value() const;

    private:
        unsigned m_error_category = 0;
    };

    struct status {

        status(const ::realm::Status&);
        status(::realm::Status&&);
        status(const status&);
        status(status&&);
        status& operator=(const status&);
        status& operator=(status&&);
        ~status();

        bool is_ok() const noexcept;
        const std::string& reason() const noexcept;
        std::string_view code_string() const noexcept;

    private:
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::Status m_status[1];
#else
        std::shared_ptr<Status> m_status;
#endif
    };

} // namespace realm::internal::bridge
#endif//CPP_REALM_BRIDGE_REALM_STATUS_HPP
