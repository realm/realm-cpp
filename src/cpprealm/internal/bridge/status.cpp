#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/status.hpp>

#include <realm/status.hpp>

namespace realm::internal::bridge {

#ifdef __i386__
    static_assert(SizeCheck<4, sizeof(::realm::Status)>{});
    static_assert(SizeCheck<4, alignof(::realm::Status)>{});
    static_assert(SizeCheck<4, sizeof(::realm::ErrorCategory)>{});
    static_assert(SizeCheck<4, alignof(::realm::ErrorCategory)>{});
#elif __arm__
    static_assert(SizeCheck<4, sizeof(::realm::Status)>{});
    static_assert(SizeCheck<4, alignof(::realm::Status)>{});
    static_assert(SizeCheck<4, sizeof(::realm::ErrorCategory)>{});
    static_assert(SizeCheck<4, alignof(::realm::ErrorCategory)>{});
#elif __x86_64__
    static_assert(SizeCheck<8, sizeof(::realm::Status)>{});
    static_assert(SizeCheck<8, alignof(::realm::Status)>{});
    static_assert(SizeCheck<4, sizeof(::realm::ErrorCategory)>{});
    static_assert(SizeCheck<4, alignof(::realm::ErrorCategory)>{});
#elif __aarch64__
    static_assert(SizeCheck<8, sizeof(::realm::Status)>{});
    static_assert(SizeCheck<8, alignof(::realm::Status)>{});
    static_assert(SizeCheck<4, sizeof(::realm::ErrorCategory)>{});
    static_assert(SizeCheck<4, alignof(::realm::ErrorCategory)>{});
#elif _WIN32
    static_assert(SizeCheck<8, sizeof(::realm::Status)>{});
    static_assert(SizeCheck<8, alignof(::realm::Status)>{});
    static_assert(SizeCheck<4, sizeof(::realm::ErrorCategory)>{});
    static_assert(SizeCheck<4, alignof(::realm::ErrorCategory)>{});
#endif

    error_category::error_category() {
        new (&m_error_category) ErrorCategory();
    }
    error_category::error_category(error_category&& other) {
        new (&m_error_category) ErrorCategory(std::move(*reinterpret_cast<ErrorCategory*>(&other.m_error_category)));

    }
    error_category::error_category(const error_category& other) {
        new (&m_error_category) ErrorCategory(*reinterpret_cast<const ErrorCategory*>(&other.m_error_category));
    }
    error_category& error_category::operator=(const error_category& other) {
        *reinterpret_cast<ErrorCategory*>(&m_error_category) = *reinterpret_cast<const ErrorCategory*>(&other.m_error_category);
        return *this;
    }
    error_category& error_category::operator=(error_category&& other) {
        *reinterpret_cast<ErrorCategory*>(&m_error_category) = std::move(*reinterpret_cast<ErrorCategory*>(&other.m_error_category));
        return *this;
    }

    bool error_category::test(type cat) {
        return reinterpret_cast<ErrorCategory*>(&m_error_category)->test(static_cast<::realm::ErrorCategory::Type>(cat));
    }
    error_category& error_category::set(type cat) {
        reinterpret_cast<ErrorCategory*>(&m_error_category)->set(static_cast<::realm::ErrorCategory::Type>(cat));
        return *this;
    }
    void error_category::reset(type cat) {
        reinterpret_cast<ErrorCategory*>(&m_error_category)->reset(static_cast<::realm::ErrorCategory::Type>(cat));
    }
    bool error_category::operator==(const error_category& other) const {
        return reinterpret_cast<const ErrorCategory*>(&m_error_category) == reinterpret_cast<const ErrorCategory*>(&other.m_error_category);
    }
    bool error_category::operator!=(const error_category& other) const {
        return reinterpret_cast<const ErrorCategory*>(&m_error_category) != reinterpret_cast<const ErrorCategory*>(&other.m_error_category);
    }
    int error_category::value() const {
        return reinterpret_cast<const ErrorCategory*>(&m_error_category)->value();
    }

    status::status(const ::realm::Status& other) {
        new (&m_status) Status(other);
    }
    status::status(::realm::Status&& other) {
        new (&m_status) Status(std::move(other));
    }
    status::status(const status& other) {
        new (&m_status) Status(*reinterpret_cast<const Status*>(&other.m_status));
    }
    status::status(status&& other) {
        new (&m_status) Status(std::move(*reinterpret_cast<Status*>(&other.m_status)));
    }
    status& status::operator=(const status& other) {
        *reinterpret_cast<Status*>(&m_status) = *reinterpret_cast<const Status*>(&other.m_status);
        return *this;
    }
    status& status::operator=(status&& other) {
        *reinterpret_cast<Status*>(&m_status) = std::move(*reinterpret_cast<Status*>(&other.m_status));
        return *this;
    }

    status::~status() {
        reinterpret_cast<Status*>(&m_status)->~Status();
    }

    inline bool status::is_ok() const noexcept {
        return reinterpret_cast<const Status*>(&m_status)->is_ok();
    }
    inline const std::string& status::reason() const noexcept {
        return reinterpret_cast<const Status*>(&m_status)->reason();
    }
    inline error_codes::error status::code() const noexcept {
        return static_cast<error_codes::error>(reinterpret_cast<const Status*>(&m_status)->code());
    }
    inline std::string_view status::code_string() const noexcept {
        return reinterpret_cast<const Status*>(&m_status)->code_string();
    }

}