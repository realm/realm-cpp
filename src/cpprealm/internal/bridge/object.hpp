#ifndef CPP_REALM_BRIDGE_OBJECT_HPP
#define CPP_REALM_BRIDGE_OBJECT_HPP

#include <functional>
#include <any>
#include <unordered_map>

namespace realm {
    class Object;
    class IndexSet;
    class CollectionChangeCallback;
    class CollectionChangeSet;
    class NotificationToken;
}
namespace realm::internal::bridge {
    struct realm;
    struct obj;
    struct object_schema;
    struct list;
    struct col_key;
    struct dictionary;

    struct notification_token {
        notification_token();
        notification_token(NotificationToken&&);
        operator NotificationToken() const;
    private:
        unsigned char m_token[24]{};
    };

    struct index_set {
        index_set();
        index_set(const IndexSet&); //NOLINT(google-explicit-constructor)
        [[nodiscard]] bool empty() const;
        struct index_iterable_adaptor;
        // An iterator over the individual indices in the set rather than the ranges
        class index_iterator {
        public:
            size_t operator*() const noexcept;
            bool operator==(index_iterator const& it) const noexcept;
            bool operator!=(index_iterator const& it) const noexcept;

            index_iterator& operator++() noexcept;

            index_iterator operator++(int) noexcept;

        private:
            friend struct index_iterable_adaptor;
            unsigned char m_iterator[32]{};
        };

        struct index_iterable_adaptor {
            using const_iterator = index_iterator;

            const_iterator begin() const noexcept;
            const_iterator end() const noexcept;
        private:
            friend struct index_set;
            unsigned char index_iterable_adaptor[8]{};
        };
        index_iterable_adaptor as_indexes() const;
    private:
        unsigned char m_idx_set[24]{};
    };
    struct collection_change_set {
        collection_change_set(const CollectionChangeSet&);
        operator CollectionChangeSet() const;
        [[nodiscard]] index_set deletions() const;
        [[nodiscard]] index_set modifications() const;
        [[nodiscard]] index_set insertions() const;
        [[nodiscard]] std::unordered_map<int64_t, index_set> columns() const;
        [[nodiscard]] bool empty() const;
        [[nodiscard]] bool collection_root_was_deleted() const;
    private:
        unsigned char m_change_set[168]{};
    };
    struct collection_change_callback {
//        operator CollectionChangeCallback() const;
        virtual void before(collection_change_set const& c) = 0;
        virtual void after(collection_change_set const& c) = 0;
    };

    struct object {
        object(); //NOLINT(google-explicit-constructor)
        object(const Object&); //NOLINT(google-explicit-constructor)
        object(const realm &realm, const obj &obj); //NOLINT(google-explicit-constructor)

        operator Object() const; //NOLINT(google-explicit-constructor)

        [[nodiscard]] obj obj() const;

        [[nodiscard]] realm get_realm() const;

        [[nodiscard]] bool is_valid() const;

        notification_token add_notification_callback(std::shared_ptr<collection_change_callback>&& cb);
        notification_token add_notification_callback(const std::shared_ptr<collection_change_callback>& cb);

        [[nodiscard]] object_schema get_object_schema() const;

        [[nodiscard]] list get_list(const col_key&) const;
        [[nodiscard]] dictionary get_dictionary(const col_key&) const;
        object freeze(internal::bridge::realm) const;
    private:
        unsigned char m_object[104]{};
    };
}

#endif //CPP_REALM_BRIDGE_OBJECT_HPP
