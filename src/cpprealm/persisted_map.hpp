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
#include <cpprealm/internal/bridge/dictionary.hpp>

#include <utility>

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
                    auto pair = (*m_parent).managed.get_pair(m_i);
                    return { pair.first, persisted<mapped_type>::deserialize(pair.second) };
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


        persisted_map_base() {
            new (&this->unmanaged) std::map<std::string, T>();
        }
        persisted_map_base(const persisted_map_base& v) {
            if (v.is_managed()) {
                new (&this->managed) internal::bridge::dictionary(v.managed);
            } else {
                new (&this->unmanaged) T(v.unmanaged);
            }
        }
        ~persisted_map_base() {
            if (this->is_managed()) {
                this->managed.~dictionary();
            } else {
                this->unmanaged.~map();
            }
        }
        persisted_map_base& operator=(const std::map<std::string, typename T::mapped_type>& o) {
            if (this->is_managed()) {
                this->managed.clear();
                for (auto& [k, v] : o) {
                    if constexpr (internal::type_info::is_optional<typename T::mapped_type>::value) {
                        this->managed.insert(k, internal::bridge::mixed(persisted<typename T::mapped_type>::serialize(*v)));
                    } else {
                        this->managed.insert(k, internal::bridge::mixed(persisted<typename T::mapped_type>::serialize(v)));
                    }
                }
            } else {
                new (&this->unmanaged) std::map<std::string, typename T::mapped_type>(o);
            }
            return *this;
        }
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
                return managed.size();
            } else {
                return this->unmanaged.size();
            }
        }

        void erase(size_type pos);
        void clear();
        notification_token observe(std::function<void(collection_change<T>)>);

        typename T::mapped_type operator[](typename T::key_type& key) {
            if (auto& obj = this->m_object) {
                return obj->obj().get_dictionary(this->managed)[key];
            } else {
                return this->unmanaged[key];
            }
        }
        typename T::mapped_type operator[](size_t& key) {
            if (auto& obj = this->m_object) {
                auto dictionary = obj->obj().get_dictionary(this->managed);
                return dictionary[dictionary.get_key(key)];
            } else {
                return this->unmanaged[key];
            }
        }

    protected:
        union {
            std::map<std::string, typename T::mapped_type> unmanaged;
            internal::bridge::dictionary managed;
        };
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
            managed.remove_all();
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

        auto scheduler = NoPlatformScheduler::make();
        notification_token token;
        auto config = this->m_object->get_realm().get_config();
        config.set_scheduler(scheduler);
        internal::bridge::realm r(config);
        token.m_realm = r;
        auto tsr = internal::bridge::thread_safe_reference(managed);
        scheduler->invoke([&handler, this, &token, tsr = std::move(tsr)]() mutable {
            auto managed = token.m_realm.resolve<internal::bridge::dictionary>(std::move(tsr));
            token.m_token = managed.add_notification_callback(
                    std::make_shared<collection_callback_wrapper<T>>(
                            std::move(handler),
                            *static_cast<persisted<T>*>(this),
                            false)
            );

            token.m_dictionary = managed;
        });
        return token;
    }

    template <typename mapped_type>
    struct box_base {
        box_base(mapped_type& val)
                : m_val(val)
        {
            is_managed = false;
        }
        box_base(internal::bridge::dictionary& backing_map,
                 std::string key,
                 internal::bridge::object* object)
                : m_backing_map(backing_map)
                , m_key(std::move(key))
                , m_object(object)
        {
            is_managed = true;
        }

        ////////
        template <typename T>
        std::enable_if_t<std::is_base_of_v<T, object_base<T>>, mapped_type&> // NOLINT(misc-unconventional-assign-operator)
        operator=(const T o) {
            return *this;
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
                if constexpr (internal::type_info::is_optional<mapped_type>::value) {
                    if (o) {
                        m_backing_map.get().insert(m_key,
                                                   persisted<mapped_type>::serialize(std::move(*o)));
                    } else {
                        m_backing_map.get().insert(m_key, internal::bridge::mixed(std::nullopt));
                    }
                } else {
                    m_backing_map.get().insert(m_key,
                                               persisted<mapped_type>::serialize(std::move(o)));
                }
            } else {
                m_val.get() = std::move(o);
            }
            return *this;
        }

        void erase(const std::string& key) {
            m_backing_map.get().remove(m_key);
        }

        bool operator==(const mapped_type& rhs) const {
            if (is_managed) {
                if constexpr (internal::type_info::is_optional<mapped_type>::value) {
                    if constexpr (std::is_base_of_v<object_base<typename mapped_type::value_type>, typename mapped_type::value_type>) {
                        return rhs == this->operator*();
                    }
                } else {
                    return persisted<mapped_type>::serialize(rhs) ==
                           this->m_backing_map.get().template get<typename internal::type_info::type_info<mapped_type>::internal_type>(
                                   this->m_key);
                }
            } else {
                return rhs == m_val.get();
            }
        }

        mapped_type operator *() const {
            if (is_managed) {
                typename mapped_type::value_type cls;
                auto obj = m_backing_map.get().template get<internal::bridge::obj>(m_key);
                if (!obj.is_valid()) {
                    return std::nullopt;
                }
                cls.assign_accessors(internal::bridge::object(m_object->get_realm(), obj));
                return cls;
            } else {
                return m_val.get();
            }
        }
        union {
            std::reference_wrapper<internal::bridge::dictionary> m_backing_map;
            std::reference_wrapper<mapped_type> m_val;
        };
        bool is_managed;
        std::string m_key;
        internal::bridge::object* m_object = nullptr;
    };
    template <typename V, typename = void>
    struct box;
    template <>
    struct box<int64_t> : public box_base<int64_t> {
        using box_base<int64_t>::box_base;
        using box_base<int64_t>::operator=;
    };
    template <>
    struct box<bool> : public box_base<bool> {
        using box_base<bool>::box_base;
        using box_base<bool>::operator=;
    };
    template <typename V>
    struct box<V, std::enable_if_t<std::is_enum_v<V>>> : public box_base<V> {
        using box_base<V>::box_base;
        using box_base<V>::operator=;
    };
    template <>
    struct box<uuid> : public box_base<uuid> {
        using box_base<uuid>::box_base;
        using box_base<uuid>::operator=;
    };
    template <>
    struct box<std::vector<uint8_t>> : public box_base<std::vector<uint8_t>> {
        using box_base<std::vector<uint8_t>>::box_base;
        using box_base<std::vector<uint8_t>>::operator=;
    };
    template <>
    struct box<std::string> : public box_base<std::string> {
        using box_base<std::string>::box_base;
        using box_base<std::string>::operator=;
    };
    template <typename V>
