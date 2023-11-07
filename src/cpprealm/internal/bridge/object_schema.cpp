#include <cpprealm/internal/bridge/object_schema.hpp>
#include <cpprealm/internal/bridge/property.hpp>

#include <realm/object-store/object_schema.hpp>
#include <realm/object-store/property.hpp>

namespace realm::internal::bridge {
    object_schema::object_schema() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_schema) ObjectSchema();
#else
        m_schema = std::make_shared<ObjectSchema>();
#endif
    }

    object_schema::object_schema(const object_schema& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_schema) ObjectSchema(*reinterpret_cast<const ObjectSchema*>(&other.m_schema));
#else
        m_schema = other.m_schema;
#endif
    }

    object_schema& object_schema::operator=(const object_schema& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<ObjectSchema*>(&m_schema) = *reinterpret_cast<const ObjectSchema*>(&other.m_schema);
        }
#else
        m_schema = other.m_schema;
#endif
        return *this;
    }

    object_schema::object_schema(object_schema&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_schema) ObjectSchema(std::move(*reinterpret_cast<ObjectSchema*>(&other.m_schema)));
#else
        m_schema = std::move(other.m_schema);
#endif
    }

    object_schema& object_schema::operator=(object_schema&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<ObjectSchema*>(&m_schema) = std::move(*reinterpret_cast<ObjectSchema*>(&other.m_schema));
        }
#else
        m_schema = std::move(other.m_schema);
#endif
        return *this;
    }

    object_schema::~object_schema() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<ObjectSchema*>(&m_schema)->~ObjectSchema();
#endif
    }

    object_schema::object_schema(const realm::ObjectSchema &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_schema) ObjectSchema(v);
#else
        m_schema = std::make_shared<ObjectSchema>(v);
#endif
    }


    ObjectSchema*  object_schema::get_object_schema() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<ObjectSchema*>(&m_schema);
#else
        return m_schema.get();
#endif
    }

    const ObjectSchema*  object_schema::get_object_schema() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const ObjectSchema*>(&m_schema);
#else
        return m_schema.get();
#endif
    }


    object_schema::object_schema(const std::string &name, const std::vector<property> &properties,
                                 const std::string &primary_key,
                                 realm::internal::bridge::object_schema::object_type type) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_schema) ObjectSchema();
#else
        m_schema = std::make_shared<ObjectSchema>();
#endif

        get_object_schema()->name = name;
        std::transform(properties.begin(),
                       properties.end(),
                       get_object_schema()->persisted_properties.begin(),
                       [](const property& p) {
            return static_cast<Property>(p);
        });
        get_object_schema()->primary_key = primary_key;
        get_object_schema()->table_type = static_cast<ObjectSchema::ObjectType>(type);
    }
    uint32_t object_schema::table_key() {
        return get_object_schema()->table_key.value;
    }

    void object_schema::set_object_type(realm::internal::bridge::object_schema::object_type o) {
        get_object_schema()->table_type = static_cast<ObjectSchema::ObjectType>(o);
    }

    void object_schema::add_property(const realm::internal::bridge::property &v) {
        get_object_schema()->persisted_properties.push_back(v);
    }

    void object_schema::set_name(const std::string &name) {
        get_object_schema()->name = name;
    }
    property object_schema::property_for_name(const std::string &v) {
        return *get_object_schema()->property_for_name(v);
    }
    void object_schema::set_primary_key(const std::string &primary_key) {
        get_object_schema()->primary_key = primary_key;
    }
    std::string object_schema::get_name() const {
        return get_object_schema()->name;
    }
    object_schema::operator ObjectSchema() const {
        return *get_object_schema();
    }

    bool object_schema::operator==(const object_schema& rhs) {
        return get_object_schema()->name == rhs.get_object_schema()->name;
    }
}
