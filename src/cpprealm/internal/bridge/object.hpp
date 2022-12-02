#ifndef CPP_REALM_BRIDGE_OBJECT_HPP
#define CPP_REALM_BRIDGE_OBJECT_HPP

#include <functional>
#include <any>
#include <unordered_map>

namespace realm::internal::bridge {
    struct realm;
    struct obj;
    struct object_schema;
    struct notification_token;

    struct index_set {
        [[nodiscard]] bool empty() const;
    private:
        unsigned char m_idx_set[24]{};
    };
    struct collection_change_set {
        index_set deletions() const;
        index_set modifications() const;
        std::unordered_map<int64_t, index_set> columns() const;
        bool empty() const;
    private:
        unsigned char m_change_set[168]{};
    };
    struct collection_change_callback {
        virtual void before(collection_change_set const& c) = 0;
        virtual void after(collection_change_set const& c) = 0;
    };
    struct object {
        object() = default;

        ~object();

        object(const realm &realm, const obj &obj);

        [[nodiscard]] obj obj() const;

        realm get_realm() const;

        bool is_valid() const;

        notification_token add_notification_callback(collection_change_callback&&);

        template<typename ValueType>
        void set_column_value(const std::string &property_name, const ValueType &value);

        object_schema get_object_schema() const;

    private:
        unsigned char m_object[104];
    };
}

#endif //CPP_REALM_BRIDGE_OBJECT_HPP
