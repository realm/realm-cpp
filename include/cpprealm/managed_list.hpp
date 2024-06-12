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

#ifndef CPPREALM_MANAGED_LIST_HPP
#define CPPREALM_MANAGED_LIST_HPP

#include <cpprealm/macros.hpp>
#include <cpprealm/notifications.hpp>
#include <cpprealm/observation.hpp>
#include <cpprealm/types.hpp>

namespace realm {

    template<typename T>
    struct managed<std::vector<T>, std::enable_if_t<internal::type_info::is_primitive<T>::value>> : managed_base {
        using managed<std::vector<T>>::managed_base::operator=;

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

            T operator*() const noexcept
            {
                return (*m_parent)[m_i];
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

            iterator(size_t i, managed<std::vector<T>>* parent)
                : m_i(i), m_parent(parent)
            {
            }
            size_t m_i;
            managed<std::vector<T>>* m_parent;
        };
        iterator begin()
        {
            return iterator(0, this);
        }

        iterator end()
        {
            return iterator(size(), this);
        }
        [[nodiscard]] std::vector<T> detach() const {
            auto list = realm::internal::bridge::list(*m_realm, *m_obj, m_key);
            using U = typename internal::type_info::type_info<T>::internal_type;

            size_t count = list.size();
            if (count == 0)
                return std::vector<T>();

            auto ret = std::vector<T>();
            ret.reserve(count);
            for(size_t i = 0; i < count; i++) {
                if constexpr (internal::type_info::MixedPersistableConcept<T>::value) {
                    ret.push_back(deserialize<T>(realm::internal::bridge::get<U>(list, i)));
                } else if constexpr (std::is_enum_v<T>) {
                    ret.push_back(deserialize<T>(realm::internal::bridge::get<U>(list, i)));
                } else {
                    ret.push_back(deserialize(realm::internal::bridge::get<U>(list, i)));
                }
            }

            return ret;
        }

        [[nodiscard]] results<T> as_results() const {
            return results<T>(realm::internal::bridge::list(*m_realm, *m_obj, m_key).as_results());
        }

        realm::notification_token observe(std::function<void(realm::collection_change)>&& fn) {
            auto list = std::make_shared<realm::internal::bridge::list>(*m_realm, *m_obj, m_key);
            realm::notification_token token = list->add_notification_callback(
                                                          std::make_shared<realm::collection_callback_wrapper>(
                                                                  std::move(fn),
                                                                  false));
            token.m_realm = *m_realm;
            token.m_list = list;
            return token;
        }

        // TODO: emulate a reference to the value.
        T operator[](size_t idx) const {
            auto list = realm::internal::bridge::list(*m_realm, *m_obj, m_key);
            using U = typename internal::type_info::type_info<T>::internal_type;
            if constexpr (internal::type_info::MixedPersistableConcept<T>::value) {
                return deserialize<T>(realm::internal::bridge::get<U>(list, idx));
            } else if constexpr (std::is_enum_v<T>) {
                return static_cast<T>(deserialize<T>(realm::internal::bridge::get<U>(list, idx)));
            } else {
                return deserialize(realm::internal::bridge::get<U>(list, idx));
            }
        }

        void pop_back() {
            internal::bridge::list(*m_realm, *m_obj, m_key).remove(size() - 1);
        }
        void erase(size_t idx) {
            internal::bridge::list(*m_realm, *m_obj, m_key).remove(idx);
        }
        void clear() {
            internal::bridge::list(*m_realm, *m_obj, m_key).remove_all();
        }
        void push_back(const T& value) {
            auto list = internal::bridge::list(*m_realm, *m_obj, m_key);
            list.add(serialize(value));
        }
        size_t size() {
            return internal::bridge::list(*m_realm, *m_obj, m_key).size();
        }
        size_t find(const T& a) {
            if constexpr (std::is_enum_v<T>) {
                return internal::bridge::list(*m_realm, *m_obj, m_key).find(static_cast<int64_t>(a));
            } else {
                return internal::bridge::list(*m_realm, *m_obj, m_key).find(a);
            }
        }
        void set(size_t pos, const T& a) {
            internal::bridge::list(*m_realm, *m_obj, m_key).set(pos, a);
        }

