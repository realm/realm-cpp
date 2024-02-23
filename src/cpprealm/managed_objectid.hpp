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

#ifndef CPPREALM_MANAGED_OBJECTID_HPP
#define CPPREALM_MANAGED_OBJECTID_HPP

#include <cpprealm/macros.hpp>
#include <cpprealm/types.hpp>

#include <cpprealm/internal/bridge/object_id.hpp>

namespace realm {
    class rbool;
}

namespace realm {
    template<>
    struct managed<realm::object_id> : managed_base {
        using managed<realm::object_id>::managed_base::operator=;
        [[nodiscard]] realm::object_id detach() const {
            return m_obj->template get<realm::internal::bridge::object_id>(m_key).operator ::realm::object_id();
        }

        [[nodiscard]] realm::object_id operator *() const {
            return detach();
        }

        [[nodiscard]] operator realm::object_id() const {
            return detach();
        }

        //MARK: -   comparison operators
        rbool operator==(const realm::object_id& rhs) const noexcept;
        rbool operator!=(const realm::object_id& rhs) const noexcept;

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
    struct managed<std::optional<realm::object_id>> : managed_base {
        using managed<std::optional<realm::object_id>>::managed_base::operator=;

        [[nodiscard]] std::optional<realm::object_id> detach() const {
            auto v = m_obj->template get_optional<realm::internal::bridge::object_id>(m_key);
            if (v) {
                return v.value().operator ::realm::object_id();
            } else {
                return std::nullopt;
            }
        }

        [[nodiscard]] std::optional<realm::object_id> operator *() const {
            return detach();
        }

        [[nodiscard]] operator std::optional<realm::object_id>() const {
            return detach();
        }

        //MARK: -   comparison operators
        rbool operator==(const std::optional<realm::object_id>& rhs) const noexcept;
        rbool operator!=(const std::optional<realm::object_id>& rhs) const noexcept;

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

#endif//CPPREALM_MANAGED_OBJECTID_HPP
