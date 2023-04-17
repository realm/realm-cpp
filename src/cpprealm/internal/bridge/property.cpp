#include <cpprealm/internal/bridge/col_key.hpp>
#include <cpprealm/internal/bridge/property.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/object-store/property.hpp>

namespace realm::internal::bridge {
    property::property(const realm::Property &v) {
        m_property = std::make_unique<Property>(v);
    }
    property::property(const std::string &name,
                       realm::internal::bridge::property::type type,
                       const std::string &object_name) {
        m_property = std::make_shared<Property>(Property(name, static_cast<PropertyType>(type), object_name));
    }
    property::property(const std::string &name, realm::internal::bridge::property::type type, bool is_primary_key) {
        m_property = std::make_shared<Property>(Property(name, static_cast<PropertyType>(type), is_primary_key));
    }
    void property::set_object_link(const std::string & v) {
        m_property->object_type = v;
    }
    col_key property::column_key() const {
        return m_property->column_key;
    }

    property::operator Property() const {
        return *m_property;
    }

    void property::set_type(realm::internal::bridge::property::type t) {
        m_property->type = static_cast<PropertyType>(t);
    }
}
