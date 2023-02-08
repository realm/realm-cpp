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

#ifndef REALM_PERSISTED_TIMESTAMP_HPP
#define REALM_PERSISTED_TIMESTAMP_HPP

#include "cpprealm/internal/bridge/obj.hpp"
#include <cpprealm/persisted.hpp>

namespace realm {
    using time_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration>;
    template <>
    struct persisted<time_point> : public persisted_primitive_base<time_point> {
        using persisted_primitive_base<time_point>::persisted_primitive_base;

        auto time_since_epoch() const {
            if (this->is_managed()) {
                auto ts = this->m_object->get_obj().template get<internal::bridge::timestamp>(this->managed);
                return ts.get_time_point().time_since_epoch();
            } else {
                return this->unmanaged.time_since_epoch();
            }
        }
    protected:
        static internal::bridge::timestamp serialize(const time_point& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
            return static_cast<internal::bridge::timestamp>(v);
        }
        static time_point deserialize(const internal::bridge::timestamp& v) {
            return v;
        }

        template <typename U, typename V, typename S>
        friend persisted<std::chrono::time_point<U, V>>& operator +=(
                persisted<std::chrono::time_point<U, V>>& lhs,
                std::chrono::duration<S> rhs);

        __cpp_realm_friends
    };

    __cpp_realm_generate_operator(std::chrono::time_point<std::chrono::system_clock>, ==, equal)
    __cpp_realm_generate_operator(std::chrono::time_point<std::chrono::system_clock>, !=, not_equal)
    __cpp_realm_generate_operator(std::chrono::time_point<std::chrono::system_clock>, >, greater)
    __cpp_realm_generate_operator(std::chrono::time_point<std::chrono::system_clock>, <, less)
    __cpp_realm_generate_operator(std::chrono::time_point<std::chrono::system_clock>, >=, greater_equal)
    __cpp_realm_generate_operator(std::chrono::time_point<std::chrono::system_clock>, <=, less_equal)

    template <typename S>
    inline persisted<time_point>& operator +=(
            persisted<time_point>& lhs,
            const std::chrono::duration<S>& rhs)
    {
        if (lhs.is_managed()) {
            auto ts = lhs.m_object->get_obj().template get<internal::bridge::timestamp>(lhs.managed);
            lhs.m_object->get_obj().set(lhs.managed, internal::bridge::timestamp(ts.get_time_point() + rhs));
        } else {
            lhs.unmanaged += rhs;
        }
        return lhs;
    }

    template <typename S>
    rbool operator ==(
            persisted<time_point>& lhs,
            std::chrono::duration<S> rhs)
    {
        return lhs == rhs;
    }
}

#endif //REALM_PERSISTED_TIMESTAMP_HPP
