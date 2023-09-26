#include <cpprealm/internal/bridge/schema.hpp>
#include <cpprealm/internal/bridge/object_schema.hpp>

#include <realm/object-store/schema.hpp>
#include <realm/object-store/property.hpp>

namespace realm::internal::bridge {
    schema::schema() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_schema) Schema();
#else
        m_schema = std::make_shared<Schema>();
#endif
    }

    schema::schema(const schema& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_schema) Schema(*reinterpret_cast<const Schema*>(&other.m_schema));
#else
        m_schema = other.m_schema;
#endif
    }

    schema& schema::operator=(const schema& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<Schema*>(&m_schema) = *reinterpret_cast<const Schema*>(&other.m_schema);
        }
#else
        m_schema = other.m_schema;
#endif
        return *this;
    }

    schema::schema(schema&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_schema) Schema(std::move(*reinterpret_cast<Schema*>(&other.m_schema)));
#else
        m_schema = std::move(other.m_schema);
#endif
    }

    schema& schema::operator=(schema&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<Schema*>(&m_schema) = std::move(*reinterpret_cast<Schema*>(&other.m_schema));
        }
#else
        m_schema = std::move(other.m_schema);
#endif
        return *this;
    }

    schema::~schema() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<Schema*>(&m_schema)->~Schema();
#endif
    }

    object_schema schema::find(const std::string &name) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<Schema*>(&m_schema)->find(name);
#else
        return *m_schema->find(name);
#endif
    }

    schema::schema(const std::vector<object_schema> &v) {
        std::vector<ObjectSchema> v2;
        for (auto& os : v) {
            v2.push_back(os);
        }
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_schema) Schema(v2);
#else
        m_schema = std::make_shared<Schema>(v2);
#endif
    }

    schema::operator Schema() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const Schema*>(&m_schema);
#else
        return *m_schema;
#endif
    }

    schema::schema(const realm::Schema &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_schema) Schema(v);
#else
        m_schema = std::make_shared<Schema>(v);
#endif
    }
}
