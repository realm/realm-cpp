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

#ifndef CPPREALM_MANAGED_SET_HPP
#define CPPREALM_MANAGED_SET_HPP

#include <cpprealm/macros.hpp>
#include <cpprealm/notifications.hpp>
#include <cpprealm/observation.hpp>
#include <cpprealm/types.hpp>

#include <set>

namespace realm {

    template<typename T>
    struct managed<std::set<T>, std::enable_if_t<internal::type_info::is_primitive<T>::value>> : managed_base {
        using managed<std::set<T>>::managed_base::operator=;
        using value_type = T;

        class iterator {
        public:
            using value_type = T;

            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using reference = T&;
            using iterator_category = std::forward_iterator_tag;

            bool operator!=(const iterator& other) const
            {
                return !(*this == other);
            }

            bool operator==(const iterator& other) const
            {
                return (m_parent == other.m_parent) && (m_i == other.m_i);
            }

            T operator*() const noexcept
            {
                auto s = realm::internal::bridge::set(*m_parent->m_realm, *m_parent->m_obj, m_parent->m_key);
                return deserialize<T>(s.get_any(m_i));
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

            iterator(size_t i, managed<std::set<T>>* parent)
                : m_i(i), m_parent(parent)
            {
            }
            size_t m_i;
            managed<std::set<T>>* m_parent;
        };
        iterator begin()
        {
            return iterator(0, this);
        }

        iterator end()
        {
            return iterator(size(), this);
        }
        [[nodiscard]] std::set<T> detach() const {
            auto set = realm::internal::bridge::set(*m_realm, *m_obj, m_key);
            auto ret = std::set<T>();
            for(size_t i = 0; i < set.size(); i++) {
                ret.insert(deserialize<T>(set.get_any(i)));
            }
            return ret;
        }

        realm::notification_token observe(std::function<void(realm::collection_change)>&& fn) {
            auto set = std::make_shared<realm::internal::bridge::set>(*m_realm, *m_obj, m_key);
            realm::notification_token token = set->add_notification_callback(
                    std::make_shared<realm::collection_callback_wrapper>(
                            std::move(fn),
                            false));
            token.m_realm = *m_realm;
            token.m_set = set;
            return token;
        }

        void erase(const iterator& it)
        {
            auto set = internal::bridge::set(*m_realm, *m_obj, m_key);
            set.remove(serialize(*it));
        }

        std::pair<iterator, bool> insert(const T& v)
        {
            auto set = internal::bridge::set(*m_realm, *m_obj, m_key);
            if constexpr (internal::type_info::MixedPersistableConcept<T>::value) {
                std::pair<size_t, bool> res = set.insert(serialize<T>(v));
                return std::pair<iterator, bool>(iterator(res.first, this), res.second);
            } else {
                std::pair<size_t, bool> res = set.insert(v);
                return std::pair<iterator, bool>(iterator(res.first, this), res.second);
            }
        }

        iterator insert(const iterator&, const T& v)
        {
            auto set = internal::bridge::set(*m_realm, *m_obj, m_key);
            std::pair<size_t, bool> res = set.insert(v);
            return iterator(res.first, this);
        }

        iterator find(const T& v)
        {
            auto set = internal::bridge::set(*m_realm, *m_obj, m_key);
            size_t idx = set.find(serialize(v));
            if (idx == realm::not_in_collection)
                return iterator(size(), this);
            return iterator(idx, this);
        }
        void clear() {
            internal::bridge::set(*m_realm, *m_obj, m_key).remove_all();
        }

