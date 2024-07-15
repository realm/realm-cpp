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

#ifndef CPPREALM_MANAGED_DICTIONARY_HPP
#define CPPREALM_MANAGED_DICTIONARY_HPP

#include <cpprealm/accessors.hpp>
#include <cpprealm/macros.hpp>
#include <cpprealm/notifications.hpp>
#include <cpprealm/observation.hpp>
#include <cpprealm/rbool.hpp>

namespace realm {

    template<typename mapped_type>
    struct box_base {
        box_base(internal::bridge::core_dictionary &&backing_map,
                 const std::string &key,
                 const internal::bridge::realm &r)
            : m_backing_map(std::move(backing_map)), m_key(key), m_realm(r) {}

        box_base(rbool* query,
                 internal::bridge::col_key column_key,
                 const std::string &key,
                 const internal::bridge::realm &r) {
            m_rbool_query = query;
            m_col_key = column_key;
            m_key = key;
            m_realm = r;
        }

        box_base &operator=(const mapped_type &o) {
            m_backing_map.insert(m_key, internal::bridge::mixed(std::move(o)));
            return *this;
        }
        box_base &operator=(mapped_type &&o) {
            if constexpr (internal::type_info::is_link<mapped_type>::value) {
                if (o) {
                    if constexpr (o->is_managed) {
                        m_backing_map.insert(m_key, o->m_managed.m_obj.get_key());
                    } else {
                        internal::bridge::obj m_obj;
                        if constexpr (managed<typename mapped_type::value_type::value_type>::schema.HasPrimaryKeyProperty) {
                            auto pk = (*o->unmanaged).*(managed<typename mapped_type::value_type::value_type>::schema.primary_key().ptr);
                            m_obj = m_backing_map.create_and_insert_linked_object(m_key, pk.value);
                        } else {
                            m_obj = m_backing_map.create_and_insert_linked_object(m_key);
                        }

                        std::apply([&m_obj, &o](auto && ...p) {
                            (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                                     m_obj, m_obj.get_table().get_column_key(p.name),
                                     (*o->unmanaged).*(std::decay_t<decltype(p)>::ptr)), ...);
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
                        internal::bridge::obj m_obj;
                        if constexpr (managed<typename mapped_type::value_type>::schema.HasPrimaryKeyProperty) {
                            auto pk = (*o->unmanaged).*(managed<typename mapped_type::value_type>::schema.primary_key().ptr);
                            m_obj = m_backing_map.create_and_insert_linked_object(m_key, pk.value);
                        } else {
                            m_obj = m_backing_map.create_and_insert_linked_object(m_key);
                        }

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

        rbool operator==(const mapped_type &rhs) const {
            if constexpr (realm::internal::type_info::MixedPersistableConcept<mapped_type>::value) {
                if (this->m_rbool_query) {
                    return this->m_rbool_query->dictionary_has_value_for_key_equals(this->m_col_key, m_key, serialize(rhs, m_realm));
                }
                return m_backing_map.get(m_key) == serialize(rhs, m_realm);
            } else {
                if (this->m_rbool_query) {
                    return this->m_rbool_query->dictionary_has_value_for_key_equals(this->m_col_key, m_key, internal::bridge::mixed(serialize(rhs)));
                }
                return m_backing_map.get(m_key) == internal::bridge::mixed(serialize(rhs));
            }
        }

        rbool operator!=(const mapped_type &rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_not_equals(this->m_col_key, m_key, internal::bridge::mixed(serialize(rhs, m_realm)));
            } else {
                return !operator==(rhs);
            }
        }

        internal::bridge::core_dictionary m_backing_map;
        internal::bridge::realm m_realm;
        std::string m_key;
        internal::bridge::col_key m_col_key;
        rbool* m_rbool_query = nullptr;
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

        rbool operator>(int64_t rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_greater_than(this->m_col_key, m_key, rhs);
            }
            return m_backing_map.get(m_key) > internal::bridge::mixed(rhs);
        }

        rbool operator>=(int64_t rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_greater_than_equals(this->m_col_key, m_key, rhs);
            }
            return m_backing_map.get(m_key) >= internal::bridge::mixed(rhs);
        }

        rbool operator<(int64_t rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_less_than(this->m_col_key, m_key, rhs);
            }
            return m_backing_map.get(m_key) < internal::bridge::mixed(rhs);
        }

        rbool operator<=(int64_t rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_less_than_equals(this->m_col_key, m_key, rhs);
            }
            return m_backing_map.get(m_key) <= internal::bridge::mixed(rhs);
        }
    };
    template<>
    struct box<double> : public box_base<double> {
        using box_base<double>::box_base;
        using box_base<double>::operator=;
        double operator*() {
            return m_backing_map.get(m_key).operator double();
        }

