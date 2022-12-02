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

#ifndef REALM_PERSISTED_OPTIONAL_HPP
#define REALM_PERSISTED_OPTIONAL_HPP

#include "cpprealm/internal/bridge/obj.hpp"
#include <cpprealm/persisted.hpp>

namespace realm {
    template <typename T>
    struct persisted<std::optional<T>, std::enable_if_t<internal::type_info::is_primitive<T>::value>> :
            public persisted_base<std::optional<T>> {

        persisted& operator=(std::optional<T>&& o) {
            if (auto& obj = this->m_object) {
                if (o) {
                    obj->obj().set(this->managed,
                                   internal::type_info::serialize(*o));
                } else {
                    obj->obj().set_null(this->managed);
                }
            } else {
                if (o)
                    new (&this->unmanaged) T(std::move(*o));
                else
                    new (&this->unmanaged) std::optional<T>();
            }
            return *this;
        }

        persisted& operator=(const std::optional<T>& o) {
            if (auto& obj = this->m_object) {
                if (o) {
                    obj->obj().set(this->managed,
                                   internal::type_info::serialize(*o));
                } else {
                    obj->obj().set_null(this->managed);
                }
            } else {
                new (&this->unmanaged) T(o);
            }
            return *this;
        }
    };

    template <typename T>
    struct persisted<std::optional<T>, std::enable_if_t<std::is_base_of_v<object_base, T>>> :
            public persisted_base<std::optional<T>> {
        struct box {
            box(T&& v)
            : m_t(std::move(v))
            {}
            T* operator ->() {
                return &m_t;
            }
        private:
            T m_t;
        };
        operator bool() const {
            if (this->is_managed()) {
                return this->m_object->obj().is_null(this->managed);
            } else {
                return static_cast<bool>(this->unmanaged);
            }
        }
        box operator ->() const {
            return box(this->operator*());
        }
        persisted& operator=(const std::optional<T>& o) {
            if (auto& obj = this->m_object) {
                if (auto link = o) {
                    // if non-null object is being assigned...
                    if (link->m_object) {
                        // if object is managed, we will to set the link
                        // to the new target's key
                        obj->obj().template set<internal::bridge::obj_key>(this->managed,
                                link->m_object->obj().get_key());
                    } else {
                        // else new unmanaged object is being assigned.
                        // we must assign the values to this object's fields
                        // TODO:
                        REALM_UNREACHABLE();
                    }
                } else {
                    // else null link is being assigned to this field
                    // e.g., `person.dog = std::nullopt;`
                    // set the parent column to null and unset the co
                    obj->obj().set_null(this->managed);
                }
            } else {
                new (&this->unmanaged) std::optional<T>(o);
            }
            return *this;
        }
    };
    template <typename T>
    rbool inline operator==(const persisted<std::optional<T>>& a, const std::optional<T>& b)
    {
        if (a.should_detect_usage_for_queries) {
            auto query = internal::bridge::query(a.query->get_table());
            if (b) {
                query.equal(a.managed, internal::type_info::serialize(*b));
            } else {
                query.equal(a.managed, realm::null{});
            }
            return {std::move(query)};
        }

        if (a && b) {
            return *a == *b;
        } else if (!a && !b) {
            return true;
        } else {
            return false;
        }
    }
}

#endif //REALM_PERSISTED_OPTIONAL_HPP