        size_t size()
        {
            return internal::bridge::set(*m_realm, *m_obj, m_key).size();
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

    template<typename T>
    struct managed<std::set<T*>> : managed_base {
        using managed<std::set<T*>>::managed_base::operator=;
        using value_type = managed<T>;

        class iterator {
        public:
            using value_type = managed<T>;

            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using reference = T&;
            using iterator_category = std::forward_iterator_tag;

            bool operator!=(const iterator& other) const
            {
                return !(*this == other);
            }

            bool operator==(const iterator& other) const
            {
                return (m_parent == other.m_parent) && (m_i == other.m_i);
            }

            managed<T> operator*() const noexcept
            {
                auto s = realm::internal::bridge::set(*m_parent->m_realm, *m_parent->m_obj, m_parent->m_key);
                managed<T> m(s.get_obj(m_i), *m_parent->m_realm);
                return {std::move(m)};
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

            iterator(size_t i, managed<std::set<T*>>* parent)
                : m_i(i), m_parent(parent)
            {
            }
            size_t m_i;
            managed<std::set<T*>>* m_parent;
        };
        iterator begin()
        {
            return iterator(0, this);
        }

        iterator end()
        {
            return iterator(size(), this);
        }
        [[nodiscard]] std::set<T*> detach() const {
            auto s = realm::internal::bridge::set(*m_realm, *m_obj, m_key);
            size_t count = s.size();
            if (count == 0)
                return std::set<T*>();
            auto ret = std::set<T*>();
            for(size_t i = 0; i < count; i++) {
                managed<T> m(s.get_obj(i), *m_realm);
                T* v = new T();
                auto assign = [&m, &v](auto& pair) {
                    (*v).*(std::decay_t<decltype(pair.first)>::ptr) = (m.*(pair.second)).detach();
                };
                auto zipped = internal::zip_tuples(managed<T>::schema.ps, managed<T>::managed_pointers());
                std::apply([&v, &m, &assign](auto && ...pair) {
                    (assign(pair), ...);
                }, zipped);

                ret.insert(v);
            }
            return ret;
        }

        realm::notification_token observe(std::function<void(realm::collection_change)>&& fn) {
            auto set = std::make_shared<realm::internal::bridge::set>(*m_realm, *m_obj, m_key);
            realm::notification_token token = set->add_notification_callback(
                    std::make_shared<realm::collection_callback_wrapper>(
                            std::move(fn),
                            false));
            token.m_realm = *m_realm;
            token.m_set = set;
            return token;
        }

        void erase(const iterator& it)
        {
            auto set = internal::bridge::set(*m_realm, *m_obj, m_key);
            set.remove(it.operator*().m_obj.get_key());
        }

        std::pair<iterator, bool> insert(T* value)
        {
            auto set = internal::bridge::set(*m_realm, *m_obj, m_key);
            auto table = m_obj->get_target_table(m_key);
            internal::bridge::obj m_obj;
            if constexpr (managed<T>::schema.HasPrimaryKeyProperty) {
                auto pk = (*value).*(managed<T>::schema.primary_key().ptr);
                m_obj = table.create_object_with_primary_key(realm::internal::bridge::mixed(serialize(pk.value)));
            } else {
                m_obj = table.create_object();
            }
            std::apply([&m_obj, &value, realm = *m_realm](auto && ...p) {
                (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                         m_obj, m_obj.get_table().get_column_key(p.name), realm,
                         (*value).*(std::decay_t<decltype(p)>::ptr)), ...);
            }, managed<T, void>::schema.ps);
            if (!managed<T>::schema.is_embedded()) {
                set.insert(m_obj.get_key());
            }
            std::pair<size_t, bool> res = set.insert(m_obj.get_key());
            return std::pair<iterator, bool>(iterator(res.first, this), res.second);
        }

        iterator insert(const iterator&, T* value)
        {
            auto set = internal::bridge::set(*m_realm, *m_obj, m_key);
            auto table = m_obj->get_target_table(m_key);
            internal::bridge::obj m_obj;
            if constexpr (managed<T>::schema.HasPrimaryKeyProperty) {
                auto pk = (*value).*(managed<T>::schema.primary_key().ptr);
                m_obj = table.create_object_with_primary_key(realm::internal::bridge::mixed(serialize(pk.value)));
            } else {
                m_obj = table.create_object();
            }
            std::apply([&m_obj, &value, realm = *m_realm](auto && ...p) {
                (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                         m_obj, m_obj.get_table().get_column_key(p.name), realm,
                         (*value).*(std::decay_t<decltype(p)>::ptr)), ...);
            }, managed<T, void>::schema.ps);
            std::pair<size_t, bool> res = set.insert(m_obj.get_key());
            return iterator(res.first, this);
        }

        std::pair<iterator, bool> insert(const managed<T>& value)
        {
            auto set = internal::bridge::set(*m_realm, *m_obj, m_key);
            std::pair<size_t, bool> res = set.insert(value.m_obj.get_key());
            return std::pair<iterator, bool>(iterator(res.first, this), res.second);

        }

        iterator insert(const iterator&, const managed<T>& value)
        {
            auto set = internal::bridge::set(*m_realm, *m_obj, m_key);
            std::pair<size_t, bool> res = set.insert(value.m_obj.get_key());
            return iterator(res.first, this);
        }

        std::pair<iterator, bool> insert(const managed<T*>& value)
        {
            auto set = internal::bridge::set(*m_realm, *m_obj, m_key);
            std::pair<size_t, bool> res = set.insert(value.m_obj.get_key());
            return std::pair<iterator, bool>(iterator(res.first, this), res.second);

        }

        iterator insert(const iterator&, const managed<T*>& value)
        {
            auto set = internal::bridge::set(*m_realm, *m_obj, m_key);
            std::pair<size_t, bool> res = set.insert(value.m_obj.get_key());
            return iterator(res.first, this);
        }

        iterator find(const managed<T>& v)
        {
            auto set = internal::bridge::set(*m_realm, *m_obj, m_key);
            size_t idx = set.find(v.m_obj.get_key());
            if (idx == realm::not_in_collection)
                return iterator(size(), this);
            return iterator(idx, this);
        }

        iterator find(const managed<T*>& v)
        {
            auto set = internal::bridge::set(*m_realm, *m_obj, m_key);
            size_t idx = set.find(v.m_obj->get_key());
            if (idx == realm::not_in_collection)
                return iterator(size(), this);
            return iterator(idx, this);
        }
        void clear() {
            internal::bridge::set(*m_realm, *m_obj, m_key).remove_all();
        }

        size_t size()
        {
            return internal::bridge::set(*m_realm, *m_obj, m_key).size();
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

#endif//CPPREALM_MANAGED_SET_HPP
