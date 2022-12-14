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

namespace realm {
    template <typename T>
    struct persisted<T, std::enable_if_t<std::is_base_of_v<object_base, T>>> : public persisted_base<T> {
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
                m_property_object->m_object->obj().set(col_key, m_property_object->m_object->obj().get_key());
            } else {
                auto target_table = object->obj().get_table().get_link_target(col_key);
                T& target_cls = *m_property_object;

                auto realm = object->get_realm();
                if constexpr (decltype(T::schema)::HasPrimaryKeyProperty) {
                    using Schema = decltype(T::schema);
                    auto val = (target_cls).*Schema::PrimaryKeyProperty::ptr;
                    target_cls.m_object = internal::bridge::object(realm, target_table.create_object_with_primary_key(*val));
                } else {
                    target_cls.m_object = internal::bridge::object(realm, target_table.create_object());
                }
                T::schema.set(target_cls);
                object->obj().set(col_key, target_cls.m_object->obj().get_key());
            }
        }

        static internal::bridge::obj_key serialize(const T& v) {
            return v.m_object->obj().get_key();
        }
        std::optional<T> m_property_object;

        __cpp_realm_friends
    };
}

#endif //REALM_PERSISTED_EMBEDDED_HPP
