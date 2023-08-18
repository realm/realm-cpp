#ifndef REALM_MANAGED_DECIMAL_HPP
#define REALM_MANAGED_DECIMAL_HPP

#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/experimental/types.hpp>

#include <cpprealm/internal/bridge/decimal128.hpp>

namespace realm {
    class rbool;
}

namespace realm::experimental {
    template<>
    struct managed<realm::decimal128> : managed_base {
        using managed<realm::decimal128>::managed_base::operator=;
        [[nodiscard]] realm::decimal128 value() const {
            return m_obj->template get<realm::internal::bridge::decimal128>(m_key).operator ::realm::decimal128();
        }

        [[nodiscard]] realm::decimal128 operator *() const {
            return value();
        }

        [[nodiscard]] operator realm::decimal128 () const {
            return value();
        }

        rbool operator==(const decimal128& rhs) const noexcept;
        rbool operator!=(const decimal128& rhs) const noexcept;
        rbool operator>(const decimal128& rhs) const noexcept;
        rbool operator<(const decimal128& rhs) const noexcept;
        rbool operator>=(const decimal128& rhs) const noexcept;
        rbool operator<=(const decimal128& rhs) const noexcept;
        decimal128 operator+(const decimal128& o);
        decimal128 operator-(const decimal128& o);
        decimal128 operator*(const decimal128& o);
        decimal128 operator/(const decimal128& o);

        managed<realm::decimal128>& operator+=(const decimal128& o);
        managed<realm::decimal128>& operator-=(const decimal128& o);
        managed<realm::decimal128>& operator*=(const decimal128& o);
        managed<realm::decimal128>& operator/=(const decimal128& o);
    };

    template<>
    struct managed<std::optional<realm::decimal128>> : managed_base {
        using managed<std::optional<realm::decimal128>>::managed_base::operator=;

        [[nodiscard]] std::optional<realm::decimal128> value() const {
            auto v = m_obj->template get_optional<realm::internal::bridge::decimal128>(m_key);
            if (v) {
                return v.value().operator ::realm::decimal128();
            } else {
                return std::nullopt;
            }
        }

        [[nodiscard]] std::optional<realm::decimal128> operator *() const {
            return value();
        }

        [[nodiscard]] operator std::optional<realm::decimal128>() const {
            return value();
        }

        rbool operator==(const std::optional<realm::decimal128>& rhs) const noexcept;
        rbool operator!=(const std::optional<realm::decimal128>& rhs) const noexcept;
        rbool operator>(const std::optional<realm::decimal128>& rhs) const noexcept;
        rbool operator<(const std::optional<realm::decimal128>& rhs) const noexcept;
        rbool operator>=(const std::optional<realm::decimal128>& rhs) const noexcept;
        rbool operator<=(const std::optional<realm::decimal128>& rhs) const noexcept;
        decimal128 operator+(const decimal128& o);
        decimal128 operator-(const decimal128& o);
        decimal128 operator*(const decimal128& o);
        decimal128 operator/(const decimal128& o);

        managed<std::optional<realm::decimal128>>& operator+=(const decimal128& o);
        managed<std::optional<realm::decimal128>>& operator-=(const decimal128& o);
        managed<std::optional<realm::decimal128>>& operator*=(const decimal128& o);
        managed<std::optional<realm::decimal128>>& operator/=(const decimal128& o);
    };

} // namespace realm::experimental

#endif //REALM_MANAGED_DECIMAL_HPP
