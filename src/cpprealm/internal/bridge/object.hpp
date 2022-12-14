#ifndef CPP_REALM_BRIDGE_OBJECT_HPP
#define CPP_REALM_BRIDGE_OBJECT_HPP

#include <functional>
#include <any>
#include <unordered_map>

namespace realm {
    class Object;
    class IndexSet;
    class CollectionChangeCallback;
}
namespace realm::internal::bridge {
    struct realm;
    struct obj;
    struct object_schema;
    struct notification_token;

    struct index_set {
        index_set();
        index_set(const IndexSet&); //NOLINT(google-explicit-constructor)
        [[nodiscard]] bool empty() const;

    private:
        unsigned char m_idx_set[24]{};
    };
    struct collection_change_set {
        [[nodiscard]] index_set deletions() const;
        [[nodiscard]] index_set modifications() const;
        [[nodiscard]] std::unordered_map<int64_t, index_set> columns() const;
        [[nodiscard]] bool empty() const;
    private:
        unsigned char m_change_set[168]{};
    };
    struct collection_change_callback {
        operator CollectionChangeCallback() const;
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

        notification_token add_notification_callback(collection_change_callback&&);

        [[nodiscard]] object_schema get_object_schema() const;
    private:
        unsigned char m_object[104]{};
    };
}

#endif //CPP_REALM_BRIDGE_OBJECT_HPP
