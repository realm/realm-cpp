#include <cpprealm/internal/bridge/object_schema.hpp>
#include <cpprealm/internal/bridge/property.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/object-store/object_schema.hpp>
#include <realm/object-store/property.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<128, sizeof(ObjectSchema)>{});

    object_schema::object_schema() {
        new (&m_schema) ObjectSchema();
    }
    object_schema::object_schema(const realm::ObjectSchema &v) {
        new (&m_schema) ObjectSchema(v);
    }
    object_schema::object_schema(const std::string &name, const std::vector<property> &properties,
                                 const std::string &primary_key,
                                 realm::internal::bridge::object_schema::object_type type) {
        new (&m_schema) ObjectSchema();
        reinterpret_cast<ObjectSchema*>(m_schema)->name = name;
        std::transform(properties.begin(),
                       properties.end(),
                       reinterpret_cast<ObjectSchema*>(m_schema)->persisted_properties.begin(),
                       [](const property& p) {
            return static_cast<Property>(p);
        });
        reinterpret_cast<ObjectSchema*>(m_schema)->primary_key = primary_key;
        reinterpret_cast<ObjectSchema*>(m_schema)->table_type = static_cast<ObjectSchema::ObjectType>(type);
    }
    int64_t object_schema::table_key() {
        return reinterpret_cast<ObjectSchema*>(m_schema)->table_key.value;
    }

    void object_schema::set_object_type(realm::internal::bridge::object_schema::object_type o) {
        reinterpret_cast<ObjectSchema*>(m_schema)->table_type = static_cast<ObjectSchema::ObjectType>(o);
    }

    void object_schema::add_property(const realm::internal::bridge::property &v) {
        reinterpret_cast<ObjectSchema*>(m_schema)->persisted_properties.push_back(v);
    }

    object_schema::operator ObjectSchema() const {
        return *reinterpret_cast<const ObjectSchema*>(m_schema);
    }
}