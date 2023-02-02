#include <cpprealm/persisted_bool.hpp>

namespace realm {
    persisted<bool>::operator bool() const {
        return (*this).operator*();
    }

    bool persisted<bool>::serialize(bool v, const std::optional<internal::bridge::realm>&) {
        return v;
    }

    bool persisted<bool>::deserialize(bool v) {
        return v;
    }
}
