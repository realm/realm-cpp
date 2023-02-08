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

#include <cpprealm/object.hpp>
#include <cpprealm/persisted.hpp>
#include <cpprealm/internal/bridge/realm.hpp>

namespace realm {
    // For forward links
    template <typename T>
    struct persisted<std::optional<T>, std::enable_if_t<std::is_base_of_v<object_base<T>, T>>> : public persisted_base<std::optional<T>> {
        persisted() = default;
        persisted(const persisted& v) {
            m_property_object = v.m_property_object;
        }
        persisted(const T& v) {
            m_property_object = v;
        }

        persisted& operator =(T&& v) {
            if (this->is_managed()) {
                auto realm = this->m_object->get_realm();
                if (std::is_base_of_v<embedded_object<T>, T>) {
                    m_property_object = v;
                    m_property_object->manage(
                            internal::bridge::object(realm, this->m_object->get_obj().create_and_set_linked_object(col_key)));
                } else {
                    if constexpr (internal::type_info::is_optional<T>::value) {
                        v.manage(realm.table_for_object_type(T::value_type::schema.name), realm);
                    } else {
                        v.manage(realm.table_for_object_type(T::schema.name), realm);
                    }
                    this->m_property_object = v;
                    this->m_object->get_obj().template set(col_key, this->m_property_object.value());
                }
            } else {
                m_property_object = v;
            }
            return *this;
        }

        persisted& operator =(T& v) {
            if (this->is_managed()) {
                auto realm = this->m_object->get_realm();
                if (std::is_base_of_v<embedded_object<T>, T>) {
                    m_property_object = v;
                    m_property_object->manage(
                            internal::bridge::object(realm, this->m_object->get_obj().create_and_set_linked_object(col_key)));
                    v = *m_property_object;
                } else {
                    if constexpr (internal::type_info::is_optional<T>::value) {
                        v.manage(realm.table_for_object_type(T::value_type::schema.name), realm);
                    } else {
                        v.manage(realm.table_for_object_type(T::schema.name), realm);
                    }
                    this->m_property_object = v;
                    this->m_object->get_obj().template set(col_key, this->m_property_object.value());
                }
            } else {
                m_property_object = v;
            }
            return *this;
        }
        persisted& operator =(std::nullopt_t) {
            m_property_object = std::nullopt;
            if (this->is_managed()) {
                this->m_object->get_obj().set_null(col_key);
            }
            return *this;
        }
        T* operator ->() {
            return &*m_property_object;
        }
        std::optional<T> operator * () const
        {
            if (this->is_managed() && this->m_object->is_valid() && !this->m_object->get_obj().is_null(col_key) && !m_property_object) {
                // Because we lazily fetch the property values there will be a point where
                // an object property will have a stored value but the persisted wrapper will have not
                // set up m_property_object. This could be due to writing to the property on object A,
                // but observing for a change on object B for the same property. Because of this we need to
                // set up the m_property_object mvar and pass it back on first access. This breaks const rules
                // when said scenario happens but shouldn't warrant this function losing its const signature.
                auto* c_this = const_cast<persisted<std::optional<T>>*>(this);
                c_this->m_property_object = T();
                c_this->m_property_object->assign_accessors(
                        internal::bridge::object(this->m_object->get_realm(),
                                                 this->m_object->get_obj().get_linked_object(col_key)));
                return m_property_object;

            }
            return m_property_object;
        }
    protected:
        void manage(internal::bridge::object* object,
                    internal::bridge::col_key&& col_key) final {
            if (!m_property_object) {
                return;
            }
            if (m_property_object->m_object) {
                m_property_object->m_object->get_obj().set(col_key, m_property_object->m_object->get_obj().get_key());
            } else {
                if (std::is_base_of_v<embedded_object<T>, T>) {
                    m_property_object->manage(
                            internal::bridge::object(object->get_realm(),
                                                     object->get_obj().create_and_set_linked_object(col_key)));
                } else {
                    m_property_object->manage(object->get_obj().get_table().get_link_target(col_key),
                                              object->get_realm());
                    object->get_obj().set(col_key, m_property_object->m_object->get_obj().get_key());
                }
            }
        }

        void assign_accessor(internal::bridge::object* object,
                             internal::bridge::col_key&& col_key) final {
            this->m_object = *object;
            this->col_key = std::move(col_key);
        }

        static internal::bridge::obj_key serialize(const T& v) {
            return v.m_object->get_obj().get_key();
        }
        std::optional<T> m_property_object;
        internal::bridge::col_key col_key;
        __cpp_realm_friends
    };

    // for container elements
    template <typename T>
    struct persisted<T, std::enable_if_t<std::is_base_of_v<object_base<T>, T>>> : public persisted_base<T> {
        persisted() = default;
        persisted(const persisted& v) {
            m_property_object = v.m_property_object;
        }
        persisted(const T& v) {
            m_property_object = v;
        }
        persisted& operator =(const T& v) {
            m_property_object = v;
            return *this;
        }
        persisted& operator =(std::nullopt_t) {
            m_property_object = std::nullopt;
            if (this->is_managed()) {
                abort();
            }
            return *this;
        }
        T* operator ->() {
            return &*m_property_object;
        }
        T& operator * () {
            return *m_property_object;
        }
    protected:
        void manage(internal::bridge::object* object,
                    internal::bridge::col_key&& col_key) final {
            if (!m_property_object) {
                return;
            }
            if (m_property_object->m_object) {
                m_property_object->m_object.obj().set(col_key, m_property_object->m_object.obj().get_key());
            } else {
                if (std::is_base_of_v<embedded_object<T>, T>) {
                    m_property_object->manage(
                            internal::bridge::object(object->get_realm(),
                                                     object->get_obj().create_and_set_linked_object(col_key)));
                } else {
                    m_property_object->manage(object->get_obj().get_table().get_link_target(col_key),
                                              object->get_realm());
                    object->get_obj().set(col_key, m_property_object->m_object->get_obj().get_key());
                }
            }
        }

        void assign_accessor(internal::bridge::object* object,
                             internal::bridge::col_key&& col_key) final {
            this->m_object = *object;
            this->col_key = std::move(col_key);
        }

        static internal::bridge::obj_key serialize(const T& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
            return v.m_object->get_obj().get_key();
        }
        std::optional<T> m_property_object;

        __cpp_realm_friends
    };
}

#endif //REALM_PERSISTED_EMBEDDED_HPP
