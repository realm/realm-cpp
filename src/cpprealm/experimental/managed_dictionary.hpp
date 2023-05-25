#ifndef REALM_MANAGED_DICTIONARY_HPP
#define REALM_MANAGED_DICTIONARY_HPP

#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/experimental/accessors.hpp>

namespace realm::experimental {

    template<typename mapped_type>
    struct box_base {
        box_base(internal::bridge::core_dictionary &&backing_map,
                 const std::string &key,
                 const internal::bridge::realm &r)
            : m_backing_map(std::move(backing_map)), m_key(key), m_realm(r) {}

        box_base &operator=(const mapped_type &o) {
            m_backing_map.insert(m_key, std::move(o));
            return *this;
        }
        box_base &operator=(mapped_type &&o) {
            if constexpr (internal::type_info::is_optional<mapped_type>::value) {
                if (o) {
                    // TODO: remove repeated code
                    if (o->is_managed) {
                        m_backing_map.insert(m_key, o->m_managed.m_obj.get_key());
                    } else {
                        auto m_obj = m_backing_map.create_and_insert_linked_object(m_key);
                        std::apply([&m_obj, &o](auto && ...p) {
                            (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                                     m_obj, m_obj.get_table().get_column_key(p.name),
                                     o->unmanaged.*(std::decay_t<decltype(p)>::ptr)), ...);
                        }, managed<typename mapped_type::value_type::value_type>::schema.ps);
                    }
                } else {
                    m_backing_map.insert(m_key, internal::bridge::mixed());
                }
            } else {
                if constexpr (internal::type_info::is_primitive<mapped_type>::value) {
                    m_backing_map.insert(m_key, serialize(std::move(o)));
                } else {
                   if (o.is_managed) {
                        m_backing_map.insert(m_key, o.managed.m_obj.get_key());
                    } else {
                        auto m_obj = m_backing_map.create_and_insert_linked_object(m_key);
                        std::apply([&m_obj, &o](auto && ...p) {
                            (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                                     m_obj, m_obj.get_table().get_column_key(p.name),
                                     o.unmanaged.*(std::decay_t<decltype(p)>::ptr)), ...);
                        }, managed<typename mapped_type::value_type>::schema.ps);
                    }
                }
            }
            return *this;
        }

        void erase(const std::string &key) {
            abort();
//            m_backing_map.get().remove(m_key);
        }

        bool operator==(const mapped_type &rhs) const {
            if constexpr (internal::type_info::is_optional<mapped_type>::value) {
                auto v = m_backing_map.get(m_key);
                if (v.is_null() && !rhs) {
                    return true;
                }
                return m_backing_map.get(m_key) == rhs;
            } else if constexpr (realm::internal::type_info::MixedPersistableConcept<mapped_type>::value) {
                return m_backing_map.get(m_key) == serialize(rhs, m_realm);
            } else {
                return m_backing_map.get(m_key) == internal::bridge::mixed(rhs);
            }
        }
        internal::bridge::core_dictionary m_backing_map;
        internal::bridge::realm m_realm;
        std::string m_key;
    };
    template<typename V, typename = void>
    struct box;
    template<>
    struct box<int64_t> : public box_base<int64_t> {
        using box_base<int64_t>::box_base;
        using box_base<int64_t>::operator=;
        int64_t operator*() {
            return m_backing_map.get(m_key).operator int64_t();
        }
    };
    template<>
    struct box<std::optional<int64_t>> : public box_base<std::optional<int64_t>> {
        using box_base<std::optional<int64_t>>::box_base;
        using box_base<std::optional<int64_t>>::operator=;
        std::optional<int64_t> operator*() {
            auto v = m_backing_map.get(m_key);
            if (v.is_null()) {
                return std::nullopt;
            } else {
                return v.operator int64_t();
            };
        }
    };
    template<>
    struct box<double> : public box_base<double> {
        using box_base<double>::box_base;
        using box_base<double>::operator=;
        double operator*() {
            return m_backing_map.get(m_key).operator double();
        }
    };
    template<>
    struct box<std::optional<double>> : public box_base<std::optional<double>> {
        using box_base<std::optional<double>>::box_base;
        using box_base<std::optional<double>>::operator=;
        std::optional<double> operator*() {
            auto v = m_backing_map.get(m_key);
            if (v.is_null()) {
                return std::nullopt;
            } else {
                return v.operator double();
            };
        }
    };
    template<>
    struct box<bool> : public box_base<bool> {
        using box_base<bool>::box_base;
        using box_base<bool>::operator=;
        bool operator*() {
            return m_backing_map.get(m_key).operator bool();
        }
    };
    template<>
    struct box<std::optional<bool>> : public box_base<std::optional<bool>> {
        using box_base<std::optional<bool>>::box_base;
        using box_base<std::optional<bool>>::operator=;
        std::optional<bool> operator*() {
            auto v = m_backing_map.get(m_key);
            if (v.is_null()) {
                return std::nullopt;
            } else {
                return v.operator bool();
            };
        }
    };
    template<typename V>
    struct box<V, std::enable_if_t<std::is_enum_v<V>>> : public box_base<V> {
        using box_base<V>::box_base;
        using box_base<V>::operator=;
        V operator*() {
            return this->m_backing_map.get(this->m_key).operator int64_t();
        }

        bool operator==(const V& rhs) const {
            return this->m_backing_map.get(this->m_key).operator int64_t() == static_cast<int64_t>(rhs);
        }
    };
    template<typename V>
    struct box<V, std::enable_if_t<std::is_enum_v<typename V::value_type>>> : public box_base<V> {
        using box_base<V>::box_base;
        using box_base<V>::operator=;

        V operator*() {
            auto v = this->m_backing_map.get(this->m_key);
            if (v.is_null()) {
                return std::nullopt;
            } else {
                return static_cast<typename V::value_type>(v.operator int64_t());
            };
        }

        bool operator==(const V &rhs) const {
            auto v = this->m_backing_map.get(this->m_key);
            if (v.is_null() && !rhs) {
                return true;
            }
            return static_cast<typename V::value_type>(v.operator int64_t()) == rhs;
        }
    };
    template<>
    struct box<uuid> : public box_base<uuid> {
        using box_base<uuid>::box_base;
        using box_base<uuid>::operator=;
        uuid operator*() {
            return this->m_backing_map.get(this->m_key).operator internal::bridge::uuid().operator ::realm::uuid();
        }
    };
    template<>
    struct box<std::optional<uuid>> : public box_base<std::optional<uuid>> {
        using box_base<std::optional<uuid>>::box_base;
        using box_base<std::optional<uuid>>::operator=;
        std::optional<uuid> operator*() {
            auto v = this->m_backing_map.get(this->m_key);
            if (v.is_null()) {
                return std::nullopt;
            } else {
                return v.operator internal::bridge::uuid().operator ::realm::uuid();
            };
        }
    };
    template<typename Mixed>
    struct box<Mixed, std::enable_if_t<internal::type_info::MixedPersistableConcept<Mixed>::value>> : public box_base<Mixed> {
        using box_base<Mixed>::box_base;
        using box_base<Mixed>::operator=;
    };
    template<>
    struct box<object_id> : public box_base<object_id> {
        using box_base<object_id>::box_base;
        using box_base<object_id>::operator=;
    };
    template<>
    struct box<std::optional<object_id>> : public box_base<std::optional<object_id>> {
        using box_base<std::optional<object_id>>::box_base;
        using box_base<std::optional<object_id>>::operator=;
    };
    template<>
    struct box<std::chrono::time_point<std::chrono::system_clock>> : public box_base<std::chrono::time_point<std::chrono::system_clock>> {
        using box_base<std::chrono::time_point<std::chrono::system_clock>>::box_base;
        using box_base<std::chrono::time_point<std::chrono::system_clock>>::operator=;
    };
    template<>
    struct box<std::optional<std::chrono::time_point<std::chrono::system_clock>>> : public box_base<std::optional<std::chrono::time_point<std::chrono::system_clock>>> {
        using box_base<std::optional<std::chrono::time_point<std::chrono::system_clock>>>::box_base;
        using box_base<std::optional<std::chrono::time_point<std::chrono::system_clock>>>::operator=;
    };
    template<>
    struct box<std::vector<uint8_t>> : public box_base<std::vector<uint8_t>> {
        using box_base<std::vector<uint8_t>>::box_base;
        using box_base<std::vector<uint8_t>>::operator=;
    };
    template<>
    struct box<std::optional<std::vector<uint8_t>>> : public box_base<std::optional<std::vector<uint8_t>>> {
        using box_base<std::optional<std::vector<uint8_t>>>::box_base;
        using box_base<std::optional<std::vector<uint8_t>>>::operator=;
    };
    template<>
    struct box<std::string> : public box_base<std::string> {
        using box_base<std::string>::box_base;
        using box_base<std::string>::operator=;
    };
    template<>
    struct box<std::optional<std::string>> : public box_base<std::optional<std::string>> {
        using box_base<std::optional<std::string>>::box_base;
        using box_base<std::optional<std::string>>::operator=;
    };
    template<typename V>
    struct box<std::optional<link<V>>> : public box_base<std::optional<link<V>>> {
        using box_base<std::optional<link<V>>>::box_base;
        using box_base<std::optional<link<V>>>::operator=;
        using box_base<std::optional<link<V>>>::operator==;

        bool operator==(const std::optional<link<V>>& rhs) const {
            auto a = const_cast<box<link<V>> *>(this)->m_backing_map.get_object(this->m_key);
            auto &b = rhs.managed.m_obj;
            if (this->m_realm != rhs.managed.m_realm) {
                return false;
            }
            return a.get_table() == b.get_table() && a.get_key() == b.get_key();
        }

        bool operator==(const managed<V, void> &rhs) const {
            auto a = const_cast<box<link<V>> *>(this)->m_backing_map.get_object(this->m_key);
            auto &b = rhs.m_obj;
            if (this->m_realm != rhs.m_realm) {
                return false;
            }
            return a.get_table() == b.get_table() && a.get_key() == b.get_key();
        }


        bool operator==(const V &rhs) const {
            auto a = const_cast<box<link<V>> *>(this)->m_backing_map.get_object(this->m_key);
            auto &b = rhs.m_obj;
            if (this->m_realm != rhs.m_realm) {
                return false;
            }
            return a.get_table() == b.get_table() && a.get_key() == b.get_key();
        }

        std::optional<link<V>> operator*() {
            auto obj = this->m_backing_map.get_object(this->m_key);
            if (!obj.is_valid()) {
                return std::nullopt;
            }
            auto m = managed<V, void>(std::move(obj), this->m_realm);
            std::apply([&m](auto &&...ptr) {
                std::apply([&](auto &&...name) {
                    ((m.*ptr).assign(&m.m_obj, &m.m_realm, m.m_obj.get_table().get_column_key(name)), ...);
                },
                managed<V, void>::managed_pointers_names);
            },
            managed<V, void>::managed_pointers());
            return m;
        }
    };

    template<typename T>
    struct managed<std::map<std::string, T>, void> : managed_base {
        [[nodiscard]] std::map<std::string, T> value() const {
            // unused
            abort();
        }

        box<T> operator[](const std::string &a) {
            return box<T>(m_obj->get_dictionary(m_key), a, *m_realm);
        }
    };

} // namespace realm::experimental

#endif//REALM_MANAGED_DICTIONARY_HPP
