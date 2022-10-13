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

#ifndef REALM_PERSISTED_EMBEDDED_HPP
#define REALM_PERSISTED_EMBEDDED_HPP

#include <cpprealm/persisted.hpp>

namespace realm {
    template<typename T>
    struct persisted<T, type_info::EmbeddedObjectPersistable<T>> : public persisted_noncontainer_base<T> {
        using persisted_noncontainer_base<T>::persisted_noncontainer_base;
        using persisted_noncontainer_base<T>::operator=;
        using persisted_noncontainer_base<T>::operator*;

        persisted& operator=(const T& o) override {
            if (auto obj = this->m_object) {
                // if non-null object is being assigned...
                if (o.m_object) {
                    // if object is managed, we will to set the link
                    // to the new target's key
                    obj->obj().template set<typename persisted_noncontainer_base<T>::type>(
                            this->managed,
                            o.m_object->obj().get_key());
                } else {
                    // else new unmanaged object is being assigned.
                    // we must assign the values to this object's fields
                    // TODO:
                    REALM_UNREACHABLE();
                }
            } else {
                new (&this->unmanaged) T(o);
            }
            return *this;
        }
    };
}

#endif //REALM_PERSISTED_EMBEDDED_HPP
