#include <cpprealm/internal/bridge/property.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/col_key.hpp>

#include <realm/object-store/property.hpp>

namespace realm::internal::bridge {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
    static_assert(LayoutCheck<storage::Property, Property>{});
#elif __i386__
    static_assert(SizeCheck<64, sizeof(Property)>{});
    static_assert(SizeCheck<4, alignof(Property)>{});
#elif __x86_64__
    #if defined(__clang__)
    static_assert(SizeCheck<120, sizeof(Property)>{});
    #elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<152, sizeof(Property)>{});
    #endif
    static_assert(SizeCheck<8, alignof(Property)>{});
#elif __arm__
    static_assert(SizeCheck<64, sizeof(Property)>{});
    static_assert(SizeCheck<8, alignof(Property)>{});
#elif __aarch64__
#if defined(__clang__)
    static_assert(SizeCheck<120, sizeof(Property)>{});
#elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<152, sizeof(Property)>{});
#endif
    static_assert(SizeCheck<8, alignof(Property)>{});
#elif _WIN32
    static_assert(SizeCheck<8, alignof(Property)>{});
    #if _DEBUG
    static_assert(SizeCheck<184, sizeof(Property)>{});
    #else
    static_assert(SizeCheck<152, sizeof(Property)>{});
    #endif
#endif


    property::property() {
        new (&m_property) Property();
    }

    property::property(const property& other) {
        new (&m_property) Property(*reinterpret_cast<const Property*>(&other.m_property));
    }

    property& property::operator=(const property& other) {
        if (this != &other) {
            *reinterpret_cast<Property*>(&m_property) = *reinterpret_cast<const Property*>(&other.m_property);
        }
        return *this;
    }

    property::property(property&& other) noexcept {
        new (&m_property) Property(std::move(*reinterpret_cast<Property*>(&other.m_property)));
    }

    property& property::operator=(property&& other) noexcept {
        if (this != &other) {
            *reinterpret_cast<Property*>(&m_property) = std::move(*reinterpret_cast<Property*>(&other.m_property));
        }
        return *this;
    }

    property::~property() {
        reinterpret_cast<Property*>(&m_property)->~Property();
    }

    property::property(const realm::Property &v) {
        new (m_property) Property(v);
    }
    property::property(const std::string &name,
                       realm::internal::bridge::property::type type,
                       const std::string &object_name) {
        new (&m_property) Property(name, static_cast<PropertyType>(type), object_name);
    }
    property::property(const std::string &name, realm::internal::bridge::property::type type, bool is_primary_key) {
        new (&m_property) Property(name, static_cast<PropertyType>(type), is_primary_key);
    }
    void property::set_object_link(const std::string & v) {
        reinterpret_cast<Property*>(&m_property)->object_type = v;
    }
    void property::set_is_computed(bool b) {
    }
    void property::set_origin_property_name(const std::string & v) {
        reinterpret_cast<Property*>(&m_property)->link_origin_property_name = v;
    }
    col_key property::column_key() const {
        return reinterpret_cast<const Property*>(&m_property)->column_key;
    }

    property::operator Property() const {
        return *reinterpret_cast<const Property*>(&m_property);
    }

    void property::set_type(realm::internal::bridge::property::type t) {
        reinterpret_cast<Property*>(&m_property)->type = static_cast<PropertyType>(t);
    }

    void property::set_primary_key(bool v) {
        reinterpret_cast<Property*>(&m_property)->is_primary = v;
    }

    bool property::is_primary() const {
        return reinterpret_cast<const Property*>(&m_property)->is_primary;
    }

    std::string property::name() const {
        return reinterpret_cast<const Property*>(&m_property)->name;
    }
}