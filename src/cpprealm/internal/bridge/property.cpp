#include <cpprealm/internal/bridge/property.hpp>
#include <cpprealm/internal/bridge/col_key.hpp>

#include <realm/object-store/property.hpp>

namespace realm::internal::bridge {
    property::property() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_property) Property();
#else
        m_property = std::make_shared<Property>();
#endif
    }

    property::property(const property& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_property) Property(*reinterpret_cast<const Property*>(&other.m_property));
#else
        m_property = other.m_property;
#endif
    }

    property& property::operator=(const property& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<Property*>(&m_property) = *reinterpret_cast<const Property*>(&other.m_property);
        }
#else
        m_property = other.m_property;
#endif
        return *this;
    }

    property::property(property&& other) noexcept {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_property) Property(std::move(*reinterpret_cast<Property*>(&other.m_property)));
#else
        m_property = std::move(other.m_property);
#endif
    }

    property& property::operator=(property&& other) noexcept {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<Property*>(&m_property) = std::move(*reinterpret_cast<Property*>(&other.m_property));
        }
#else
        m_property = std::move(other.m_property);
#endif
        return *this;
    }

    property::~property() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<Property*>(&m_property)->~Property();
#endif
    }

    property::property(const realm::Property &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (m_property) Property(v);
#else
        m_property = std::make_shared<Property>(v);
#endif
    }
    property::property(const std::string &name,
                       realm::internal::bridge::property::type type,
                       const std::string &object_name) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_property) Property(name, static_cast<PropertyType>(type), object_name);
#else
        m_property = std::make_shared<Property>(Property(name, static_cast<PropertyType>(type), object_name));

#endif
    }
    property::property(const std::string &name, realm::internal::bridge::property::type type, bool is_primary_key) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_property) Property(name, static_cast<PropertyType>(type), is_primary_key);
#else
        m_property = std::make_shared<Property>(Property(name, static_cast<PropertyType>(type), is_primary_key));
#endif
    }
    void property::set_object_link(const std::string & v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<Property*>(&m_property)->object_type = v;
#else
        m_property->object_type = v;
#endif
    }

    void property::set_origin_property_name(const std::string & v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<Property*>(&m_property)->link_origin_property_name = v;
#else
        m_property->link_origin_property_name = v;
#endif
    }
    col_key property::column_key() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Property*>(&m_property)->column_key;
#else
        return m_property->column_key;
#endif
    }

    property::operator Property() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const Property*>(&m_property);
#else
        return *m_property;
#endif
    }

    void property::set_type(realm::internal::bridge::property::type t) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<Property*>(&m_property)->type = static_cast<PropertyType>(t);
#else
        m_property->type = static_cast<PropertyType>(t);
#endif
    }

    void property::set_primary_key(bool v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<Property*>(&m_property)->is_primary = v;
#else
        m_property->is_primary = v;
#endif
    }

    bool property::is_primary() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Property*>(&m_property)->is_primary;
#else
        return m_property->is_primary;
#endif
    }

    std::string property::name() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Property*>(&m_property)->name;
#else
        return m_property->name;
#endif
    }
}