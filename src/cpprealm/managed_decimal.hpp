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

#ifndef CPPREALM_MANAGED_DECIMAL_HPP
#define CPPREALM_MANAGED_DECIMAL_HPP

#include <cpprealm/macros.hpp>
#include <cpprealm/types.hpp>

#include <cpprealm/internal/bridge/decimal128.hpp>

namespace realm {
    class rbool;
}

namespace realm {
    template<>
    struct managed<realm::decimal128> : managed_base {
        using managed<realm::decimal128>::managed_base::operator=;
        [[nodiscard]] realm::decimal128 detach() const {
            return m_obj->template get<realm::internal::bridge::decimal128>(m_key).operator ::realm::decimal128();
        }

        [[nodiscard]] realm::decimal128 operator *() const {
            return detach();
        }

        [[nodiscard]] operator realm::decimal128 () const {
            return detach();
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

    private:
        managed() = default;
        managed(const managed&) = delete;
        managed(managed &&) = delete;
        managed& operator=(const managed&) = delete;
        managed& operator=(managed&&) = delete;
        template<typename, typename>
        friend struct managed;
    };

    template<>
    struct managed<std::optional<realm::decimal128>> : managed_base {
        using managed<std::optional<realm::decimal128>>::managed_base::operator=;

        [[nodiscard]] std::optional<realm::decimal128> detach() const {
            auto v = m_obj->template get_optional<realm::internal::bridge::decimal128>(m_key);
            if (v) {
                return v.value().operator ::realm::decimal128();
            } else {
                return std::nullopt;
            }
        }

        [[nodiscard]] std::optional<realm::decimal128> operator *() const {
            return detach();
        }

        [[nodiscard]] operator std::optional<realm::decimal128>() const {
            return detach();
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

    private:
        managed() = default;
        managed(const managed&) = delete;
        managed(managed &&) = delete;
        managed& operator=(const managed&) = delete;
        managed& operator=(managed&&) = delete;
        template<typename, typename>
        friend struct managed;
    };

} // namespace realm

#endif //CPPREALM_MANAGED_DECIMAL_HPP
