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

#include <cpprealm/persisted.hpp>

namespace realm {
    template<typename T>
    struct persisted<T, type_info::TimestampPersistable<T>> : public persisted_noncontainer_base<T> {
        using persisted_noncontainer_base<T>::persisted_noncontainer_base;
        using persisted_noncontainer_base<T>::operator*;
        using persisted_noncontainer_base<T>::operator=;
        template <typename C, typename U, typename V>
        friend persisted<C>& operator +=(persisted<C>& a, std::chrono::duration<U, V> b);
        constexpr auto time_since_epoch() const {
            if (auto& object = this->m_object) {
                auto obj = object->obj();
                auto ts = static_cast<T>(obj.template get<Timestamp>(this->managed));
                return ts.time_since_epoch();
            } else {
                return this->unmanaged.time_since_epoch();
            }
        }
    };
    template <typename T, typename U, typename V>
    persisted<T>& operator +=(persisted<T>& a, std::chrono::duration<U, V> b) {
        if (auto& object = a.m_object) {
            auto obj = object->obj();
            auto ts = obj.template get<Timestamp>(a.managed);
            obj.template set(a.managed, Timestamp(ts.get_time_point() + b));
        } else {
            a.unmanaged += b;
        }
        return a;
    }
}

#endif //REALM_PERSISTED_TIMESTAMP_HPP