        rbool operator>(double rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_greater_than(this->m_col_key, m_key, rhs);
            }
            return m_backing_map.get(m_key) > internal::bridge::mixed(rhs);
        }

        rbool operator>=(double rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_greater_than_equals(this->m_col_key, m_key, rhs);
            }
            return m_backing_map.get(m_key) >= internal::bridge::mixed(rhs);
        }

        rbool operator<(double rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_less_than(this->m_col_key, m_key, rhs);
            }
            return m_backing_map.get(m_key) < internal::bridge::mixed(rhs);
        }

        rbool operator<=(double rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_less_than_equals(this->m_col_key, m_key, rhs);
            }
            return m_backing_map.get(m_key) <= internal::bridge::mixed(rhs);
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
    template<typename V>
    struct box<V, std::enable_if_t<std::is_enum_v<V>>> : public box_base<V> {
        using box_base<V>::box_base;
        using box_base<V>::operator=;
        V operator*() {
            return this->m_backing_map.get(this->m_key).operator int64_t();
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
    template<typename Mixed>
    struct box<Mixed, std::enable_if_t<internal::type_info::MixedPersistableConcept<Mixed>::value>> : public box_base<Mixed> {
        using box_base<Mixed>::box_base;
        using box_base<Mixed>::operator=;

        rbool operator>(Mixed rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_greater_than(this->m_col_key, this->m_key, serialize(rhs, this->m_realm));
            }
            return this->m_backing_map.get(this->m_key) > serialize(rhs, this->m_realm);
        }

        rbool operator>=(Mixed rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_greater_than_equals(this->m_col_key, this->m_key, serialize(rhs, this->m_realm));
            }
            return this->m_backing_map.get(this->m_key) >= serialize(rhs, this->m_realm);
        }

        rbool operator<(Mixed rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_less_than(this->m_col_key, this->m_key, serialize(rhs, this->m_realm));
            }
            return this->m_backing_map.get(this->m_key) < serialize(rhs, this->m_realm);
        }

        rbool operator<=(Mixed rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_less_than_equals(this->m_col_key, this->m_key, serialize(rhs, this->m_realm));
            }
            return this->m_backing_map.get(this->m_key) <= serialize(rhs, this->m_realm);
        }
    };
    template<>
    struct box<object_id> : public box_base<object_id> {
        using box_base<object_id>::box_base;
        using box_base<object_id>::operator=;
        object_id operator*() {
            return this->m_backing_map.get(this->m_key).operator internal::bridge::object_id().operator ::realm::object_id();
        }
    };
    template<>
    struct box<decimal128> : public box_base<decimal128> {
        using box_base<decimal128>::box_base;
        using box_base<decimal128>::operator=;
        decimal128 operator*() {
            return this->m_backing_map.get(this->m_key).operator internal::bridge::decimal128().operator ::realm::decimal128();
        }

        rbool operator>(decimal128 rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_greater_than(this->m_col_key, m_key, serialize(rhs));
            }
            return m_backing_map.get(m_key) > internal::bridge::mixed(serialize(rhs));
        }

        rbool operator>=(decimal128 rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_greater_than_equals(this->m_col_key, m_key, serialize(rhs));
            }
            return m_backing_map.get(m_key) >= internal::bridge::mixed(serialize(rhs));
        }

        rbool operator<(decimal128 rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_less_than(this->m_col_key, m_key, serialize(rhs));
            }
            return m_backing_map.get(m_key) < internal::bridge::mixed(serialize(rhs));
        }

        rbool operator<=(decimal128 rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_less_than_equals(this->m_col_key, m_key, serialize(rhs));
            }
            return m_backing_map.get(m_key) <= internal::bridge::mixed(serialize(rhs));
        }
    };
    template<>
    struct box<std::chrono::time_point<std::chrono::system_clock>> : public box_base<std::chrono::time_point<std::chrono::system_clock>> {
        using box_base<std::chrono::time_point<std::chrono::system_clock>>::box_base;
        using box_base<std::chrono::time_point<std::chrono::system_clock>>::operator=;
        std::chrono::time_point<std::chrono::system_clock> operator*() {
            return this->m_backing_map.get(this->m_key).operator internal::bridge::timestamp().operator std::chrono::time_point<std::chrono::system_clock>();
        }

        rbool operator>(std::chrono::time_point<std::chrono::system_clock> rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_greater_than(this->m_col_key, m_key, serialize(rhs));
            }
            return m_backing_map.get(m_key) > internal::bridge::mixed(rhs);
        }

        rbool operator>=(std::chrono::time_point<std::chrono::system_clock> rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_greater_than_equals(this->m_col_key, m_key, serialize(rhs));
            }
            return m_backing_map.get(m_key) >= internal::bridge::mixed(rhs);
        }

        rbool operator<(std::chrono::time_point<std::chrono::system_clock> rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_less_than(this->m_col_key, m_key, serialize(rhs));
            }
            return m_backing_map.get(m_key) < internal::bridge::mixed(rhs);
        }

        rbool operator<=(std::chrono::time_point<std::chrono::system_clock> rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_less_than_equals(this->m_col_key, m_key, serialize(rhs));
            }
            return m_backing_map.get(m_key) <= internal::bridge::mixed(rhs);
        }
    };
    template<>
    struct box<std::vector<uint8_t>> : public box_base<std::vector<uint8_t>> {
        using box_base<std::vector<uint8_t>>::box_base;
        using box_base<std::vector<uint8_t>>::operator=;
        std::vector<uint8_t> operator*() {
            return this->m_backing_map.get(this->m_key).operator internal::bridge::binary().operator std::vector<uint8_t>();
        }
    };
    template<>
    struct box<std::string> : public box_base<std::string> {
        using box_base<std::string>::box_base;
        using box_base<std::string>::operator=;
        std::string operator*() {
            return this->m_backing_map.get(this->m_key).operator std::string();
        }

        rbool contains(const std::string& rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_contains_string_for_key(this->m_col_key, m_key, rhs);
            }
            std::string lhs = m_backing_map.get(m_key);
            return lhs.find(rhs) != std::string::npos;
        }
    };

    //MARK: - Boxed Link
    template<typename V>
    struct box<managed<V*>> : public box_base<managed<V*>> {
        using box_base<managed<V*>>::box_base;
        using box_base<managed<V*>>::operator=;
        using box_base<managed<V*>>::operator==;

        rbool operator==(const managed<V*> &rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_equals(this->m_col_key,
                                                                                this->m_key,
                                                                                rhs ? internal::bridge::mixed(internal::bridge::obj_link(rhs.m_obj->get_table().get_key(), rhs.m_obj->get_key())) : internal::bridge::mixed());
            }
            auto a = const_cast<box<managed<V*>> *>(this)->m_backing_map.get_object(this->m_key);
            auto &b = rhs.m_obj;
            if (this->m_realm != *rhs.m_realm) {
                return false;
            }
            return a.get_key() == b->get_key();
        }

        rbool operator!=(const managed<V*> rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_not_equals(this->m_col_key,
                                                                                    this->m_key,
                                                                                    rhs ? internal::bridge::mixed(internal::bridge::obj_link(rhs.m_obj->get_table().get_key(), rhs.m_obj->get_key())) : internal::bridge::mixed());
            }
            return !this->operator==(rhs);
        }

        rbool operator==(const managed<V> &rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_equals(this->m_col_key,
                                                                                this->m_key,
                                                                                internal::bridge::obj_link(rhs.m_obj.get_table().get_key(), rhs.m_obj.get_key()));
            }

            auto a = const_cast<box<managed<V*>> *>(this)->m_backing_map.get_object(this->m_key);
            auto &b = rhs.m_obj;
            if (this->m_realm != rhs.m_realm) {
                return false;
            }
            return a.get_key() == b.get_key();
        }

        rbool operator!=(const managed<V> rhs) const {
            if (this->m_rbool_query) {
                return this->m_rbool_query->dictionary_has_value_for_key_not_equals(this->m_col_key,
                                                                                    this->m_key,
                                                                                    internal::bridge::obj_link(rhs.m_obj.get_table().get_key(), rhs.m_obj.get_key()));
            }

            return !this->operator==(rhs);
        }

        std::optional<typename managed<V*>::ref_type> operator*() {
            auto obj = this->m_backing_map.get_object(this->m_key);
            if (!obj.is_valid()) {
                return std::nullopt;
            }
            return typename managed<V*>::ref_type(managed<V>(std::move(obj), this->m_realm));
        }

        typename managed<V*>::ref_type operator->() {
            if (this->m_rbool_query) {
                rbool::dictionary_context ctx;
                ctx.m_key = this->m_key;
                ctx.origin_col_key = this->m_col_key;
                this->m_rbool_query->add_dictionary_link_chain(std::move(ctx));
                return typename managed<V*>::ref_type(managed<V>::prepare_for_query(this->m_realm, this->m_rbool_query));
            }
            auto obj = this->m_backing_map.get_object(this->m_key);
            return typename managed<V*>::ref_type(managed<V>(std::move(obj), this->m_realm));
        }

        box& operator=(V* o) {
            if (!o) {
                // Insert null link for key.
                this->m_backing_map.insert(this->m_key, realm::internal::bridge::mixed());
                return *this;
            }
            internal::bridge::obj m_obj;
            if constexpr (managed<V>::schema.HasPrimaryKeyProperty) {
                auto pk = (*o).*(managed<V>::schema.primary_key().ptr);
                m_obj = const_cast<box<managed<V*>> *>(this)->m_backing_map.create_and_insert_linked_object(const_cast<box<managed<V*>> *>(this)->m_key, pk.value);
            } else {
                m_obj = const_cast<box<managed<V*>> *>(this)->m_backing_map.create_and_insert_linked_object(const_cast<box<managed<V*>> *>(this)->m_key);
            }

            std::apply([&m_obj, &o, realm = this->m_realm](auto && ...p) {
                (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                         m_obj, m_obj.get_table().get_column_key(p.name), realm,
                         (*o).*(std::decay_t<decltype(p)>::ptr)), ...);
            }, managed<V>::schema.ps);
            return *this;
        }

        box& operator=(const managed<V*>& o) {
            this->m_backing_map.insert(this->m_key, o->m_obj.get_key());
            return *this;
        }

        box& operator=(const managed<V>& o) {
            this->m_backing_map.insert(this->m_key, o.m_obj.get_key());
            return *this;
        }

        bool operator==(const box<managed<V*>> &rhs) const {
            auto a = const_cast<box<managed<V*>>*>(this)->m_backing_map.get_object(this->m_key);
            auto &b = static_cast<box<managed<V*>>>(rhs)->m_obj;
            if (this->m_realm != rhs.m_realm) {
                return false;
            }
            return a.get_key() == b.get_key();
        }

        bool operator!=(const box<managed<V*>> rhs) const {
            return !this->operator==(rhs);
        }

        bool operator==(const box<V*>& rhs) {
            auto a = const_cast<box<managed<V*>> *>(this)->m_backing_map.get_object(this->m_key);
            auto &b = (&rhs)->m_obj;
            if (this->m_realm != rhs.m_realm) {
                return false;
            }
            return a.get_key() == b.get_key();
        }

        bool operator!=(const box<V*>& rhs) const {
            return !this->operator==(rhs);
        }
    };

    template<typename T>
    struct managed<std::map<std::string, T>, void> : managed_base {
        using managed<std::map<std::string, T>>::managed_base::operator=;

        [[nodiscard]] std::map<std::string, T> detach() const {
            if constexpr (std::is_pointer_v<T>) {
                auto d = internal::bridge::get<internal::bridge::core_dictionary>(*m_obj, m_key);
                size_t s = d.size();
                std::map<std::string, T> ret;
                for (size_t i = 0; i < s; i++) {
                    auto pair = d.get_pair(i);
                    using Type = std::remove_pointer_t<T>;
                    T v = new Type;
                    managed<Type, void> m(d.get_object(pair.first), *m_realm);

                    auto assign = [&m, &v](auto& pair) {
                        (*v).*(std::decay_t<decltype(pair.first)>::ptr) = (m.*(pair.second)).detach();
                    };
                    auto zipped = internal::zip_tuples(managed<Type>::schema.ps, managed<Type>::managed_pointers());

                    std::apply([&v, &m, &assign](auto && ...pair) {
                        (assign(pair), ...);
                    }, zipped);

                    ret[pair.first] = v;
                }

                return ret;
            }
            auto ret = std::map<std::string, T>();
            for (auto [k, v] : *this) {
                ret[k] = v;
            }
            return ret;
        }

        std::enable_if<std::is_pointer_v<T>, std::map<std::string, managed<T>>> to_map() const {
            auto ret = std::map<std::string, T>();
            for (auto [k, v] : *this) {
                ret[k] = v;
            }
            return ret;
        }

        class iterator {
        public:
            using iterator_category = std::input_iterator_tag;

            bool operator!=(const iterator& other) const
            {
                return !(*this == other);
            }

            bool operator==(const iterator& other) const
            {
                return (m_parent == other.m_parent) && (m_i == other.m_i);
            }

            std::pair<std::string, T> operator*() noexcept
            {
                auto pair = m_parent->m_obj->get_dictionary(m_parent->m_key).get_pair(m_i);
                return { pair.first, deserialize<T>(pair.second) };
            }

            iterator& operator++()
            {
                this->m_i++;
                return *this;
            }

            const iterator& operator++(int i)
            {
                this->m_i += i;
                return *this;
            }
        private:
            template<typename, typename>
            friend struct managed;

            iterator(size_t i, const managed<std::map<std::string, T>>* parent)
                : m_i(i), m_parent(parent)
            {
            }
            size_t m_i;
            const managed<std::map<std::string, T>>* m_parent;
        };

        size_t size() const
        {
            return m_obj->get_dictionary(m_key).size();
        }

        iterator begin() const
        {
            return iterator(0, this);
        }

        iterator end() const
        {
            return iterator(size(), this);
        }

        iterator find(const std::string& key) {
            if (m_rbool_query) {
                throw std::runtime_error("`find` is not available in Type Safe Queries, use `contains_key` instead.");
            }
            // Dictionary's `find` searches for the index of the value and not the key.
            auto d = m_obj->get_dictionary(m_key);
            auto i = d.find_any_key(key);
            if (i == size_t(-1)) {
                return iterator(size(), this);
            } else {
                return iterator(i, this);
            }
        }

        box<std::conditional_t<std::is_pointer_v<T>, managed<T>, T>>  operator[](const std::string &key) {
            if constexpr (std::is_pointer_v<T>) {
                if (m_rbool_query) {
                    return box<managed<T>>(m_rbool_query, m_key, key, *m_realm);
                }
                return box<managed<T>>(m_obj->get_dictionary(m_key), key, *m_realm);
            } else {
                if (m_rbool_query) {
                    return box<T>(m_rbool_query, m_key, key, *m_realm);
                }
                return box<T>(m_obj->get_dictionary(m_key), key, *m_realm);
            }
        }

        void erase(const std::string& key) {
            m_obj->get_dictionary(m_key).erase(key);
        }

        /// Convenience method to be primarily used in type safe queries.
        rbool contains_key(const std::string& key) {
            if (m_rbool_query) {
                return m_rbool_query->dictionary_has_key(m_key, key);
            } else {
                return m_obj->get_dictionary(m_key).find_any_key(key) != size_t(-1);
            }
        }

        notification_token observe(std::function<void(realm::dictionary_collection_change)>&& fn)
        {
            auto o = internal::bridge::object(*m_realm, *m_obj);
            auto dict = std::make_shared<realm::internal::bridge::dictionary>(o.get_dictionary(m_key));
            realm::notification_token token = dict->add_key_based_notification_callback(
                                                std::make_shared<realm::dictionary_callback_wrapper>(std::move(fn), false));
            token.m_realm = *m_realm;
            token.m_dictionary = dict;
            return token;
        }

    private:
        managed() = default;
        managed(const managed&) = delete;
        managed(managed &&) = delete;
        managed& operator=(const managed&) = delete;
        managed& operator=(managed&&) = delete;
        template<typename, typename>
        friend struct managed;
    };

} // namespace realm

#endif//CPPREALM_MANAGED_DICTIONARY_HPP
