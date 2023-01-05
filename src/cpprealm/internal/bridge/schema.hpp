#ifndef CPP_REALM_BRIDGE_SCHEMA_HPP
#define CPP_REALM_BRIDGE_SCHEMA_HPP

#include <string>
#include <vector>

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
#ifdef __i386__
        std::aligned_storage<12, 4>::type m_schema[1];
#elif __x86_64__
        std::aligned_storage<24, 8>::type m_schema[1];
#elif __arm__
        std::aligned_storage<12, 4>::type m_schema[1];
#elif __aarch64__
        std::aligned_storage<24, 8>::type m_schema[1];
#else
        std::aligned_storage<24, 8>::type m_schema[1];
#endif
    };
}

#endif //CPP_REALM_BRIDGE_SCHEMA_HPP
