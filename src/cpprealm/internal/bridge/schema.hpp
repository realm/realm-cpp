#ifndef CPP_REALM_BRIDGE_SCHEMA_HPP
#define CPP_REALM_BRIDGE_SCHEMA_HPP

#include <string>
#include <vector>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    class Schema;
}
namespace realm::internal::bridge {
    struct object_schema;

    struct schema {
        schema();
        schema(const schema& other) ;
        schema& operator=(const schema& other) ;
        schema(schema&& other);
        schema& operator=(schema&& other);
        ~schema();
        schema(const std::vector<object_schema>&); //NOLINT(google-explicit-constructor)
        schema(const Schema&); //NOLINT(google-explicit-constructor)
        operator Schema() const; //NOLINT(google-explicit-constructor)
        object_schema find(const std::string &name);
    private:
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::Schema m_schema[1];
#else
        std::shared_ptr<Schema> m_schema;
#endif
    };
}

#endif //CPP_REALM_BRIDGE_SCHEMA_HPP
