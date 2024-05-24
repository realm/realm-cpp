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

#ifndef CPPREALM_BRIDGE_OBJECT_HPP
#define CPPREALM_BRIDGE_OBJECT_HPP

#include <any>
#include <functional>
#include <memory>
#include <unordered_map>

#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    class Object;
    class IndexSet;
    class CollectionChangeCallback;
    struct CollectionChangeSet;
    struct NotificationToken;
}
namespace realm::internal::bridge {
    struct realm;
    struct obj;
    struct obj_link;
    struct object_schema;
    struct list;
    struct col_key;
    struct dictionary;

    struct notification_token {
        notification_token();
        notification_token(const notification_token &other) = delete;
        notification_token &operator=(const notification_token &other) = delete;
        notification_token(notification_token &&other);
        notification_token &operator=(notification_token &&other);
        notification_token(NotificationToken&&);
        ~notification_token();
        operator NotificationToken() const;
        void unregister();
    private:
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::NotificationToken m_token[1];
#else
        std::shared_ptr<NotificationToken> m_token;
#endif
    };

    struct index_set {
        index_set(); //NOLINT(google-explicit-constructor)
        index_set(const index_set& other) ;
        index_set& operator=(const index_set& other);
        index_set(index_set&& other);
        index_set& operator=(index_set&& other);
        ~index_set();
        index_set(const IndexSet&); //NOLINT(google-explicit-constructor)
        [[nodiscard]] bool empty() const;
        struct index_iterable_adaptor;
        // An iterator over the individual indices in the set rather than the ranges
        class index_iterator {
        public:
            index_iterator() = default;
            index_iterator(const index_iterator& other) ;
            index_iterator& operator=(const index_iterator& other);
            index_iterator(index_iterator&& other);
            index_iterator& operator=(index_iterator&& other);
            ~index_iterator();
            size_t operator*() const noexcept;
            bool operator==(index_iterator const& it) const noexcept;
            bool operator!=(index_iterator const& it) const noexcept;

            index_iterator& operator++() noexcept;

            index_iterator operator++(int) noexcept;

        private:
            friend struct index_iterable_adaptor;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
            storage::IndexSet_IndexIterator m_iterator[1];
#else
            std::shared_ptr<std::any> m_iterator;
#endif
        };

        struct index_iterable_adaptor {
            index_iterable_adaptor() = default;
            index_iterable_adaptor(const index_iterable_adaptor& other) ;
            index_iterable_adaptor& operator=(const index_iterable_adaptor& other);
            index_iterable_adaptor(index_iterable_adaptor&& other);
            index_iterable_adaptor& operator=(index_iterable_adaptor&& other);
            ~index_iterable_adaptor();
            using const_iterator = index_iterator;

            const_iterator begin() const noexcept;
            const_iterator end() const noexcept;
        private:
            friend struct index_set;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
            storage::IndexSet_IndexIteratableAdaptor m_index_iterable_adaptor[1];
#else
            std::shared_ptr<std::any> m_index_iterable_adaptor;
#endif
        };
        index_iterable_adaptor as_indexes() const;
    private:
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::IndexSet m_idx_set[1];
#else
        std::shared_ptr<IndexSet> m_idx_set;
#endif
    };
    struct collection_change_set {
        collection_change_set(); //NOLINT(google-explicit-constructor)
        collection_change_set(const collection_change_set& other) ;
        collection_change_set& operator=(const collection_change_set& other);
        collection_change_set(collection_change_set&& other);
        collection_change_set& operator=(collection_change_set&& other);
        ~collection_change_set();
        collection_change_set(const CollectionChangeSet&);
        operator CollectionChangeSet() const;
        [[nodiscard]] index_set deletions() const;
        [[nodiscard]] index_set modifications() const;
        [[nodiscard]] index_set insertions() const;
        [[nodiscard]] std::unordered_map<int64_t, index_set> columns() const;
        [[nodiscard]] bool empty() const;
        [[nodiscard]] bool collection_root_was_deleted() const;
    private:
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::CollectionChangeSet m_change_set[1];
#else
        std::shared_ptr<CollectionChangeSet> m_change_set;
#endif
    };
    struct collection_change_callback {
        virtual ~collection_change_callback() {}
        virtual void before(collection_change_set const& c) = 0;
        virtual void after(collection_change_set const& c) = 0;
    };

    struct object {
        object(); //NOLINT(google-explicit-constructor)
        object(const object& other) ;
        object& operator=(const object& other);
        object(object&& other);
        object& operator=(object&& other);
        ~object();
        object(const Object&); //NOLINT(google-explicit-constructor)
        object(const realm &realm, const obj &obj); //NOLINT(google-explicit-constructor)
        object(const realm &realm, const obj_link& link);

        operator Object() const; //NOLINT(google-explicit-constructor)

        [[nodiscard]] obj get_obj() const;

        [[nodiscard]] realm get_realm() const;

        [[nodiscard]] bool is_valid() const;

        notification_token add_notification_callback(std::shared_ptr<collection_change_callback>&& cb);

        [[nodiscard]] object_schema get_object_schema() const;

        [[nodiscard]] list get_list(const col_key&) const;
        [[nodiscard]] dictionary get_dictionary(const col_key&) const;
    private:
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::Object m_object[1];
#else
        std::shared_ptr<Object> m_object;
#endif
    };
}

#endif //CPPREALM_BRIDGE_OBJECT_HPP
