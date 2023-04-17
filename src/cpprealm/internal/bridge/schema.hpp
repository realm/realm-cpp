#ifndef CPP_REALM_BRIDGE_SCHEMA_HPP
#define CPP_REALM_BRIDGE_SCHEMA_HPP

#include <string>
#include <vector>
#include <memory>

namespace realm {
    class Schema;
}
namespace realm::internal::bridge {
    struct object_schema;

    struct schema {
        schema(const std::vector<object_schema>&); //NOLINT(google-explicit-constructor)
        schema(const Schema&); //NOLINT(google-explicit-constructor)
        operator Schema() const; //NOLINT(google-explicit-constructor)
        object_schema find(const std::string &name);
    private:
        std::unique_ptr<Schema> m_schema;
    };
}

#endif //CPP_REALM_BRIDGE_SCHEMA_HPP