struct box<std::optional<V>, std::enable_if_t<std::is_base_of_v<object_base<V>, V>>> : public box_base<std::optional<V>> {
        using box_base<std::optional<V>>::box_base;
        using box_base<std::optional<V>>::operator=;
        using box_base<std::optional<V>>::operator==;
    };

    template <typename T>
    struct persisted<std::map<std::string, T>> : public persisted_map_base<std::map<std::string, T>> {
        using persisted_map_base<std::map<std::string, T>>::persisted_map_base;
        using persisted_map_base<std::map<std::string, T>>::operator=;
        using value_type = typename std::map<std::string, T>::value_type;
        using mapped_type = typename std::map<std::string, T>::mapped_type;
        using size_type = typename std::map<std::string, T>::size_type;
        using typename persisted_map_base<std::map<std::string, T>>::iterator;
        using persisted_map_base<std::map<std::string, T>>::end;
        using persisted_map_base<std::map<std::string, T>>::size;
        using persisted_map_base<std::map<std::string, T>>::erase;
        using persisted_map_base<std::map<std::string, T>>::clear;
        using persisted_map_base<std::map<std::string, T>>::observe;

        std::map<std::string, T> operator *() const {
            return {};
        }

        box<mapped_type> operator[](const std::string& a) {
            if (this->m_object) {
                return box<mapped_type>(this->managed, a, this->m_object);
            } else {
                return box<mapped_type>(this->unmanaged[a]);
            }
        }
    protected:
        void manage(internal::bridge::object *object, internal::bridge::col_key &&col_key) override {
            auto managed = object->get_dictionary(col_key);
            for (auto& [k, v] : this->unmanaged) {
                if constexpr (internal::type_info::is_optional<T>::value) {
                    if constexpr (std::is_base_of_v<realm::object<typename T::value_type>, typename T::value_type>) {
                        (*v).manage(object->obj().get_target_table(col_key),
                                 object->get_realm());
                        managed.insert(k, internal::bridge::mixed(persisted<T>::serialize(*v)));
                    } else if constexpr (std::is_base_of_v<embedded_object<typename T::value_type>, typename T::value_type>) {
                        managed.insert_embedded(k);
                    }
                } else {
                    managed.insert(k, internal::bridge::mixed(persisted < T > ::serialize(v)));
                }
            }
            this->unmanaged.~map<std::string, T>();
            assign_accessor(object, std::move(col_key));
        }

        void assign_accessor(internal::bridge::object *object, internal::bridge::col_key &&col_key) override {
            this->m_object = object;
            new (&this->managed) internal::bridge::dictionary(object->get_dictionary(col_key));
        }
        __cpp_realm_friends
    };
}

template <typename T>
inline typename std::enable_if<std::is_base_of<realm::object_base<T>, T>::value, std::ostream>::type&
operator<< (std::ostream& stream, const realm::box<T>& object)
{
    stream << *object;
    return stream;
}

#endif //REALM_PERSISTED_MAP_HPP
