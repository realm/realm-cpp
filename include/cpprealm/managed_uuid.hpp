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

#ifndef CPPREALM_MANAGED_UUID_HPP
#define CPPREALM_MANAGED_UUID_HPP

#include <cpprealm/macros.hpp>
#include <cpprealm/types.hpp>

namespace realm {
    class rbool;
}
namespace realm {

    template<>
    struct managed<realm::uuid> : managed_base {
        using value_type = realm::uuid;
        using managed<realm::uuid>::managed_base::operator=;

        [[nodiscard]] realm::uuid detach() const {
            return m_obj->template get<realm::internal::bridge::uuid>(m_key).operator ::realm::uuid();
        }

        [[nodiscard]] realm::uuid operator *() const {
            return detach();
        }

        [[nodiscard]] operator realm::uuid() const {
            return detach();
        }

        //MARK: -   comparison operators
        rbool operator==(const realm::uuid& rhs) const noexcept;
        rbool operator!=(const realm::uuid& rhs) const noexcept;

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
    struct managed<std::optional<realm::uuid>> : managed_base {
        using value_type = std::optional<realm::uuid>;
        using managed<std::optional<realm::uuid>>::managed_base::operator=;

        [[nodiscard]] std::optional<realm::uuid> detach() const {
            auto v = m_obj->template get_optional<realm::internal::bridge::uuid>(m_key);
            if (v) {
                return v.value().operator ::realm::uuid();
            } else {
                return std::nullopt;
            }
        }

        [[nodiscard]] std::optional<realm::uuid> operator *() const {
            return detach();
        }

        [[nodiscard]] operator std::optional<realm::uuid>() const {
            return detach();
        }

        //MARK: -   comparison operators
        rbool operator==(const std::optional<realm::uuid>& rhs) const noexcept;
        rbool operator!=(const std::optional<realm::uuid>& rhs) const noexcept;

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


#endif//CPPREALM_MANAGED_UUID_HPP
