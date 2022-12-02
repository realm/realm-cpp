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
    template <typename C, typename D>
    struct persisted<std::chrono::time_point<C, D>> : public persisted_base<std::chrono::time_point<C, D>> {
        using persisted_base<std::chrono::time_point<C, D>>::persisted_base;

        persisted& operator=(const std::chrono::time_point<C, D>& o) {
            if (this->is_managed()) {
                this->m_object->obj().template set(
                        this->managed,
                        internal::bridge::timestamp(o));
            } else {
                new (&this->unmanaged) std::chrono::time_point<C, D>(o);
            }
            return *this;
        }

        auto time_since_epoch() const {
            if (this->is_managed()) {
                auto ts = this->m_object->obj().template get<internal::bridge::timestamp>(this->managed);
                return ts.get_time_point().time_since_epoch();
            } else {
                return this->unmanaged.time_since_epoch();
            }
        }
    private:
        template <typename U, typename V, typename S>
        friend persisted<std::chrono::time_point<U, V>>& operator +=(
                persisted<std::chrono::time_point<U, V>>& lhs,
                std::chrono::duration<S> rhs);
    };

    template <typename U, typename V, typename S>
    persisted<std::chrono::time_point<U, V>>& operator +=(
            persisted<std::chrono::time_point<U, V>>& lhs,
            std::chrono::duration<S> rhs)
    {
        if (lhs.is_managed()) {
            auto ts = lhs.m_object->obj().template get<internal::bridge::timestamp>(lhs.managed);
            lhs.m_object->obj().template set(lhs.managed, internal::bridge::timestamp(ts.get_time_point() + rhs));
        } else {
            lhs.unmanaged += rhs;
        }
        return lhs;
    }

    template <typename U, typename V, typename S>
    rbool operator ==(
            persisted<std::chrono::time_point<U, V>>& lhs,
            std::chrono::duration<S> rhs)
    {
        return lhs == rhs;
    }
}

#endif //REALM_PERSISTED_TIMESTAMP_HPP
