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

#ifndef CPPREALM_RESULTS_HPP
#define CPPREALM_RESULTS_HPP

#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/query.hpp>
#include <cpprealm/internal/bridge/results.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/macros.hpp>
#include <cpprealm/schema.hpp>

namespace realm {
    class rbool;
    struct mutable_sync_subscription_set;
}

namespace realm {

    using sort_descriptor = internal::bridge::sort_descriptor;

    template<typename>
    struct results;
    template<typename T, typename Derived, typename ShouldEnable = void>
    struct results_base;
    template<typename T, typename Derived>
    struct results_common_base;


    template<typename T>
    struct query : public T {
    private:
        template<typename V>
        void set_managed(V &prop, const internal::bridge::col_key &column_key, internal::bridge::realm& r) {
            new(&prop.m_key) internal::bridge::col_key(column_key);
            prop.m_realm = &r;
        }

        template<size_t N, typename P>
        constexpr auto
        prepare_for_query(internal::bridge::query &query, internal::bridge::object_schema &schema,
                          internal::bridge::realm& r, P property) {
            if constexpr (N + 1 == std::tuple_size_v<decltype(T::managed_pointers() )>) {
                (this->*property).prepare_for_query(query);
                set_managed((this->*property), schema.property_for_name(T::schema.names[N]).column_key(), r);
                return;
            } else {
                (this->*property).prepare_for_query(query);
                set_managed((this->*property), schema.property_for_name(T::schema.names[N]).column_key(), r);
                return prepare_for_query<N + 1>(query, schema, r, std::get<N + 1>(T::managed_pointers() ));
            }
        }

        query(internal::bridge::query &query, internal::bridge::object_schema &&schema, internal::bridge::realm& r) {
            prepare_for_query<0>(query, schema, r, std::get<0>(T::managed_pointers()));
        }
        template<typename>
        friend struct ::realm::results;
        template<typename, typename, typename>
        friend struct ::realm::results_base;
        template<typename, typename>
        friend struct ::realm::results_common_base;
        template<typename, typename>
        friend struct ::realm::managed;
        friend struct ::realm::mutable_sync_subscription_set;
    };

    template<typename T, typename Derived>
    struct results_common_base {
        explicit results_common_base(internal::bridge::results &&parent)
            : m_parent(parent) {
        }
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
            using value_type = managed<T, void>;
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
                internal::bridge::obj obj = internal::bridge::get<internal::bridge::obj>(m_parent->m_parent, m_idx);
                value = managed<T, void>(std::move(obj), this->m_parent->m_parent.get_realm());
                return value;
            }

            pointer operator->() const noexcept {
                auto obj = internal::bridge::get<internal::bridge::obj>(*m_parent, m_idx);
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
            iterator(size_t idx, Derived *parent)
                : m_idx(idx), m_parent(parent) {
            }

            size_t m_idx;
            Derived *m_parent;
            managed<T, void> value;

            template<auto>
            friend struct linking_objects;
            template<typename, typename>
            friend struct results_common_base;
        };
        virtual ~results_common_base() = default;
        iterator begin() {
            return iterator(0, static_cast<Derived*>(this));
        }

        iterator end() {
            return iterator(m_parent.size(), static_cast<Derived*>(this));
        }

        size_t size() {
            return m_parent.size();
        }

        Derived& where(const std::string &query, const std::vector<realm::mixed>& arguments) {
            std::vector<internal::bridge::mixed> mixed_args;
            for(auto& a : arguments)
                mixed_args.push_back(serialize(a));
            m_parent = internal::bridge::results(m_parent.get_realm(),
                                                 m_parent.get_table().query(query, std::move(mixed_args)));
            return static_cast<Derived&>(*this);
        }

        Derived& where(std::function<rbool(managed<T>&)>&& fn) {
            static_assert(sizeof(managed<T>), "Must declare schema for T");
            auto realm = m_parent.get_realm();
            auto schema = realm.schema().find(managed<T>::schema.name);
            auto group = realm.read_group();
            auto table_ref = group.get_table(schema.table_key());
            auto builder = internal::bridge::query(table_ref);
            auto q = realm::query<managed<T>>(builder, std::move(schema), realm);
            auto full_query = fn(q).q;
            m_parent = internal::bridge::results(m_parent.get_realm(), full_query);
            return static_cast<Derived&>(*this);
        }

