#ifndef CPPREALM_MANAGED_DICTIONARY_HPP
#define CPPREALM_MANAGED_DICTIONARY_HPP

#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/experimental/accessors.hpp>
#include <cpprealm/experimental/observation.hpp>
#include <cpprealm/notifications.hpp>

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
        bool operator!=(const mapped_type &rhs) const {
            return !this->operator==(rhs);
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
        bool operator!=(const V& rhs) const {
            return !this->operator==(rhs);
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

        bool operator==(const V& rhs) const {
            auto v = this->m_backing_map.get(this->m_key);
            if (v.is_null() && !rhs) {
                return true;
            }
            return static_cast<typename V::value_type>(v.operator int64_t()) == rhs;
        }
        bool operator!=(const V& rhs) const {
            return !this->operator==(rhs);
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
        object_id operator*() {
            return this->m_backing_map.get(this->m_key).operator internal::bridge::object_id().operator ::realm::object_id();
        }
    };
    template<>
    struct box<std::optional<object_id>> : public box_base<std::optional<object_id>> {
        using box_base<std::optional<object_id>>::box_base;
        using box_base<std::optional<object_id>>::operator=;
        std::optional<object_id> operator*() {
            auto v = this->m_backing_map.get(this->m_key);
            if (v.is_null()) {
                return std::nullopt;
            } else {
                return v.operator internal::bridge::object_id().operator ::realm::object_id();
            };
        }
    };
    template<>
    struct box<decimal128> : public box_base<decimal128> {
        using box_base<decimal128>::box_base;
        using box_base<decimal128>::operator=;
        decimal128 operator*() {
            return this->m_backing_map.get(this->m_key).operator internal::bridge::decimal128().operator ::realm::decimal128();
        }
    };
    template<>
    struct box<std::optional<decimal128>> : public box_base<std::optional<decimal128>> {
        using box_base<std::optional<decimal128>>::box_base;
        using box_base<std::optional<decimal128>>::operator=;
        std::optional<decimal128> operator*() {
            auto v = this->m_backing_map.get(this->m_key);
            if (v.is_null()) {
                return std::nullopt;
            } else {
                return v.operator internal::bridge::decimal128().operator ::realm::decimal128();
            };
        }
    };
    template<>
    struct box<std::chrono::time_point<std::chrono::system_clock>> : public box_base<std::chrono::time_point<std::chrono::system_clock>> {
        using box_base<std::chrono::time_point<std::chrono::system_clock>>::box_base;
        using box_base<std::chrono::time_point<std::chrono::system_clock>>::operator=;
        std::chrono::time_point<std::chrono::system_clock> operator*() {
            return this->m_backing_map.get(this->m_key).operator internal::bridge::timestamp().operator std::chrono::time_point<std::chrono::system_clock>();
        }
    };
    template<>
    struct box<std::optional<std::chrono::time_point<std::chrono::system_clock>>> : public box_base<std::optional<std::chrono::time_point<std::chrono::system_clock>>> {
        using box_base<std::optional<std::chrono::time_point<std::chrono::system_clock>>>::box_base;
        using box_base<std::optional<std::chrono::time_point<std::chrono::system_clock>>>::operator=;
        std::optional<std::chrono::time_point<std::chrono::system_clock>> operator*() {
            auto v = this->m_backing_map.get(this->m_key);
            if (v.is_null()) {
                return std::nullopt;
            } else {
                return this->m_backing_map.get(this->m_key).operator internal::bridge::timestamp().operator std::chrono::time_point<std::chrono::system_clock>();
            };
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
    struct box<std::optional<std::vector<uint8_t>>> : public box_base<std::optional<std::vector<uint8_t>>> {
        using box_base<std::optional<std::vector<uint8_t>>>::box_base;
        using box_base<std::optional<std::vector<uint8_t>>>::operator=;
        std::optional<std::vector<uint8_t>> operator*() {
            auto v = this->m_backing_map.get(this->m_key);
            if (v.is_null()) {
                return std::nullopt;
            } else {
                return this->m_backing_map.get(this->m_key).operator internal::bridge::binary().operator std::vector<uint8_t>();
            };
        }
    };
    template<>
    struct box<std::string> : public box_base<std::string> {
        using box_base<std::string>::box_base;
        using box_base<std::string>::operator=;
        std::string operator*() {
            return this->m_backing_map.get(this->m_key).operator std::string();
        }
    };
    template<>
    struct box<std::optional<std::string>> : public box_base<std::optional<std::string>> {
        using box_base<std::optional<std::string>>::box_base;
        using box_base<std::optional<std::string>>::operator=;
        std::optional<std::string> operator*() {
            auto v = this->m_backing_map.get(this->m_key);
            if (v.is_null()) {
                return std::nullopt;
            } else {
                return this->m_backing_map.get(this->m_key).operator std::string();
            };
        }
    };

    //MARK: - Boxed Link
    template<typename V>
    struct box<managed<V*>> : public box_base<managed<V*>> {
        using box_base<managed<V*>>::box_base;
        using box_base<managed<V*>>::operator=;
        using box_base<managed<V*>>::operator==;

        bool operator==(const V*& rhs) const {
            auto a = const_cast<box<V*> *>(this)->m_backing_map.get_object(this->m_key);
            auto &b = rhs->m_managed.m_obj;
            if (this->m_realm != rhs->m_managed.m_realm) {
                return false;
            }
            return a.get_table() == b.get_table() && a.get_key() == b.get_key();
        }
        bool operator!=(const V*& rhs) const {
            return !this->operator==(rhs);
        }

        bool operator==(const managed<V*> &rhs) const {
           // const realm::experimental::box<realm::experimental::managed<realm::experimental::AllTypesObjectEmbedded *>> *' to 'box<realm::experimental::AllTypesObjectEmbedded *> *'
            auto a = const_cast<box<managed<V*>> *>(this)->m_backing_map.get_object(this->m_key);
            auto &b = rhs.m_obj;
            if (this->m_realm != *rhs.m_realm) {
                return false;
            }
            return a.get_key() == b->get_key();
        }
        bool operator!=(const managed<V*> rhs) const {
            return !this->operator==(rhs);
        }

        bool operator==(const managed<V> &rhs) const {
            auto a = const_cast<box<managed<V*>> *>(this)->m_backing_map.get_object(this->m_key);
            auto &b = rhs.m_obj;
            if (this->m_realm != rhs.m_realm) {
                return false;
            }
            return a.get_key() == b.get_key();
        }
        bool operator!=(const managed<V> rhs) const {
            return !this->operator==(rhs);
        }

        typename managed<V*>::ref_type operator*() {
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

        typename managed<V*>::ref_type operator->() {
            auto obj = this->m_backing_map.get_object(this->m_key);
            auto m = managed<V>(std::move(obj), this->m_realm);
            std::apply([&m](auto &&...ptr) {
                std::apply([&](auto &&...name) {
                    ((m.*ptr).assign(&m.m_obj, &m.m_realm, m.m_obj.get_table().get_column_key(name)), ...);
                }, managed<V, void>::managed_pointers_names);
            }, managed<V, void>::managed_pointers());
            return {std::move(m)};
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

            std::apply([&m_obj, &o](auto && ...p) {
                (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                         m_obj, m_obj.get_table().get_column_key(p.name),
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

        bool operator==(const V& rhs) const {
            auto a = const_cast<box<managed<V*>> *>(this)->m_backing_map.get_object(this->m_key);
            auto &b = rhs.m_obj;
            if (this->m_realm != rhs.m_realm) {
                return false;
            }
            return a.get_key() == b.get_key();
        }
        bool operator!=(const V& rhs) const {
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

        [[nodiscard]] std::map<std::string, T> value() const {
            // unused
            return std::map<std::string, T>();
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

            iterator(size_t i, managed<std::map<std::string, T>>* parent)
                : m_i(i), m_parent(parent)
            {
            }
            size_t m_i;
            managed<std::map<std::string, T>>* m_parent;
        };

        size_t size()
        {
            return m_obj->get_dictionary(m_key).size();
        }

        iterator begin()
        {
            return iterator(0, this);
        }

        iterator end()
        {
            return iterator(size(), this);
        }

        iterator find(const std::string& key) {
            // Dictionary's `find` searches for the index of the value and not the key.
            auto d = m_obj->get_dictionary(m_key);
            auto i = d.find_any_key(key);
            if (i == size_t(-1)) {
                return iterator(size(), this);
            } else {
                return iterator(i, this);
            }
        }

        box<std::conditional_t<std::is_pointer_v<T>, managed<T>, T>>  operator[](const std::string &a) {
            if constexpr (std::is_pointer_v<T>) {
                return box<managed<T>>(m_obj->get_dictionary(m_key), a, *m_realm);
            } else {
                return box<T>(m_obj->get_dictionary(m_key), a, *m_realm);
            }
        }

        void erase(const std::string& key) {
            m_obj->get_dictionary(m_key).erase(key);
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
    };

} // namespace realm::experimental

#endif//CPPREALM_MANAGED_DICTIONARY_HPP
