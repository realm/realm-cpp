#include <cpprealm/persisted_object_id.hpp>

namespace realm {
    internal::bridge::object_id persisted<object_id>::serialize(const object_id &v, const std::optional<internal::bridge::realm>&) {
        return static_cast<internal::bridge::object_id>(v);
    }

    object_id persisted<object_id>::deserialize(const internal::bridge::object_id &v) {
        return static_cast<object_id>(v);
    }
}