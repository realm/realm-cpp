#include <cpprealm/persisted_object_id.hpp>

namespace realm {
    object_id::object_id(const std::string &v)
            : m_object_id(v)
    {
    }
    object_id::object_id(const internal::bridge::object_id &v)
            : m_object_id(v)
    {
    }

    object_id object_id::generate() {
        return object_id(internal::bridge::object_id::generate());
    }

    std::string object_id::to_string() const {
        return m_object_id.to_string();
    }

    internal::bridge::object_id persisted<object_id>::serialize(const object_id &v) {
        return static_cast<internal::bridge::object_id>(v);
    }

    object_id persisted<object_id>::deserialize(const internal::bridge::object_id &v) {
        return static_cast<object_id>(v);
    }
}