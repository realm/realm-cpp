#include <cpprealm/internal/bridge/object_schema.hpp>
#include <cpprealm/internal/bridge/property.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/object-store/object_schema.hpp>
#include <realm/object-store/property.hpp>

namespace realm::internal::bridge {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
    static_assert(LayoutCheck<storage::ObjectSchema, ObjectSchema>{});
#elif __i386__
    static_assert(SizeCheck<68, sizeof(ObjectSchema)>{});
    static_assert(SizeCheck<4, alignof(ObjectSchema)>{});
#elif __x86_64__
    #if defined(__clang__)
    static_assert(SizeCheck<128, sizeof(ObjectSchema)>{});
    #elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<152, sizeof(ObjectSchema)>{});
    #endif
    static_assert(SizeCheck<8, alignof(ObjectSchema)>{});
#elif __arm__
    static_assert(SizeCheck<68, sizeof(ObjectSchema)>{});
    static_assert(SizeCheck<4, alignof(ObjectSchema)>{});
#elif __aarch64__
#if defined(__clang__)
    static_assert(SizeCheck<128, sizeof(ObjectSchema)>{});
#elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<152, sizeof(ObjectSchema)>{});
#endif
    static_assert(SizeCheck<8, alignof(ObjectSchema)>{});
#elif _WIN32
    #if _DEBUG
    static_assert(SizeCheck<192, sizeof(ObjectSchema)>{});
    #else
    static_assert(SizeCheck<152, sizeof(ObjectSchema)>{});
    #endif
    static_assert(SizeCheck<8, alignof(ObjectSchema)>{});
#endif

    object_schema::object_schema() {
        new (&m_schema) ObjectSchema();
    }

    object_schema::object_schema(const object_schema& other) {
        new (&m_schema) ObjectSchema(*reinterpret_cast<const ObjectSchema*>(&other.m_schema));
    }

    object_schema& object_schema::operator=(const object_schema& other) {
        if (this != &other) {
            *reinterpret_cast<ObjectSchema*>(&m_schema) = *reinterpret_cast<const ObjectSchema*>(&other.m_schema);
        }
        return *this;
    }

    object_schema::object_schema(object_schema&& other) {
        new (&m_schema) ObjectSchema(std::move(*reinterpret_cast<ObjectSchema*>(&other.m_schema)));
    }

    object_schema& object_schema::operator=(object_schema&& other) {
        if (this != &other) {
            *reinterpret_cast<ObjectSchema*>(&m_schema) = std::move(*reinterpret_cast<ObjectSchema*>(&other.m_schema));
        }
        return *this;
    }

    object_schema::~object_schema() {
        reinterpret_cast<ObjectSchema*>(&m_schema)->~ObjectSchema();
    }





    object_schema::object_schema(const realm::ObjectSchema &v) {
        new (&m_schema) ObjectSchema(v);
    }
    object_schema::object_schema(const std::string &name, const std::vector<property> &properties,
                                 const std::string &primary_key,
                                 realm::internal::bridge::object_schema::object_type type) {
        new (&m_schema) ObjectSchema();
        reinterpret_cast<ObjectSchema*>(&m_schema)->name = name;
        std::transform(properties.begin(),
                       properties.end(),
                       reinterpret_cast<ObjectSchema*>(&m_schema)->persisted_properties.begin(),
                       [](const property& p) {
            return static_cast<Property>(p);
        });
        reinterpret_cast<ObjectSchema*>(&m_schema)->primary_key = primary_key;
        reinterpret_cast<ObjectSchema*>(&m_schema)->table_type = static_cast<ObjectSchema::ObjectType>(type);
    }
    uint32_t object_schema::table_key() {
        return reinterpret_cast<ObjectSchema*>(&m_schema)->table_key.value;
    }

    void object_schema::set_object_type(realm::internal::bridge::object_schema::object_type o) {
        reinterpret_cast<ObjectSchema*>(&m_schema)->table_type = static_cast<ObjectSchema::ObjectType>(o);
    }

    void object_schema::add_property(const realm::internal::bridge::property &v) {
        reinterpret_cast<ObjectSchema*>(&m_schema)->persisted_properties.push_back(v);
    }

    void object_schema::set_name(const std::string &name) {
        reinterpret_cast<ObjectSchema*>(&m_schema)->name = name;
    }
    property object_schema::property_for_name(const std::string &v) {
        return *reinterpret_cast<ObjectSchema*>(&m_schema)->property_for_name(v);
    }
    void object_schema::set_primary_key(const std::string &primary_key) {
        reinterpret_cast<ObjectSchema*>(&m_schema)->primary_key = primary_key;
    }
    object_schema::operator ObjectSchema() const {
        return *reinterpret_cast<const ObjectSchema*>(&m_schema);
    }

    bool object_schema::operator==(const object_schema& rhs) {
        return reinterpret_cast<const ObjectSchema*>(&m_schema)->name == reinterpret_cast<const ObjectSchema*>(&rhs.m_schema)->name;
    }
}
