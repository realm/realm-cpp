#include <cpprealm/persisted_uuid.hpp>

namespace realm {
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
}