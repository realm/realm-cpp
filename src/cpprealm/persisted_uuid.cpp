#include <cpprealm/persisted_uuid.hpp>

namespace realm {
    /*
    uuid::uuid(const std::string &v)
    : m_uuid(v)
    {
    }
    uuid::uuid(const internal::bridge::uuid &v)
    : m_uuid(v)
    {
    }
    std::string uuid::to_string() const {
        return m_uuid.to_string();
    }

    std::string uuid::to_base64() const {
        return m_uuid.to_base64();
    }

    std::array<uint8_t, 16> uuid::to_bytes() const {
        return m_uuid.to_bytes();
    }*/

    internal::bridge::uuid persisted<uuid>::serialize(const uuid &v, const std::optional<internal::bridge::realm>&) {
        return static_cast<internal::bridge::uuid>(v);
    }
    uuid persisted<uuid>::deserialize(const internal::bridge::uuid &v) {
        return static_cast<uuid>(v);
    }
}