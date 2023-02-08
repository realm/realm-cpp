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


#ifndef REALM_PERSISTED_BOOL_HPP
#define REALM_PERSISTED_BOOL_HPP

#include <cpprealm/persisted.hpp>

namespace realm {
    template <>
    struct persisted<bool> : public persisted_primitive_base<bool> {
        operator bool() const; // NOLINT(google-explicit-constructor)

        persisted<bool>() = default;
        persisted<bool>(const bool value) {
            new (&this->unmanaged) bool(value);
        }

        template <typename T>
        std::enable_if_t<std::is_integral_v<T>, persisted&> // NOLINT(misc-unconventional-assign-operator)
        operator=(const T o) {
            if (this->is_managed()) {
                this->m_object->get_obj().template set<bool>(
                        this->managed,
                        static_cast<bool>(o));
            } else {
                new (&this->unmanaged) T(o);
            }
            return *this;
        }
    protected:
        static bool serialize(bool v, const std::optional<internal::bridge::realm>& = std::nullopt);
        static bool deserialize(bool v);

        __cpp_realm_friends
    };

    __cpp_realm_generate_operator(bool, ==, equal)
    __cpp_realm_generate_operator(bool, !=, not_equal)
}

#endif //REALM_PERSISTED_BOOL_HPP
