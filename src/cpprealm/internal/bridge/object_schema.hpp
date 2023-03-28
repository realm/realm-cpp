#ifndef CPP_REALM_BRIDGE_OBJECT_SCHEMA_HPP
#define CPP_REALM_BRIDGE_OBJECT_SCHEMA_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace realm {
    class ObjectSchema;
}
namespace realm::internal::bridge {
    struct property;

    struct object_schema {
        enum class object_type : uint8_t { TopLevel = 0, Embedded = 0x1, TopLevelAsymmetric = 0x2 };

        object_schema();

        object_schema(const object_schema& other) ;
        object_schema& operator=(const object_schema& other) ;
        object_schema(object_schema&& other);
        object_schema& operator=(object_schema&& other);
        ~object_schema();


        object_schema(const std::string& name,
                      const std::vector<property>& properties,
                      const std::string& primary_key,
                      object_type type);
        object_schema(const ObjectSchema&);
        operator ObjectSchema() const;
        uint32_t table_key();
        void add_property(const property&);

        void set_name(const std::string& name);
        void set_primary_key(const std::string& primary_key);
        void set_object_type(object_type);
        property property_for_name(const std::string&);
    private:
#ifdef __i386__
        std::aligned_storage<68, 4>::type m_schema[1];
#elif __x86_64__
    #if defined(__clang__)
        std::aligned_storage<128, 8>::type m_schema[1];
    #elif defined(__GNUC__) || defined(__GNUG__)
        std::aligned_storage<152, 8>::type m_schema[1];
    #endif
#elif __arm__
        std::aligned_storage<68, 4>::type m_schema[1];
#elif __aarch64__
#if defined(__clang__)
        std::aligned_storage<128, 8>::type m_schema[1];
#elif defined(__GNUC__) || defined(__GNUG__)
        std::aligned_storage<152, 8>::type m_schema[1];
#endif
#endif
    };
}

#endif //CPP_REALM_BRIDGE_OBJECT_SCHEMA_HPP
