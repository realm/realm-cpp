#include <cpprealm/internal/bridge/schema.hpp>
#include <cpprealm/internal/bridge/object_schema.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/object-store/schema.hpp>
#include <realm/object-store/property.hpp>

namespace realm::internal::bridge {
#ifdef __i386__
    static_assert(SizeCheck<12, sizeof(Schema)>{});
    static_assert(SizeCheck<4, alignof(Schema)>{});
#elif __x86_64__
    static_assert(SizeCheck<24, sizeof(Schema)>{});
    static_assert(SizeCheck<8, alignof(Schema)>{});
#elif __arm__
    static_assert(SizeCheck<12, sizeof(Schema)>{});
    static_assert(SizeCheck<4, alignof(Schema)>{});
#elif __aarch64__
    static_assert(SizeCheck<24, sizeof(Schema)>{});
    static_assert(SizeCheck<8, alignof(Schema)>{});
#else
    static_assert(SizeCheck<24, sizeof(Schema)>{});
    static_assert(SizeCheck<8, alignof(Schema)>{});
#endif

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
