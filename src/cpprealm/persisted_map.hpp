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

#ifndef REALM_PERSISTED_MAP_HPP
#define REALM_PERSISTED_MAP_HPP

#include <cpprealm/persisted.hpp>
#include <cpprealm/notifications.hpp>

#include <realm/object-store/dictionary.hpp>

namespace realm {

    template <typename T>
    class persisted_map_base : public persisted_base<T> {
    public:
        using value_type = typename T::value_type;
        using mapped_type = typename T::mapped_type;
        using size_type = typename T::size_type;

        class iterator {
        public:
            using difference_type = size_type;
            using pointer = value_type*;
            using reference = value_type;
            using iterator_category = std::input_iterator_tag;

            bool operator!=(const iterator& other) const
            {
                return !(*this == other);
            }

            bool operator==(const iterator& other) const
            {
                return (m_parent == other.m_parent) &&
                    m_parent->m_object ? (m_i == other.m_i) : (m_idx == other.m_idx);
            }

            reference operator*() noexcept
            {
                if (m_parent->m_object) {
                    auto pair = (*m_parent).m_object->obj().get_dictionary(m_parent->managed).get_pair(m_i);
                    return { pair.first.template get<StringData>(),
                            pair.second.template get<typename type_info::persisted_type<mapped_type>::type>() };
                } else {
                    return *m_idx;
                }
            }

            iterator& operator++()
            {
                if (!m_parent->m_object) {
                    m_idx++;
                } else {
                    this->m_i++;
                }
                return *this;
            }

            const iterator& operator++(int i)
            {
                if (!m_parent->m_object) {
                    m_idx+=i;
                } else {
                    this->m_i += i;
                }
                return *this;
            }
        private:
            friend struct persisted_map_base<T>;

            iterator(typename T::iterator&& idx, persisted_map_base<T>* parent)
                    : m_idx(std::move(idx))
                    , m_parent(parent)
            {
            }
            iterator(size_t i, persisted_map_base<T>* parent)
                    : m_i(i)
                    , m_parent(parent)
            {
            }
            union {
                typename T::iterator m_idx;
                size_t m_i;
            };
            persisted_map_base<T>* m_parent;
            value_type value;
        };

        iterator begin()
        {
            if (this->m_object) {
                return iterator(0, this);
            } else {
                return iterator(this->unmanaged.begin(), this);
            }
        }

        iterator end()
        {
            if (this->m_object) {
                return iterator(size(), this);
            } else {
                return iterator(this->unmanaged.end(), this);
            }
        }

        size_t size()
        {
            if (this->m_object) {
                return m_backing_map.size();
            } else {
                return this->unmanaged.size();
            }
        }

        void erase(size_type pos);
        void clear();
        notification_token observe(std::function<void(collection_change<T>)>);

        /// Make this container property managed
        /// @param object The parent object
        /// @param col_key The column key for this property
        /// @param realm The Realm instance managing the parent.
        void assign(const Obj& object, const ColKey& col_key, SharedRealm realm) {
            this->m_object = Object(realm, object);
            this->managed = ColKey(col_key);
            m_backing_map = object_store::Dictionary(realm, object, this->managed);
        }

        typename T::mapped_type operator[](typename T::key_type& key) {
            if (auto& obj = this->m_object) {
                return obj->obj().get_dictionary(this->managed)[key];
            } else {
                return this->unmanaged[key];
            }
        }
        typename T::mapped_type operator[](size_t& key) {
            if (auto& obj = this->m_object) {
                Dictionary dictionary = obj->obj().get_dictionary(this->managed);
                return dictionary[dictionary.get_key(key)];
            } else {
                return this->unmanaged[key];
            }
        }

    protected:
        object_store::Dictionary m_backing_map;
        template <typename>
        friend class persisted_primitive_container_base;
        template <typename>
        friend class persisted_object_container_base;
    };

    template <typename T>
    class persisted_primitive_map_base : public persisted_map_base<T> {
    public:
        using persisted_map_base<T>::persisted_map_base;
        using value_type = typename T::value_type;
        using size_type = typename T::size_type;
        using typename persisted_map_base<T>::iterator;
        using persisted_map_base<T>::end;
        using persisted_map_base<T>::size;
        using persisted_map_base<T>::erase;
        using persisted_map_base<T>::clear;
    };

    template <typename T>
    void persisted_map_base<T>::clear() {
        if (this->m_object) {
            m_backing_map.remove_all();
        } else {
            this->unmanaged.clear();
        }
    }