        struct results_callback_wrapper : internal::bridge::collection_change_callback {
            std::function<void(results_change)> handler;
            Derived &collection;
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
                    handler({
                            &collection,
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

        internal::bridge::notification_token observe(std::function<void(results_change)> &&handler) {
            return m_parent.add_notification_callback(
                    std::make_shared<results_callback_wrapper>(std::move(handler), static_cast<Derived&>(*this)));
        }

        Derived freeze() {
            auto frozen_realm = m_parent.get_realm().freeze();
            return Derived(internal::bridge::results(frozen_realm, frozen_realm.table_for_object_type(managed<T>::schema.name)));
        }

        Derived thaw() {
            auto thawed_realm = m_parent.get_realm().thaw();
            return Derived(internal::bridge::results(thawed_realm, thawed_realm.table_for_object_type(managed<T>::schema.name)));
        }

        bool is_frozen() {
            return m_parent.get_realm().is_frozen();
        }

        Derived& sort(const std::string& key_path, bool ascending) {
            m_parent = m_parent.sort({{key_path, ascending}});
            return static_cast<Derived&>(*this);
        }

        Derived& sort(const std::vector<sort_descriptor>& sort_descriptors) {
            m_parent = m_parent.sort(sort_descriptors);
            return static_cast<Derived&>(*this);
        }

    protected:
        internal::bridge::results m_parent;
        template <auto> friend struct linking_objects;
    };

    template<typename T>
    using results_is_primitive = std::enable_if_t<!managed<T>::is_object && !std::is_enum_v<T> && !internal::type_info::is_variant_t<T>::value>;
    template<typename T>
    using results_is_enum = std::enable_if_t<!managed<T>::is_object && std::is_enum_v<T> && !internal::type_info::is_variant_t<T>::value>;
    template<typename T>
    using results_is_mixed = std::enable_if_t<!managed<T>::is_object && !std::is_enum_v<T> && internal::type_info::is_variant_t<T>::value>;

    template<typename T, typename Derived>
    struct results_base<T, Derived, results_is_primitive<T>> : public results_common_base<T, Derived> {
        explicit results_base(internal::bridge::results &&parent)
            : results_common_base<T, Derived>(std::move(parent)) {
        }

        T operator[](size_t index) {
            if (index >= this->m_parent.size())
                throw std::out_of_range("Index out of range.");
            return internal::bridge::get<T>(this->m_parent, index);
        }
    };

    template<typename T, typename Derived>
    struct results_base<T, Derived, results_is_mixed<T>> : public results_common_base<T, Derived> {
        explicit results_base(internal::bridge::results &&parent)
            : results_common_base<T, Derived>(std::move(parent)) {
        }

        T operator[](size_t index) {
            if (index >= this->m_parent.size())
                throw std::out_of_range("Index out of range.");
            return deserialize<T>(internal::bridge::get<internal::bridge::mixed>(this->m_parent, index));
        }
    };

    template<typename T, typename Derived>
    struct results_base<T, Derived, results_is_enum<T>> : public results_common_base<T, Derived> {
        explicit results_base(internal::bridge::results &&parent)
            : results_common_base<T, Derived>(std::move(parent)) {
        }

        T operator[](size_t index) {
            if (index >= this->m_parent.size())
                throw std::out_of_range("Index out of range.");
            return static_cast<T>(internal::bridge::get<int64_t>(this->m_parent, index));
        }
    };

    template<typename T, typename Derived>
    struct results_base<T, Derived, std::enable_if_t<managed<T>::is_object>> : public results_common_base<T, Derived> {
        explicit results_base(internal::bridge::results &&parent)
            : results_common_base<T, Derived>(std::move(parent)) {
        }

        managed<T, void> operator[](size_t index) {
            if (index >= this->m_parent.size())
                throw std::out_of_range("Index out of range.");
            return managed<T, void>(internal::bridge::get<internal::bridge::obj>(this->m_parent, index), this->m_parent.get_realm());
        }
    };

    template<typename T>
    struct results : public results_base<T, results<T>> {
        using value_type = T;
        explicit results(internal::bridge::results &&parent)
            : results_base<T, results<T>>(std::move(parent)) {
        }
    };

    template <auto ptr>
    struct linking_objects {
        static inline auto Ptr = ptr;
        using Class = typename internal::ptr_type_extractor<ptr>::class_type;

        static_assert(sizeof(managed<typename internal::ptr_type_extractor<ptr>::class_type>), "Type is not managed by the Realm");
    };

    template <auto ptr> struct managed<linking_objects<ptr>> : managed_base {
        using iterator = typename results<typename internal::ptr_type_extractor<ptr>::class_type>::iterator;
        using Class = typename internal::ptr_type_extractor<ptr>::class_type;

        linking_objects<ptr> detach() const {
            return {};
        }

        iterator begin() {
            return iterator(0, get_results());
        }

        iterator end() {
            auto r = get_results();
            return iterator(r.size(), r);
        }

        size_t size() {
            return get_results().size();
        }
        managed<Class> operator[](size_t idx) {
            return get_results()[idx];
        }

    private:
        results<Class> get_results() {
            auto table = m_obj->get_table();
            if (!table.is_valid(m_obj->get_key())) {
                throw std::logic_error("Object has been deleted or invalidated.");
            }

            internal::bridge::obj* obj = m_obj;
            auto schema = m_realm->schema().find(managed<Class>::schema.name);
            auto linking_property = schema.property_for_name(managed<Class>::schema.template name_for_property<ptr>());
            if (!linking_property.column_key()) {
                throw std::logic_error("Invalid column key for origin property.");
            }

            internal::bridge::results results(*m_realm, obj->get_backlink_view(m_realm->get_table(schema.table_key()), linking_property.column_key()));
            return ::realm::results<Class>(std::move(results));
        }
    };
}


#endif //CPPREALM_RESULTS_HPP
