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
#elif _WIN32
    static_assert(SizeCheck<32, sizeof(Schema)>{});
    static_assert(SizeCheck<8, alignof(Schema)>{});
#endif

    schema::schema() {
        new (&m_schema) Schema();
    }

    schema::schema(const schema& other) {
        new (&m_schema) Schema(*reinterpret_cast<const Schema*>(&other.m_schema));
    }

    schema& schema::operator=(const schema& other) {
        if (this != &other) {
            *reinterpret_cast<Schema*>(&m_schema) = *reinterpret_cast<const Schema*>(&other.m_schema);
        }
        return *this;
    }

    schema::schema(schema&& other) {
        new (&m_schema) Schema(std::move(*reinterpret_cast<Schema*>(&other.m_schema)));
    }

    schema& schema::operator=(schema&& other) {
        if (this != &other) {
            *reinterpret_cast<Schema*>(&m_schema) = std::move(*reinterpret_cast<Schema*>(&other.m_schema));
        }
        return *this;
    }

    schema::~schema() {
        reinterpret_cast<Schema*>(&m_schema)->~Schema();
    }

    object_schema schema::find(const std::string &name) {
        return *reinterpret_cast<Schema*>(&m_schema)->find(name);
    }

    schema::schema(const std::vector<object_schema> &v) {
        std::vector<ObjectSchema> v2;
        for (auto& os : v) {
            v2.push_back(os);
        }
        new (&m_schema) Schema(v2);
    }

    schema::operator Schema() const {
        return *reinterpret_cast<const Schema*>(&m_schema);
    }

    schema::schema(const realm::Schema &v) {
        new (&m_schema) Schema(v);
    }
}
