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
#include <cpprealm/notifications.hpp>
#include <cpprealm/schema.hpp>
#include <cpprealm/rbool.hpp>

namespace realm {
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

        size_t size() {
            return m_parent.size();
        }

        virtual ~results_common_base() = default;
        Derived where(const std::string &query, const std::vector<realm::mixed>& arguments) {
            std::vector<internal::bridge::mixed> mixed_args;
            for(auto& a : arguments)
                mixed_args.push_back(serialize(a));
            return Derived(internal::bridge::results(m_parent.get_realm(),
                                                     m_parent.get_table().query(query, std::move(mixed_args))));
        }

        Derived where(std::function<rbool(managed<T>&)>&& fn) {
            static_assert(sizeof(managed<T>), "Must declare schema for T");
            auto realm = m_parent.get_realm();
            auto schema = realm.schema().find(managed<T>::schema.name);
            auto group = realm.read_group();
            auto table_ref = group.get_table(schema.table_key());
            rbool query = rbool(internal::bridge::query(table_ref));
            auto query_object = managed<T>::prepare_for_query(realm, &query);
            auto full_query = fn(query_object).q;
            return Derived(internal::bridge::results(m_parent.get_realm(), full_query));
        }

        struct results_callback_wrapper : internal::bridge::collection_change_callback {
            explicit results_callback_wrapper(std::function<void(results_change)>&& fn,
                                              Derived *c,
                                              bool ignore_initial_notification = true)
                : m_handler(std::move(fn)),
                  collection(c),
                  m_ignore_changes_in_initial_notification(ignore_initial_notification) {}

            void before(const realm::internal::bridge::collection_change_set&) override {}

            void after(internal::bridge::collection_change_set const &changes) final {
                if (m_ignore_changes_in_initial_notification) {
                    m_ignore_changes_in_initial_notification = false;
                    m_handler({collection, {}, {}, {}});
                } else if (changes.empty()) {
                    m_handler({collection, {}, {}, {}});
                } else if (!changes.collection_root_was_deleted() || !changes.deletions().empty()) {
                    m_handler({
                            collection,
                            to_vector(changes.deletions()),
                            to_vector(changes.insertions()),
                            to_vector(changes.modifications()),
                    });
                }
            }

            Derived *collection;

        private:
            std::function<void(results_change)> m_handler;
            bool m_ignore_changes_in_initial_notification;
            std::vector<uint64_t> to_vector(const internal::bridge::index_set &index_set) {
                auto vector = std::vector<uint64_t>();
                for (auto index: index_set.as_indexes()) {
                    vector.push_back(index);
                }
                return vector;
            };
        };

        realm::notification_token observe(std::function<void(results_change)>&& handler) {
            auto r = std::make_shared<internal::bridge::results>(m_parent.get_realm(), m_parent.get_realm().table_for_object_type(managed<T>::schema.name));
            realm::notification_token token = r->add_notification_callback(std::make_shared<results_callback_wrapper>(std::move(handler), static_cast<Derived*>(this)));
            token.m_realm = r->get_realm();
            token.m_results = r;
            return token;
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

        Derived sort(const std::string& key_path, bool ascending) {
            return Derived(m_parent.sort({{key_path, ascending}}));
        }

        Derived sort(const std::vector<sort_descriptor>& sort_descriptors) {
            return Derived(m_parent.sort(sort_descriptors));
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

        class iterator {
        public:
            using difference_type = size_t;
            using value_type = T;
            using iterator_category = std::input_iterator_tag;

            bool operator!=(const iterator &other) const {
                return !(*this == other);
            }

            bool operator==(const iterator &other) const {
                return (m_parent == other.m_parent) && (m_idx == other.m_idx);
            }

            value_type operator*() noexcept {
                return m_parent->operator[](m_idx);
            }

            iterator &operator++() {
                m_idx++;
                return *this;
            }

            iterator operator++(int i) {
                m_idx += i;
                return *this;
            }

            explicit iterator(size_t idx, Derived *parent)
                : m_idx(idx), m_parent(parent) {
            }
        private:
            size_t m_idx;
            Derived *m_parent;
        };

        iterator begin() {
            return iterator(0, static_cast<Derived*>(this));
        }

        iterator end() {
            return iterator(this->m_parent.size(), static_cast<Derived*>(this));
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

        // TODO: The current impl of realm::mixed does not allow managed object types,
        // to be accessed from the iterator as it would be required to be wrapped in a
        // managed<> template. As these templates require on a col & obj key as they act as managed
        // properties on an object this use case is broken. Ideally we should replace realm::mixed to
        // not be a std::variant, but rather be a type-safe union we define in the SDK so that
        // realm::mixed could have a managed context itself.
        class iterator {
        public:
            using difference_type = size_t;
            using value_type = T;
            using iterator_category = std::input_iterator_tag;

            bool operator!=(const iterator &other) const {
                return !(*this == other);
            }

            bool operator==(const iterator &other) const {
                return (m_parent == other.m_parent) && (m_idx == other.m_idx);
            }

            value_type operator*() noexcept {
                return m_parent->operator[](m_idx);
            }

            iterator &operator++() {
                m_idx++;
                return *this;
            }

            iterator operator++(int i) {
                m_idx += i;
                return *this;
            }

            explicit iterator(size_t idx, Derived *parent)
                : m_idx(idx), m_parent(parent) {
            }
        private:
            size_t m_idx;
            Derived *m_parent;
        };

        iterator begin() {
            return iterator(0, static_cast<Derived*>(this));
        }

        iterator end() {
            return iterator(this->m_parent.size(), static_cast<Derived*>(this));
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

        class iterator {
        public:
            using difference_type = size_t;
            using value_type = T;
            using iterator_category = std::input_iterator_tag;

            bool operator!=(const iterator &other) const {
                return !(*this == other);
            }

            bool operator==(const iterator &other) const {
                return (m_parent == other.m_parent) && (m_idx == other.m_idx);
            }

            value_type operator*() noexcept {
                return m_parent->operator[](m_idx);
            }

            iterator &operator++() {
                m_idx++;
                return *this;
            }

            iterator operator++(int i) {
                m_idx += i;
                return *this;
            }

            explicit iterator(size_t idx, Derived *parent)
                : m_idx(idx), m_parent(parent) {
            }
        private:
            size_t m_idx;
            Derived *m_parent;
        };

        iterator begin() {
            return iterator(0, static_cast<Derived*>(this));
        }

        iterator end() {
            return iterator(this->m_parent.size(), static_cast<Derived*>(this));
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

        class iterator {
        public:
            using difference_type = size_t;
            using value_type = managed<T, void>;
            using iterator_category = std::input_iterator_tag;

            bool operator!=(const iterator &other) const {
                return !(*this == other);
            }

            bool operator==(const iterator &other) const {
                return (m_parent == other.m_parent) && (m_idx == other.m_idx);
            }

            value_type operator*() noexcept {
                internal::bridge::obj obj = internal::bridge::get<internal::bridge::obj>(m_parent->m_parent, m_idx);
                return managed<T, void>(std::move(obj), this->m_parent->m_parent.get_realm());
            }

            iterator &operator++() {
                m_idx++;
                return *this;
            }

            iterator operator++(int i) {
                m_idx += i;
                return *this;
            }

            explicit iterator(size_t idx, Derived *parent)
                : m_idx(idx), m_parent(parent) {
            }
        private:
            size_t m_idx;
            Derived *m_parent;
        };

        iterator begin() {
            return iterator(0, static_cast<Derived*>(this));
        }

        iterator end() {
            return iterator(this->m_parent.size(), static_cast<Derived*>(this));
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
