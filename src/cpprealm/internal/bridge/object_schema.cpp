#include <cpprealm/internal/bridge/object_schema.hpp>
#include <cpprealm/internal/bridge/property.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/object-store/object_schema.hpp>
#include <realm/object-store/property.hpp>

namespace realm::internal::bridge {

    object_schema::object_schema() {
        m_schema = std::make_shared<ObjectSchema>();
    }
    object_schema::object_schema(const realm::ObjectSchema &v) {
        m_schema = std::make_shared<ObjectSchema>(v);
    }
    object_schema::object_schema(const std::string &name, const std::vector<property> &properties,
                                 const std::string &primary_key,
                                 realm::internal::bridge::object_schema::object_type type) {
        m_schema = std::make_shared<ObjectSchema>();
        m_schema->name = name;
        std::transform(properties.begin(),
                       properties.end(),
                       m_schema->persisted_properties.begin(),
                       [](const property& p) {
            return static_cast<Property>(p);
        });
        m_schema->primary_key = primary_key;
        m_schema->table_type = static_cast<ObjectSchema::ObjectType>(type);
    }
    uint32_t object_schema::table_key() {
        return m_schema->table_key.value;
    }

    void object_schema::set_object_type(realm::internal::bridge::object_schema::object_type o) {
        m_schema->table_type = static_cast<ObjectSchema::ObjectType>(o);
    }

    void object_schema::add_property(const realm::internal::bridge::property &v) {
        m_schema->persisted_properties.push_back(v);
    }

    void object_schema::set_name(const std::string &name) {
        m_schema->name = name;
    }
    property object_schema::property_for_name(const std::string &v) {
        return *m_schema->property_for_name(v);
    }
    void object_schema::set_primary_key(const std::string &primary_key) {
        m_schema->primary_key = primary_key;
    }
    object_schema::operator ObjectSchema() const {
        return *m_schema;
    }
}
