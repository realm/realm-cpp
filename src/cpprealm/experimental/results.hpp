#ifndef CPPREALM_EXPERIMENTAL_RESULTS_HPP
#define CPPREALM_EXPERIMENTAL_RESULTS_HPP

#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/query.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/results.hpp>
#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/schema.hpp>

#include <cpprealm/persisted.hpp>

namespace realm {
    class rbool;
    struct mutable_sync_subscription_set;
}

namespace realm::experimental {

    template<typename>
    struct results;

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
        friend struct ::realm::experimental::results;
        friend struct ::realm::mutable_sync_subscription_set;
    };

    template<typename T>
    struct results {
        struct results_change {
            results<T> *collection;
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
            iterator(size_t idx, results<T> *parent)
                : m_idx(idx), m_parent(parent) {
            }

            size_t m_idx;
            results<T> *m_parent;
            managed<T, void> value;

            template<auto>
            friend struct linking_objects;
            template<typename>
            friend struct results;
        };
        virtual ~results() = default;
        iterator begin() {
            return iterator(0, this);
        }

        iterator end() {
            return iterator(m_parent.size(), this);
        }

        size_t size() {
            return m_parent.size();
        }

        results<T> &where(const std::string &query, std::vector<internal::bridge::mixed> arguments) {
            m_parent = internal::bridge::results(m_parent.get_realm(),
                                                 m_parent.get_table().query(query, std::move(arguments)));
            return dynamic_cast<results<T> &>(*this);
        }

        results<T> &where(std::function<rbool(experimental::managed<T>&)>&& fn) {
            static_assert(sizeof(managed<T>), "Must declare schema for T");
            auto realm = m_parent.get_realm();
            auto schema = realm.schema().find(experimental::managed<T>::schema.name);
            auto group = realm.read_group();
            auto table_ref = group.get_table(schema.table_key());
            auto builder = internal::bridge::query(table_ref);
            auto q = realm::experimental::query<experimental::managed<T>>(builder, std::move(schema), realm);
            auto full_query = fn(q).q;
            m_parent = internal::bridge::results(m_parent.get_realm(), full_query);
            return dynamic_cast<results &>(*this);
        }

        struct results_callback_wrapper : internal::bridge::collection_change_callback {
            std::function<void(results_change)> handler;
            results<T> &collection;
            bool ignoreChangesInInitialNotification = true;

            results_callback_wrapper(std::function<void(results_change)> handler,
                                     results<T> &collection)
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
                    std::make_shared<results_callback_wrapper>(std::move(handler), dynamic_cast<results<T> &>(*this)));
        }

        explicit results(internal::bridge::results &&parent)
            : m_parent(parent) {
        }

        managed<T, void> operator[](size_t index) {
            if (index >= this->m_parent.size())
                throw std::out_of_range("Index out of range.");
            return managed<T, void>(internal::bridge::get<internal::bridge::obj>(this->m_parent, index), this->m_parent.get_realm());
        }

    protected:
        internal::bridge::results m_parent;
        template <auto> friend struct linking_objects;
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

        linking_objects<ptr> value() const {
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
            return ::realm::experimental::results<Class>(std::move(results));
        }
    };
}


#endif //CPPREALM_EXPERIMENTAL_RESULTS_HPP