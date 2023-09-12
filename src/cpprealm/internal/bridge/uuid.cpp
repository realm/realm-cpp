#include <cpprealm/internal/bridge/uuid.hpp>
#include <realm/uuid.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/experimental/types.hpp>

namespace realm::internal::bridge {
#ifdef __i386__
    static_assert(SizeCheck<16, sizeof(::realm::UUID)>{});
    static_assert(SizeCheck<1, alignof(::realm::UUID)>{});
#elif __x86_64__
    static_assert(SizeCheck<16, sizeof(::realm::UUID)>{});
    static_assert(SizeCheck<1, alignof(::realm::UUID)>{});
#elif __arm__
    static_assert(SizeCheck<16, sizeof(::realm::UUID)>{});
    static_assert(SizeCheck<1, alignof(::realm::UUID)>{});
#elif __aarch64__
    static_assert(SizeCheck<16, sizeof(::realm::UUID)>{});
    static_assert(SizeCheck<1, alignof(::realm::UUID)>{});
#elif _WIN32
    static_assert(SizeCheck<16, sizeof(::realm::UUID)>{});
    static_assert(SizeCheck<1, alignof(::realm::UUID)>{});
#endif
    uuid::uuid() {
        new (&m_uuid) UUID();
    }
    uuid::uuid(const uuid& other) {
        new (&m_uuid) UUID(*reinterpret_cast<const UUID*>(&other.m_uuid));
    }

    uuid& uuid::operator=(const uuid& other) {
        if (this != &other) {
            *reinterpret_cast<UUID*>(&m_uuid) = *reinterpret_cast<const UUID*>(&other.m_uuid);
        }
        return *this;
    }

    uuid::uuid(uuid&& other) {
        new (&m_uuid) UUID(std::move(*reinterpret_cast<UUID*>(&other.m_uuid)));
    }

    uuid& uuid::operator=(uuid&& other) {
        if (this != &other) {
            *reinterpret_cast<UUID*>(&m_uuid) = std::move(*reinterpret_cast<UUID*>(&other.m_uuid));
        }
        return *this;
    }

    uuid::~uuid() {
        reinterpret_cast<UUID*>(&m_uuid)->~UUID();
    }
    uuid::uuid(const std::string &v) {
        new (&m_uuid) UUID(v);
    }
    uuid::uuid(const ::realm::uuid &v)
    {
        new (&m_uuid) UUID(v.to_string());
    }
    uuid::uuid(const UUID &v) {
        new (&m_uuid) UUID(v);
    }

    std::string uuid::to_string() const {
        return reinterpret_cast<const UUID*>(&m_uuid)->to_string();
    }

    uuid::operator ::realm::uuid() const {
        return ::realm::uuid(*this);
    }

    uuid::operator UUID() const {
        return *reinterpret_cast<const UUID*>(&m_uuid);
    }

    std::string uuid::to_base64() const {
        return reinterpret_cast<const UUID*>(&m_uuid)->to_base64();
    }

    std::array<uint8_t, 16> uuid::to_bytes() const {
        return reinterpret_cast<const UUID*>(&m_uuid)->to_bytes();
    }

#define __cpp_realm_gen_uuid_op(op) \
    bool operator op(const uuid& a, const uuid& b) { \
        return *reinterpret_cast<const UUID*>(a.m_uuid) op *reinterpret_cast<const UUID*>(b.m_uuid);                                        \
    }

    __cpp_realm_gen_uuid_op(==)
    __cpp_realm_gen_uuid_op(!=)
    __cpp_realm_gen_uuid_op(>)
    __cpp_realm_gen_uuid_op(<)
    __cpp_realm_gen_uuid_op(>=)
    __cpp_realm_gen_uuid_op(<=)
}