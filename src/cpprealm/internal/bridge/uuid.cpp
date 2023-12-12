#include <cpprealm/internal/bridge/uuid.hpp>

#include "cpprealm/types.hpp"
#include <realm/uuid.hpp>

namespace realm::internal::bridge {
    uuid::uuid() {
        m_uuid = UUID::UUIDBytes();
    }

    uuid::uuid(const std::string &v) {
        m_uuid = UUID(v).to_bytes();
    }

    uuid::uuid(const ::realm::uuid &v)
    {
        m_uuid = UUID(v.to_string()).to_bytes();
    }

    uuid::uuid(const UUID &v) {
        m_uuid = UUID(v).to_bytes();
    }

    std::string uuid::to_string() const {
        return UUID(m_uuid).to_string();
    }

    uuid::operator ::realm::uuid() const {
        return ::realm::uuid(*this);
    }

    uuid::operator UUID() const {
        return UUID(m_uuid);
    }

    std::string uuid::to_base64() const {
        return UUID(m_uuid).to_base64();
    }

    std::array<uint8_t, 16> uuid::to_bytes() const {
        return m_uuid;
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