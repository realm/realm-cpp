#include <cpprealm/internal/bridge/uuid.hpp>

#include <cpprealm/experimental/types.hpp>
#include <realm/uuid.hpp>

namespace realm::internal::bridge {

    uuid::uuid() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_uuid) UUID();
#else
        m_uuid = UUID::UUIDBytes();
#endif
    }
    uuid::uuid(const uuid& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_uuid) UUID(*reinterpret_cast<const UUID*>(&other.m_uuid));
#else
        m_uuid = other.m_uuid;
#endif
    }

    uuid& uuid::operator=(const uuid& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<UUID*>(&m_uuid) = *reinterpret_cast<const UUID*>(&other.m_uuid);
        }
#else
        m_uuid = other.m_uuid;
#endif
        return *this;
    }

    uuid::uuid(uuid&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_uuid) UUID(std::move(*reinterpret_cast<UUID*>(&other.m_uuid)));
#else
        m_uuid = std::move(other.m_uuid);
#endif
    }

    uuid& uuid::operator=(uuid&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<UUID*>(&m_uuid) = std::move(*reinterpret_cast<UUID*>(&other.m_uuid));
        }
#else
        m_uuid = std::move(other.m_uuid);
#endif
        return *this;
    }

    uuid::~uuid() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<UUID*>(&m_uuid)->~UUID();
#endif
    }
    uuid::uuid(const std::string &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_uuid) UUID(v);
#else
        m_uuid = UUID(v).to_bytes();
#endif
    }
    uuid::uuid(const ::realm::uuid &v)
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_uuid) UUID(v.to_string());
#else
        m_uuid = UUID(v.to_string()).to_bytes();
#endif
    }
    uuid::uuid(const UUID &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_uuid) UUID(v);
#else
        m_uuid = UUID(v).to_bytes();
#endif
    }

    std::string uuid::to_string() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const UUID*>(&m_uuid)->to_string();
#else
        return UUID(m_uuid).to_string();
#endif
    }

    uuid::operator ::realm::uuid() const {
        return ::realm::uuid(*this);
    }

    uuid::operator UUID() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const UUID*>(&m_uuid);
#else
        return UUID(m_uuid);
#endif
    }

    std::string uuid::to_base64() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const UUID*>(&m_uuid)->to_base64();
#else
        return UUID(m_uuid).to_base64();
#endif
    }

    std::array<uint8_t, 16> uuid::to_bytes() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const UUID*>(&m_uuid)->to_bytes();
#else
        return m_uuid;
#endif
    }

#define __cpp_realm_gen_uuid_op(op) \
    bool operator op(const uuid& a, const uuid& b) { \
        return a.operator UUID() op b.operator UUID(); \
    }

    __cpp_realm_gen_uuid_op(==)
    __cpp_realm_gen_uuid_op(!=)
    __cpp_realm_gen_uuid_op(>)
    __cpp_realm_gen_uuid_op(<)
    __cpp_realm_gen_uuid_op(>=)
    __cpp_realm_gen_uuid_op(<=)
}