    template <typename T>
    notification_token persisted_map_base<T>::observe(std::function<void(collection_change<T>)> handler)
    {
        if (!this->m_object) {
            throw std::runtime_error("Only collections which are managed by a Realm support change notifications");
        }

        if (!this->m_object->is_valid()) {
            throw std::runtime_error("Only collections which are managed by a Realm support change notifications");
        }

        notification_token token;
        Realm::Config config = this->m_object->realm()->config();
        auto tsr = ThreadSafeReference(m_backing_map);
        auto tsr_o = ThreadSafeReference(*this->m_object);
        config.scheduler = LooperScheduler::make();
        auto realm = Realm::get_shared_realm(config);
        token.m_realm = realm;
        auto scheduler = std::reinterpret_pointer_cast<LooperScheduler>(token.m_realm->scheduler());
        scheduler->l.push(std::packaged_task<void()>([&token, &handler, &tsr, &tsr_o, this]() {
            token.m_object = tsr_o.template resolve<Object>(token.m_realm);
            token.m_dictionary = tsr.template resolve<object_store::Dictionary>(token.m_realm);
            token.m_token = token.m_dictionary.add_notification_callback(collection_callback_wrapper<T> { std::move(handler), *static_cast<persisted<T>*>(this), false });
        })).get_future().get();
        return std::move(token);
    }

    template <typename mapped_type>
    struct box_base {
        box_base(mapped_type& val)
                : m_val(val)
        {
            is_managed = false;
        }
        box_base(realm::object_store::Dictionary& backing_map,
                 std::string key)
                : m_backing_map(backing_map)
                , m_key(key)
        {
            is_managed = true;
        }
        box_base& operator=(const mapped_type& o) {
            if (is_managed) {
                m_backing_map.get().insert(m_key, std::move(o));
            } else {
                m_val = o;
            }
            return *this;
        }
        box_base& operator=(mapped_type&& o) {
            if (is_managed) {
                m_backing_map.get().insert(m_key,
                                           type_info::persisted_type<mapped_type>::convert_if_required(std::move(o)));
            } else {
                m_val.get() = std::move(o);
            }
            return *this;
        }

        bool operator==(const mapped_type& rhs) const {
            if (is_managed) {
                return type_info::persisted_type<mapped_type>::convert_if_required(rhs) ==
                       this->m_backing_map.get().template get<typename type_info::persisted_type<mapped_type>::type>(this->m_key);
            } else {
                return rhs == m_val.get();
            }
        }

        union {
            std::reference_wrapper<realm::object_store::Dictionary> m_backing_map;
            std::reference_wrapper<mapped_type> m_val;
        };
        bool is_managed;
        std::string m_key;
    };
    template <typename V, typename = void>
    struct box;
    template <typename V>
    struct box<V, type_info::IntPersistable<V>> : public box_base<V> {
        using box_base<V>::box_base;
        using box_base<V>::operator=;
    };
    template <typename V>
    struct box<V, type_info::BoolPersistable<V>> : public box_base<V> {
        using box_base<V>::box_base;
        using box_base<V>::operator=;
    };
    template <typename V>
    struct box<V, type_info::EnumPersistable<V>> : public box_base<V> {
        using box_base<V>::box_base;
        using box_base<V>::operator=;
    };
    template <typename V>
    struct box<V, type_info::UUIDPersistable<V>> : public box_base<V> {
        using box_base<V>::box_base;
        using box_base<V>::operator=;
    };
    template <typename V>
    struct box<V, type_info::BinaryPersistable<V>> : public box_base<V> {
        using box_base<V>::box_base;
        using box_base<V>::operator=;
    };
    template <typename V>
    struct box<V, type_info::StringPersistable<V>> : public box_base<V> {
        using box_base<V>::box_base;
        using box_base<V>::operator=;

        bool operator==(const char* rhs) const {
            if (this->is_managed) {
                return rhs == this->m_backing_map.get().template get<typename type_info::persisted_type<V>::type>(this->m_key);
            } else {
                return rhs == this->m_val.get();
            }
        }
    };

    template <typename T>
    struct persisted<T, type_info::MapPrimitivePersistable<T>> : public persisted_primitive_map_base<T> {
        using persisted_primitive_map_base<T>::persisted_primitive_map_base;
        using persisted_primitive_map_base<T>::operator=;
        using value_type = typename T::value_type;
        using mapped_type = typename T::mapped_type;
        using size_type = typename T::size_type;
        using typename persisted_map_base<T>::iterator;
        using persisted_primitive_map_base<T>::end;
        using persisted_primitive_map_base<T>::size;
        using persisted_primitive_map_base<T>::erase;
        using persisted_primitive_map_base<T>::clear;
        using persisted_primitive_map_base<T>::observe;

