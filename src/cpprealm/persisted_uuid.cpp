#include <cpprealm/persisted_uuid.hpp>

namespace realm {

    internal::bridge::uuid persisted<uuid>::serialize(const uuid &v, const std::optional<internal::bridge::realm>&) {
        return static_cast<internal::bridge::uuid>(v);
    }
    uuid persisted<uuid>::deserialize(const internal::bridge::uuid &v) {
        return static_cast<uuid>(v);
    }
}