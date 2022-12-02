#include <cpprealm/internal/bridge/uuid.hpp>
#include <realm/uuid.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/persisted_uuid.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<16, sizeof(::realm::UUID)>{});
    uuid::uuid() {
        new (&m_uuid) UUID();
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
        return reinterpret_cast<const UUID*>(m_uuid)->to_string();
    }

    uuid::operator UUID() const {
        return *reinterpret_cast<const UUID*>(m_uuid);
    }
}