        persisted& operator=(const T& o) override {
            if (std::optional<Object>& obj = this->m_object) {
                realm::Dictionary dictionary = obj->obj().get_dictionary(this->managed);
                dictionary.clear();
                for (auto& [k, v] : o) {
                    dictionary.insert(k, type_info::persisted_type<mapped_type>::convert_if_required(v));
                }
            } else {
                new (&this->unmanaged) T(o);
            }
            return *this;
        }

        T operator *() const {
            if (std::optional<Object>& obj = this->m_object) {
                Dictionary dictionary = obj->obj().get_dictionary(this->managed);
                T map;
                for (auto [k, v] : dictionary) {
                    map.insert(k, v);
                }
                return map;
            } else {
                return this->unmanaged;
            }
        }

        box<mapped_type> operator[](typename T::key_type a) {
            if (this->m_object) {
                return box<mapped_type>(this->m_backing_map, a);
            } else {
                return box<mapped_type>(this->unmanaged[a]);
            }
        }
    };

    template <typename T>
    struct persisted<T, type_info::MapMixedPersistable<T>> : public persisted_map_base<T> {
        using persisted_map_base<T>::persisted_map_base;
        using persisted_map_base<T>::operator=;
        using value_type = typename T::value_type;
        using mapped_type = typename T::mapped_type;
        using size_type = typename T::size_type;
        using typename persisted_map_base<T>::iterator;
        using persisted_map_base<T>::end;
        using persisted_map_base<T>::size;
        using persisted_map_base<T>::erase;
        using persisted_map_base<T>::clear;
        using persisted_map_base<T>::observe;

        persisted& operator=(const T& o) override {
            if (std::optional<Object>& obj = this->m_object) {
                realm::Dictionary dictionary = obj->obj().get_dictionary(this->managed);
                dictionary.clear();
                for (auto& [k, v] : o) {
                    dictionary.insert(k, type_info::persisted_type<mapped_type>::convert_if_required(v));
                }
            } else {
                new (&this->unmanaged) T(o);
            }
            return *this;
        }

        T operator *() const {
            if (std::optional<Object>& obj = this->m_object) {
                Dictionary dictionary = obj->obj().get_dictionary(this->managed);
                T map;
                for (auto [k, v] : dictionary) {
                    map.insert(k, v);
                }
                return map;
            } else {
                return this->unmanaged;
            }
        }

        box<mapped_type> operator[](typename T::key_type a) {
            if (this->m_object) {
                if constexpr (realm::type_info::BinaryPersistableConcept<typename T::mapped_type>::value) {
                    return box<mapped_type>(this->m_backing_map, a);
                } else if constexpr (type_info::MixedPersistableConcept<typename T::mapped_type>::value) {
                    return type_info::mixed_to_variant<typename T::mapped_type>(this->m_backing_map.template get<typename type_info::persisted_type<typename T::value_type>::type>(a));
                } else {
                    return box<mapped_type>(this->m_backing_map, a);
                }
            } else {
                return box<mapped_type>(this->unmanaged[a]);
            }
        }
    };

    template <typename T>
    struct persisted<T, type_info::MapObjectPersistable<T>> : public persisted_map_base<T> {
        using persisted_map_base<T>::persisted_map_base;
        using persisted_map_base<T>::operator=;
        using value_type = typename T::value_type;
        using mapped_type = typename T::mapped_type;
        using size_type = typename T::size_type;
        using typename persisted_map_base<T>::iterator;
        using persisted_map_base<T>::end;
        using persisted_map_base<T>::size;
        using persisted_map_base<T>::erase;
        using persisted_map_base<T>::clear;
        using persisted_map_base<T>::observe;

        persisted& operator=(const T& o) override {
            if (std::optional<Object>& obj = this->m_object) {
                realm::Dictionary dictionary = obj->obj().get_dictionary(this->managed);
                dictionary.clear();
                for (auto& [k, v] : o) {
                    dictionary.insert(k, type_info::persisted_type<mapped_type>::convert_if_required(v));
                }
            } else {
                new (&this->unmanaged) T(o);
            }
            return *this;
        }

        T operator *() const {
            if (std::optional<Object>& obj = this->m_object) {
                Dictionary dictionary = obj->obj().get_dictionary(this->managed);
                T map;
                for (auto [k, v] : dictionary) {
                    map.insert(k, v);
                }
                return map;
            } else {
                return this->unmanaged;
            }
        }

        box<mapped_type> operator[](typename T::key_type a) {
            if (this->m_object) {
                return box<mapped_type>(this->m_backing_map, a);
            } else {
                return box<mapped_type>(this->unmanaged[a]);
            }
        }
    };
}

#endif //REALM_PERSISTED_MAP_HPP
