#ifndef REALM_MANAGED_TIMESTAMP_HPP
#define REALM_MANAGED_TIMESTAMP_HPP

#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/internal/bridge/timestamp.hpp>

namespace realm::experimental {

    template<>
    struct managed<std::chrono::time_point<std::chrono::system_clock>> : public managed_base {
        using managed<std::chrono::time_point<std::chrono::system_clock>>::managed_base::operator=;

        [[nodiscard]] std::chrono::time_point<std::chrono::system_clock> value() const {
            return m_obj->template get<realm::internal::bridge::timestamp>(m_key);
        }

        [[nodiscard]] std::chrono::time_point<std::chrono::system_clock> operator *() const {
            return value();
        }

        //MARK: -   comparison operators
        inline bool operator==(const std::chrono::time_point<std::chrono::system_clock>& rhs) const noexcept {
            return value() == rhs;
        }
        inline bool operator!=(const std::chrono::time_point<std::chrono::system_clock>& rhs) const noexcept {
            return value() != rhs;
        }
    };

    template<>
    struct managed<std::optional<std::chrono::time_point<std::chrono::system_clock>>> : managed_base {
        using managed<std::optional<std::chrono::time_point<std::chrono::system_clock>>>::managed_base::operator=;

        [[nodiscard]] std::optional<std::chrono::time_point<std::chrono::system_clock>> value() const {
            return m_obj->template get<realm::internal::bridge::timestamp>(m_key);
        }

        [[nodiscard]] std::optional<std::chrono::time_point<std::chrono::system_clock>> operator *() const {
            return value();
        }

        //MARK: -   comparison operators
        inline bool operator==(const std::optional<std::chrono::time_point<std::chrono::system_clock>>& rhs) const noexcept {
            return value() == rhs;
        }
        inline bool operator!=(const std::optional<std::chrono::time_point<std::chrono::system_clock>>& rhs) const noexcept {
            return value() != rhs;
        }
    };

} // namespace realm::experimental


#endif//REALM_MANAGED_TIMESTAMP_HPP
