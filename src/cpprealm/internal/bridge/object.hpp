#ifndef CPP_REALM_BRIDGE_OBJECT_HPP
#define CPP_REALM_BRIDGE_OBJECT_HPP

#include <any>
#include <functional>
#include <memory>
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
    struct obj_link;
    struct object_schema;
    struct list;
    struct col_key;
    struct dictionary;

    struct notification_token {
        notification_token();
        notification_token(NotificationToken&&);
        operator NotificationToken() const;
        void unregister();
    private:
#ifdef __i386__
        std::aligned_storage<16, 4>::type m_token[1];
#elif __x86_64__
        std::aligned_storage<24, 8>::type m_token[1];
#elif __arm__
        std::aligned_storage<16, 8>::type m_token[1];
#elif __aarch64__
        std::aligned_storage<24, 8>::type m_token[1];
#endif
    };

    using index_set = std::vector<uint64_t>;
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
        std::unique_ptr<CollectionChangeSet> m_change_set;
    };
    struct collection_change_callback {
        virtual void before(collection_change_set const& c) = 0;
        virtual void after(collection_change_set const& c) = 0;
    };

    struct object {
        object(); //NOLINT(google-explicit-constructor)
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
#ifdef __i386__
        std::aligned_storage<64, 4>::type m_object[1];
#elif __x86_64__
        std::aligned_storage<104, 8>::type m_object[1];
#elif __arm__
        std::aligned_storage<80, 8>::type m_object[1];
#elif __aarch64__
        std::aligned_storage<104, 8>::type m_object[1];
#endif
    };
}

#endif //CPP_REALM_BRIDGE_OBJECT_HPP
