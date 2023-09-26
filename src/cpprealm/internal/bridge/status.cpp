#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/status.hpp>

#include <realm/status.hpp>

namespace realm::internal::bridge {

    error_category::error_category() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_error_category) ErrorCategory();
#else
        m_error_category = std::make_shared<ErrorCategory>();
#endif
    }
    error_category::error_category(error_category&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_error_category) ErrorCategory(std::move(*reinterpret_cast<ErrorCategory*>(&other.m_error_category)));
#else
        m_error_category = std::move(other.m_error_category);
#endif
    }
    error_category::error_category(const error_category& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_error_category) ErrorCategory(*reinterpret_cast<const ErrorCategory*>(&other.m_error_category));
#else
        m_error_category = other.m_error_category;
#endif
    }
    error_category& error_category::operator=(const error_category& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<ErrorCategory*>(&m_error_category) = *reinterpret_cast<const ErrorCategory*>(&other.m_error_category);
#else
        m_error_category = other.m_error_category;
#endif
        return *this;
    }
    error_category& error_category::operator=(error_category&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<ErrorCategory*>(&m_error_category) = std::move(*reinterpret_cast<ErrorCategory*>(&other.m_error_category));
#else
        m_error_category = std::move(m_error_category);
#endif
        return *this;
    }

    bool error_category::test(type cat) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<ErrorCategory*>(&m_error_category)->test(static_cast<::realm::ErrorCategory::Type>(cat));
#else
        m_error_category->test(static_cast<::realm::ErrorCategory::Type>(cat));
#endif
    }
    error_category& error_category::set(type cat) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<ErrorCategory*>(&m_error_category)->set(static_cast<::realm::ErrorCategory::Type>(cat));
#else
        m_error_category->set(static_cast<::realm::ErrorCategory::Type>(cat));
#endif
        return *this;
    }
    void error_category::reset(type cat) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<ErrorCategory*>(&m_error_category)->reset(static_cast<::realm::ErrorCategory::Type>(cat));
#else
        m_error_category->reset(static_cast<::realm::ErrorCategory::Type>(cat));
#endif
    }
    bool error_category::operator==(const error_category& other) const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const ErrorCategory*>(&m_error_category) == reinterpret_cast<const ErrorCategory*>(&other.m_error_category);
#else
        return *m_error_category == *other.m_error_category;
#endif
    }
    bool error_category::operator!=(const error_category& other) const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const ErrorCategory*>(&m_error_category) != reinterpret_cast<const ErrorCategory*>(&other.m_error_category);
#else
        return *m_error_category != *other.m_error_category;
#endif
    }
    int error_category::value() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const ErrorCategory*>(&m_error_category)->value();
#else
        return m_error_category->value();
#endif
    }

    status::status(const ::realm::Status& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_status) Status(other);
#else
        m_status = std::make_shared<::realm::Status>(other);
#endif
    }
    status::status(::realm::Status&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_status) Status(std::move(other));
#else
        m_status = std::make_shared<::realm::Status>(std::move(other));
#endif
    }
    status::status(const status& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_status) Status(*reinterpret_cast<const Status*>(&other.m_status));
#else
        m_status = other.m_status;
#endif
    }
    status::status(status&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_status) Status(std::move(*reinterpret_cast<Status*>(&other.m_status)));
#else
        m_status = std::move(other.m_status);
#endif
    }
    status& status::operator=(const status& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<Status*>(&m_status) = *reinterpret_cast<const Status*>(&other.m_status);
#else
        m_status = other.m_status;
#endif
        return *this;
    }
    status& status::operator=(status&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<Status*>(&m_status) = std::move(*reinterpret_cast<Status*>(&other.m_status));
#else
        m_status = std::move(other.m_status);
#endif
        return *this;
    }

    status::~status() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<Status*>(&m_status)->~Status();
#endif
    }

    inline bool status::is_ok() const noexcept {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Status*>(&m_status)->is_ok();
#else
        return m_status->is_ok();
#endif
    }
    inline const std::string& status::reason() const noexcept {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Status*>(&m_status)->reason();
#else
        return m_status->reason();
#endif
    }
    inline error_codes::error status::code() const noexcept {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return static_cast<error_codes::error>(reinterpret_cast<const Status*>(&m_status)->code());
#else
        return static_cast<error_codes::error>(m_status->code());
#endif
    }
    inline std::string_view status::code_string() const noexcept {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Status*>(&m_status)->code_string();
#else
        return m_status->code_string();
#endif
    }

}