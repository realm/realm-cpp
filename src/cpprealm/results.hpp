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

#ifndef realm_results_hpp
#define realm_results_hpp

#include <any>

#include <cpprealm/internal/bridge/object_schema.hpp>
#include <cpprealm/internal/bridge/results.hpp>
#include <cpprealm/internal/bridge/schema.hpp>
#include <cpprealm/object.hpp>

class QObject;

namespace realm {

    template<typename T>
    struct query : public T {
    private:
        template<typename V>
        void set_managed(V &prop, const internal::bridge::col_key &column_key) {
            if constexpr (internal::type_info::is_primitive<typename V::Result>::value ||
                          std::is_same_v<std::vector<uint8_t>, typename V::Result>) {
                new(&prop.managed) internal::bridge::col_key(column_key);
            }
        }

        template<size_t N, typename P>
        constexpr auto
        prepare_for_query(internal::bridge::query &query, internal::bridge::object_schema &schema, P &property) {
            if constexpr (N + 1 == std::tuple_size_v<decltype(T::schema.properties)>) {
                (this->*property.ptr).prepare_for_query(query);
                set_managed((this->*property.ptr), schema.property_for_name(T::schema.names[N]).column_key());
                return;
            } else {
                (this->*property.ptr).prepare_for_query(query);
                set_managed((this->*property.ptr), schema.property_for_name(T::schema.names[N]).column_key());
                return prepare_for_query<N + 1>(query, schema, std::get<N + 1>(T::schema.properties));
            }
        }

        query(internal::bridge::query &query, internal::bridge::object_schema &&schema) {
            prepare_for_query<0>(query, schema, std::get<0>(T::schema.properties));
        }

        template<typename, typename>
        friend struct results_base;
        friend struct MutableSyncSubscriptionSet;
    };

    template<typename T, typename>
    struct results;
    template<typename T, typename Derived>
    struct results_base {
        struct results_change {
            Derived *collection;
            std::vector<uint64_t> deletions;
            std::vector<uint64_t> insertions;
            std::vector<uint64_t> modifications;

            // This flag indicates whether the underlying object which is the source of this
            // collection was deleted. This applies to lists, dictionaries and sets.
            // This enables notifiers to report a change on empty collections that have been deleted.
            bool collection_root_was_deleted = false;

            [[nodiscard]] bool empty() const noexcept {
                return deletions.empty() && insertions.empty() && modifications.empty() &&
                       !collection_root_was_deleted;
            }
        };
        class iterator {
        public:
            using difference_type = size_t;
            using value_type = T;
            using pointer = std::unique_ptr<value_type>;
            using reference = value_type &;
            using iterator_category = std::input_iterator_tag;

            bool operator!=(const iterator &other) const {
                return !(*this == other);
            }

            bool operator==(const iterator &other) const {
                return (m_parent == other.m_parent) && (m_idx == other.m_idx);
            }

            reference operator*() noexcept {
                auto obj = m_parent->m_parent.template get<Obj>(m_idx);
                value = std::move(T::schema.create(std::move(obj), m_parent->m_parent.get_realm()));
                return value;
            }

            pointer operator->() const noexcept {
                auto obj = m_parent->m_parent.template get<Obj>(m_idx);
                return T::schema::create_unique(std::move(obj), m_parent->m_parent.get_realm());
            }

            iterator &operator++() {
                m_idx++;
                return *this;
            }

            iterator operator++(int i) {
                m_idx += i;
                return *this;
            }

        private:
            iterator(size_t idx, results<T, T> *parent)
                    : m_idx(idx), m_parent(parent) {
            }

            size_t m_idx;
            results<T, T> *m_parent;
            T value;

            template<typename>
            friend
            class results;
        };
        virtual ~results_base() = default;
        iterator begin() {
            return iterator(0, this);
        }

        iterator end() {
            return iterator(m_parent.size(), this);
        }

        size_t size() {
            return m_parent.size();
        }

        Derived &where(const std::string &query, std::vector<internal::bridge::mixed> arguments) {
            m_parent = internal::bridge::results(m_parent.get_realm(),
                                                 m_parent.get_table().query(query, std::move(arguments)));
            return dynamic_cast<Derived&>(*this);
        }

        Derived &where(std::function<rbool(T &)> fn) {
            auto builder = internal::bridge::query(m_parent.get_table());
            auto schema = m_parent.get_realm().schema().find(T::schema.name);
            auto q = query<T>(builder, std::move(schema));
            auto full_query = fn(q).q;
            m_parent = internal::bridge::results(m_parent.get_realm(), full_query);
            return dynamic_cast<Derived&>(*this);
        }

        struct results_callback_wrapper : internal::bridge::collection_change_callback {
            std::function<void(results_change)> handler;
            Derived& collection;
            bool ignoreChangesInInitialNotification = true;

            results_callback_wrapper(std::function<void(results_change)> handler,
                                     Derived &collection)
                    : handler(handler), collection(collection) {}

            void before(const realm::internal::bridge::collection_change_set &c) override {}

            void after(internal::bridge::collection_change_set const &changes) final {
                if (ignoreChangesInInitialNotification) {
                    ignoreChangesInInitialNotification = false;
                    handler({&collection, {}, {}, {}});
                } else if (changes.empty()) {
                    handler({&collection, {}, {}, {}});
                } else if (!changes.collection_root_was_deleted() || !changes.deletions().empty()) {
                    handler({&collection,
                             to_vector(changes.deletions()),
                             to_vector(changes.insertions()),
                             to_vector(changes.modifications()),
                            });
                }
            }

        private:
            std::vector<uint64_t> to_vector(const internal::bridge::index_set &index_set) {
                auto vector = std::vector<uint64_t>();
                for (auto index: index_set.as_indexes()) {
                    vector.push_back(index);
                }
                return vector;
            };
        };

        notification_token observe(std::function<void(results_change)> &&handler) {
            return m_parent.add_notification_callback(
                    std::make_shared<results_callback_wrapper>(std::move(handler), dynamic_cast<Derived&>(*this)));
        }
    protected:
        template<typename ...V>
        friend
        struct db;

        explicit results_base(internal::bridge::results &&parent)
                : m_parent(parent) {
        }

        internal::bridge::results m_parent;
    };

    template<typename T>
    struct results<T, std::enable_if_t<!std::is_base_of_v<QObject, T>>> : public results_base<T, results<T, void>> {
        using results_base<T, results<T, void>>::results_base;

        T operator[](size_t index) {
            if (index >= this->m_parent.size())
                throw std::out_of_range("Index out of range.");
            auto object = internal::bridge::object(this->m_parent.get_realm(),
                                                   internal::bridge::get<internal::bridge::obj>(this->m_parent, index));
            auto cls = T();
            cls.assign_accessors(object);
            return cls;
        }
    };

    template<typename T>
    struct results<T, std::enable_if_t<std::is_base_of_v<QObject, T>>> : public results_base<T, results<T, void>> {
        using results_base<T, results<T, void>>::results_base;

        std::unique_ptr<T> operator[](size_t index) {
            if (index >= this->m_parent.size())
                throw std::out_of_range("Index out of range.");
            auto object = internal::bridge::object(this->m_parent.get_realm(),
                                                   internal::bridge::get<internal::bridge::obj>(this->m_parent, index));
            auto cls = std::make_unique<T>();
            cls->assign_accessors(object);
            return cls;
        }
    };

    template <typename T>
    using results_change = typename results_base<T, results<T, void>>::results_change;
}

#endif /* realm_results_hpp */