        results<T> sort(bool ascending) {
            return results<T>(internal::bridge::list(*m_realm, *m_obj, m_key)
                                      .sort(std::vector<internal::bridge::sort_descriptor>({{"self", ascending}})));
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
    struct managed<std::vector<T*>> : managed_base {
    public:
        [[nodiscard]] std::vector<T*> detach() const {
            auto list = realm::internal::bridge::list(*m_realm, *m_obj, m_key);
            size_t count = list.size();
            if (count == 0)
                return std::vector<T*>();
            auto ret = std::vector<T*>();
            ret.reserve(count);
            for(size_t i = 0; i < count; i++) {
                managed<T> m(realm::internal::bridge::get<internal::bridge::obj>(list, i), *m_realm);
                T* v = new T();
                auto assign = [&m, &v](auto& pair) {
                    (*v).*(std::decay_t<decltype(pair.first)>::ptr) = (m.*(pair.second)).detach();
                };
                auto zipped = internal::zip_tuples(managed<T>::schema.ps, managed<T>::managed_pointers());
                std::apply([&v, &m, &assign](auto && ...pair) {
                    (assign(pair), ...);
                }, zipped);

                ret.push_back(v);
            }
            return ret;
        }

        [[nodiscard]] results<T*> as_results() const {
            return results<T*>(realm::internal::bridge::list(*m_realm, *m_obj, m_key).as_results());
        }

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
                auto list = realm::internal::bridge::list(*m_parent->m_realm, *m_parent->m_obj, m_parent->m_key);
                managed<T> m(realm::internal::bridge::get<realm::internal::bridge::obj>(list, m_i), *m_parent->m_realm);
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

            iterator(size_t i, managed<std::vector<T*>>* parent)
                : m_i(i), m_parent(parent)
            {
            }
            size_t m_i;
            managed<std::vector<T*>>* m_parent;
        };

        iterator begin()
        {
            return iterator(0, this);
        }

        iterator end()
        {
            return iterator(size(), this);
        }

        void pop_back() {
            internal::bridge::list(*m_realm, *m_obj, m_key).remove(size() - 1);
        }
        void erase(size_t idx) {
            internal::bridge::list(*m_realm, *m_obj, m_key).remove(idx);
        }
        void clear() {
            internal::bridge::list(*m_realm, *m_obj, m_key).remove_all();
        }
        void push_back(T* value)
        {
            auto list = internal::bridge::list(*m_realm, *m_obj, m_key);
            auto table = m_obj->get_target_table(m_key);
            internal::bridge::obj m_obj;
            if constexpr (managed<T>::schema.HasPrimaryKeyProperty) {
                auto pk = (*value).*(managed<T>::schema.primary_key().ptr);
                m_obj = table.create_object_with_primary_key(realm::internal::bridge::mixed(serialize(pk.value)));
            } else if (managed<T>::schema.is_embedded()) {
                m_obj = list.add_embedded();
            } else {
                m_obj = table.create_object();
            }
            std::apply([&m_obj, &value, realm = *m_realm](auto && ...p) {
                (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                         m_obj, m_obj.get_table().get_column_key(p.name), realm,
                         (*value).*(std::decay_t<decltype(p)>::ptr)), ...);
            }, managed<T, void>::schema.ps);
            if (!managed<T>::schema.is_embedded()) {
                list.add(m_obj.get_key());
            }
        }
        void push_back(const managed<T>& value)
        {
            auto list = internal::bridge::list(*m_realm, *m_obj, m_key);
            if (!managed<T>::schema.is_embedded()) {
                list.add(value.m_obj.get_key());
            } else {
                throw std::logic_error("Cannot add existing embedded object to managed list.");
            }
        }
        void push_back(const managed<T*>& value)
        {
            if (!managed<T>::schema.is_embedded()) {
                auto list = internal::bridge::list(*m_realm, *m_obj, m_key);
                list.add(value.m_obj->get_key());
            } else {
                throw std::logic_error("Cannot add existing embedded object to managed list.");
            }
        }

        size_t size() const
        {
            return internal::bridge::list(*m_realm, *m_obj, m_key).size();
        }
        size_t find(const managed<T>& a) {
            return internal::bridge::list(*m_realm, *m_obj, m_key).find(a.m_obj.get_key());
        }
        size_t find(const typename managed<T*>::ref_type& a) const {
            return internal::bridge::list(*m_realm, *m_obj, m_key).find(a->m_obj.get_key());
        }
        typename managed<T*>::ref_type operator[](size_t idx) const {
            auto list = realm::internal::bridge::list(*m_realm, *m_obj, m_key);
            return typename managed<T*>::ref_type(managed<T>(realm::internal::bridge::get<realm::internal::bridge::obj>(list, idx), *m_realm));
        }

        realm::notification_token observe(std::function<void(realm::collection_change)>&& fn) {
            auto list = std::make_shared<realm::internal::bridge::list>(*m_realm, *m_obj, m_key);
            realm::notification_token token = list->add_notification_callback(
                    std::make_shared<realm::collection_callback_wrapper>(
                            std::move(fn),
                            false));
            token.m_realm = *m_realm;
            token.m_list = list;
            return token;
        }

        results<T> where(const std::string &query, const std::vector<realm::mixed> &arguments) {
            std::vector<internal::bridge::mixed> mixed_args;
            for(auto& a : arguments)
                mixed_args.push_back(serialize(a));
            return results<T>(internal::bridge::results(*m_realm, m_obj->get_target_table(m_key).query(query, std::move(mixed_args))));
        }

        results<T> where(std::function<rbool(managed<T> &)> &&fn) {
            static_assert(sizeof(managed<T>), "Must declare schema for T");
            auto schema = m_realm->schema().find(managed<T>::schema.name);
            auto group = m_realm->read_group();
            auto table_ref = group.get_table(schema.table_key());
            rbool query = rbool(internal::bridge::query(table_ref));
            auto query_object = managed<T>::prepare_for_query(*m_realm, &query);
            auto full_query = fn(query_object).q;
            return results<T>(internal::bridge::results(*m_realm, full_query));
        }

        results<T> sort(const std::string &key_path, bool ascending) {
            auto schema = m_realm->schema().find(managed<T>::schema.name);
            auto table_ref = m_obj->get_target_table(m_key);
            return results<T>(internal::bridge::results(*m_realm, table_ref)).sort({{key_path, ascending}});
        }

        results<T> sort(const std::vector<internal::bridge::sort_descriptor> &sort_descriptors) {
            auto schema = m_realm->schema().find(managed<T>::schema.name);
            auto table_ref = m_obj->get_target_table(m_key);
            return results<T>(internal::bridge::results(*m_realm, table_ref)).sort(sort_descriptors);
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

#endif//CPPREALM_MANAGED_LIST_HPP
