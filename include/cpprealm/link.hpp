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

#ifndef CPPREALM_BRIDGE_LINK_HPP
#define CPPREALM_BRIDGE_LINK_HPP

#include <cpprealm/accessors.hpp>
#include <cpprealm/macros.hpp>
#include <cpprealm/rbool.hpp>

#include <cpprealm/internal/type_info.hpp>

namespace realm {
    template <typename, typename>
    struct managed;

    template<typename T>
    struct managed<T*> : managed_base {
        T* detach() const {
            T* v = new T();
            managed<T> m(m_obj->is_null(m_key) ? *m_obj : m_obj->get_linked_object(m_key), *m_realm);
            auto assign = [&m, &v](auto& pair) {
                (*v).*(std::decay_t<decltype(pair.first)>::ptr) = (m.*(pair.second)).detach();
            };
            auto zipped = internal::zip_tuples(managed<T>::schema.ps, managed<T>::managed_pointers());
            std::apply([&v, &m, &assign](auto && ...pair) {
                (assign(pair), ...);
            }, zipped);
            return v;
        }

        struct ref_type {
            explicit ref_type(managed<T>&& value) : m_managed(std::move(value)) { }
            const managed<T>* operator ->() const {
                return &m_managed;
            }
            managed<T>* operator ->() {
                return &m_managed;
            }

            const managed<T>& operator *() const {
                return m_managed;
            }
            managed<T>& operator *() {
                return m_managed;
            }
            
            bool operator ==(const managed<T*>& rhs) const {
                if (this->m_managed.m_realm != *rhs.m_realm) {
                    return false;
                }
                return this->m_managed.m_obj.get_key() == rhs.m_obj->get_key();
            }
            bool operator ==(const managed<T>& rhs) const {
                if (this->m_managed.m_realm != rhs.m_realm) {
                    return false;
                }
                return this->m_managed.m_obj.get_table() == rhs.m_obj.get_table() &&
                    this->m_managed.m_obj.get_key() == rhs.m_obj.get_key();
            }
            bool operator ==(const ref_type& rhs) const {
                if (this->m_managed.m_realm != rhs.m_managed.m_realm) {
                    return false;
                }
                return this->m_managed.m_obj.get_table() == rhs.m_managed.m_obj.get_table() &&
                    this->m_managed.m_obj.get_key() == rhs.m_managed.m_obj.get_key();
            }
            bool operator !=(const managed<T*>& rhs) const {
                return !this->operator==(rhs);
            }
            bool operator !=(const managed<T>& rhs) const {
                return !this->operator==(rhs);
            }
            bool operator !=(const ref_type& rhs) const {
                return !this->operator==(rhs);
            }
        private:
            managed<T> m_managed;
        };
        ref_type operator ->() const {
            if (this->m_rbool_query) {
                this->m_rbool_query->add_link_chain(m_key);
                return ref_type(managed<T>::prepare_for_query(*m_realm, this->m_rbool_query));
            }
            return ref_type(managed<T>(m_obj->get_linked_object(m_key), *m_realm));
        }
        operator bool() const {
            if (m_obj && m_key) {
                return !m_obj->is_null(m_key);
            }
            return false;
        }
        managed &operator=(const managed<T>& obj) {
            m_obj->set(m_key, obj.m_obj.get_key());
            return *this;
        }
        managed &operator=(const managed<T*> &obj) {
            m_obj->set(m_key, obj.m_obj->get_key());
            return *this;
        }
        managed &operator=(std::nullptr_t) {
            m_obj->set_null(m_key);
            return *this;
        }
        managed &operator=(T* o) {
            auto table = m_realm->table_for_object_type(managed<T>::schema.name);
            internal::bridge::obj obj;
            if (!o) {
                m_obj->set_null(m_key);
                return *this;
            } else if constexpr (managed<T>::schema.HasPrimaryKeyProperty) {
                auto pk = (*o).*(managed<T>::schema.primary_key().ptr);
                obj = table.create_object_with_primary_key(realm::internal::bridge::mixed(serialize(pk.value)));
                m_obj->set(m_key, obj.get_key());
            } else if (managed<T>::schema.is_embedded()) {
                obj = m_obj->create_and_set_linked_object(m_key);
            } else {
                obj = table.create_object();
                m_obj->set(m_key, obj.get_key());
            }

            std::apply([&obj, &o, realm = *m_realm](auto && ...p) {
                (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                         obj, obj.get_table().get_column_key(p.name), realm, (*o).*(std::decay_t<decltype(p)>::ptr)
                                 ), ...);
            }, managed<T>::schema.ps);
            return *this;
        }

        rbool operator ==(std::nullptr_t) const {
            if (m_rbool_query) {
                return m_rbool_query->link_equal(m_key, std::nullopt);
            }
            return !m_obj->get_linked_object(m_key).is_valid();
        }
        rbool operator ==(const managed<T>& rhs) const {
            if (m_rbool_query) {
                return m_rbool_query->link_equal(m_key, rhs.m_obj);
            }
            if (*this->m_realm != rhs.m_realm)
                return false;
            return m_obj->get_linked_object(m_key).get_key() == rhs.m_obj.get_key();
        }

        rbool operator ==(const managed<T*>& rhs) const {
            if (m_rbool_query) {
                return m_rbool_query->link_equal(m_key, *rhs.m_obj);
            }
            if (*this->m_realm != *rhs.m_realm)
                return false;
            return m_obj->get_linked_object(m_key).get_key() == rhs.m_obj->get_key();
        }

        rbool operator !=(std::nullptr_t) const {
            if (m_rbool_query) {
                return m_rbool_query->link_not_equal(m_key, std::nullopt);
            }
            return m_obj->get_linked_object(m_key).is_valid();
        }
        rbool operator !=(const managed<T>& rhs) const {
            if (m_rbool_query) {
                return m_rbool_query->link_not_equal(m_key, rhs.m_obj);
            }
            return m_obj->get_linked_object(m_key).is_valid();
        }

        rbool operator !=(const managed<T*>& rhs) const {
            if (m_rbool_query) {
                return m_rbool_query->link_not_equal(m_key, *rhs.m_obj);
            }
            return m_obj->get_linked_object(m_key).is_valid();
        }

    private:
        managed<T*>() = default;
        template<typename, typename>
        friend struct managed;
    };
} //namespace realm
#endif //CPPREALM_BRIDGE_LINK_HPP
