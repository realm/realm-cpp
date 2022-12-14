#include <cpprealm/internal/bridge/schema.hpp>
#include <cpprealm/internal/bridge/object_schema.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/object-store/schema.hpp>
#include <realm/object-store/property.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<24, sizeof(Schema)>{});

    object_schema schema::find(const std::string &name) {
        return *reinterpret_cast<Schema*>(m_schema)->find(name);
    }

    schema::schema(const std::vector<object_schema> &v) {
        std::vector<ObjectSchema> v2;
        for (auto& os : v) {
            v2.push_back(os);
        }
        new (&m_schema) Schema(v2);
    }

    schema::operator Schema() const {
        return *reinterpret_cast<const Schema*>(m_schema);
    }

    schema::schema(const realm::Schema &v) {
        new (&m_schema) Schema(v);
    }
}
