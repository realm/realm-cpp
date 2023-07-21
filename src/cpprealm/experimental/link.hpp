#ifndef CPPREALM_BRIDGE_LINK_HPP
#define CPPREALM_BRIDGE_LINK_HPP

#include <cpprealm/internal/type_info.hpp>
#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/experimental/accessors.hpp>

namespace realm {
    namespace experimental {
        template <typename, typename>
        struct managed;

        template<typename T>
        struct managed<T*> : managed_base {
            managed<T*> value() const { return *this; }
            struct ref_type {
                managed<T> m_managed;
                managed<T>* operator ->() const {
                    return const_cast<managed<T>*>(&m_managed);
                }
                managed<T>* operator ->() {
                    return &m_managed;
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
                    return this->m_managed.m_obj.get_key() == rhs.m_obj.get_key();

                }
                bool operator ==(const ref_type& rhs) const {
                    if (this->m_managed.m_realm != rhs.m_managed.m_realm) {
                        return false;
                    }
                    return this->m_managed.m_obj.get_key() == rhs.m_managed.m_obj.get_key();
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
            };
            ref_type operator ->() const {
                managed<T> m(m_obj->get_linked_object(m_key), *m_realm);
                return {std::move(m)};
            }
            operator bool() {
                if (m_obj && m_key) {
                    return m_obj->is_null(m_key);
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
                } else if (managed<T>::schema.is_embedded_experimental()) {
                    obj = m_obj->create_and_set_linked_object(m_key);
                } else {
                    obj = table.create_object();
                    m_obj->set(m_key, obj.get_key());
                }

                std::apply([&obj, &o](auto && ...p) {
                    (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                             obj, obj.get_table().get_column_key(p.name), (*o).*(std::decay_t<decltype(p)>::ptr)
                                     ), ...);
                }, managed<T>::schema.ps);
                auto m = managed<T>(std::move(obj), *m_realm);
                std::apply([&m](auto && ...ptr) {
                    std::apply([&](auto&& ...name) {
                        ((m.*ptr).assign(&m.m_obj, &m.m_realm, m.m_obj.get_table().get_column_key(name)), ...);
                    }, managed<T>::managed_pointers_names);
                }, managed<T>::managed_pointers());
                return *this;
            }

            bool operator ==(const std::nullptr_t) const {
                return !m_obj->get_linked_object(m_key).is_valid();
            }
            bool operator ==(const managed<T>& rhs) const {
                if (*this->m_realm != rhs.m_realm)
                    return false;                
                return m_obj->get_linked_object(m_key).get_key() == rhs.m_obj.get_key();
            }

            bool operator ==(const managed<T*>& rhs) const {
                if (*this->m_realm != *rhs.m_realm)
                    return false;
                return m_obj->get_linked_object(m_key).get_key() == rhs.m_obj->get_key();
            }

            bool operator !=(const std::nullptr_t) const {
                return !this->operator==(nullptr);
            }
            bool operator !=(const managed<T>& rhs) const {
                return !this->operator==(rhs);
            }

            bool operator !=(const managed<T*>& rhs) const {
                return !this->operator==(rhs);
            }
        };
    }
}
#endif //CPPREALM_BRIDGE_LINK_HPP
