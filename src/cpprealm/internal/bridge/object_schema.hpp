#ifndef CPP_REALM_BRIDGE_OBJECT_SCHEMA_HPP
#define CPP_REALM_BRIDGE_OBJECT_SCHEMA_HPP

#include <vector>

namespace realm {
    class ObjectSchema;
}
namespace realm::internal::bridge {
    struct property;

    struct object_schema {
        enum class object_type : uint8_t { TopLevel = 0, Embedded = 0x1, TopLevelAsymmetric = 0x2 };

        object_schema();
        object_schema(const std::string& name,
                      const std::vector<property>& properties,
                      const std::string& primary_key,
                      object_type type);
        object_schema(const ObjectSchema&);
        operator ObjectSchema() const;
        int64_t table_key();
        void add_property(const property&);

        void set_name(const std::string& name);
        void set_primary_key(const std::string& primary_key);
        void set_object_type(object_type);
        property property_for_name(const std::string&);
    private:
        unsigned char m_schema[128]{};
    };
}

#endif //CPP_REALM_BRIDGE_OBJECT_SCHEMA_HPP
