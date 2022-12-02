#ifndef CPP_REALM_BRIDGE_SCHEMA_HPP
#define CPP_REALM_BRIDGE_SCHEMA_HPP

#include <vector>

namespace realm {
    class Schema;
}
namespace realm::internal::bridge {
    struct object_schema;

    struct schema {
        schema(const std::vector <object_schema> &);
        schema(const Schema&);
        operator Schema() const;
        object_schema find(const std::string &name);
    private:
        unsigned char m_schema[24];
    };
}

#endif //CPP_REALM_BRIDGE_SCHEMA_HPP
