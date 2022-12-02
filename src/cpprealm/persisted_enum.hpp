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

#ifndef REALM_PERSISTED_ENUM_HPP
#define REALM_PERSISTED_ENUM_HPP

#include <cpprealm/persisted.hpp>

namespace realm {
    template <typename T>
    struct persisted<T, std::enable_if_t<std::is_enum_v<T>>> : public persisted_base<T> {
        using persisted_base<T>::persisted_base;

        persisted& operator=(const T& o) {
            if (this->is_managed()) {
                this->m_object->obj().template set<int64_t>(this->managed,
                        static_cast<int64_t>(o));
            } else {
                new (&this->unmanaged) T(o);
            }
            return *this;
        }
    };

    namespace {
        enum class test_enum {
            one, two
        };
        static_assert(sizeof(persisted<test_enum>));
        static_assert(sizeof(persisted<test_enum>{}.operator=(test_enum::one)));
    }
}

#endif //REALM_PERSISTED_ENUM_HPP
