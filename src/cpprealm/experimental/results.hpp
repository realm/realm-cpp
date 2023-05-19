#ifndef REALM_RESULTS_HPP
#define REALM_RESULTS_HPP

#include <vector>
#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/query.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/results.hpp>
#include <cpprealm/experimental/macros.hpp>

//#include <cpprealm/persisted.hpp>

namespace realm::experimental {

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
                abort();
                //                auto obj = m_parent->m_parent.template get<Obj>(m_idx);
                //                value = std::move(T::schema.create(std::move(obj), m_parent->m_parent.get_realm()));
                //                return value;
            }

            pointer operator->() const noexcept {
                abort();
                //                auto obj = m_parent->m_parent.template get<Obj>(m_idx);
                //                return T::schema::create_unique(std::move(obj), m_parent->m_parent.get_realm());
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
            T value;

            template<typename>
            friend class results;
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

//        results<T> &where(std::function<rbool(T &)> fn) {
//            auto builder = internal::bridge::query(m_parent.get_table());
//            auto schema = m_parent.get_realm().schema().find(T::schema.name);
//            auto q = query<T>(builder, std::move(schema));
//            auto full_query = fn(q).q;
//            m_parent = internal::bridge::results(m_parent.get_realm(), full_query);
//            return dynamic_cast<results<T> &>(*this);
//        }

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
        template<typename... V>
        friend struct db;

        internal::bridge::results m_parent;
    };
}


#endif//REALM_RESULTS_HPP
