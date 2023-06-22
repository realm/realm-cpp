#include <cpprealm/experimental/types.hpp>

namespace realm {
    uuid::uuid(const std::string &v)
        : m_uuid(v) {
    }
    uuid::uuid(const internal::bridge::uuid &v)
        : m_uuid(v) {
    }
    std::string uuid::to_string() const {
        return m_uuid.to_string();
    }

    std::string uuid::to_base64() const {
        return m_uuid.to_base64();
    }

    std::array<uint8_t, 16> uuid::to_bytes() const {
        return m_uuid.to_bytes();
    }

    object_id::object_id(const std::string &v)
        : m_object_id(v) {
    }
    object_id::object_id(const internal::bridge::object_id &v)
        : m_object_id(v) {
    }

    object_id object_id::generate() {
        return object_id(internal::bridge::object_id::generate());
    }

    std::string object_id::to_string() const {
        return m_object_id.to_string();
    }
} // namespace realm
