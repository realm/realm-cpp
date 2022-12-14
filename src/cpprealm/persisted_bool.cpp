#include <cpprealm/persisted_bool.hpp>

namespace realm {
    persisted<bool>::operator bool() const {
        return (*this).operator*();
    }

    bool persisted<bool>::serialize(bool v) {
        return v;
    }

    bool persisted<bool>::deserialize(bool v) {
        return v;
    }
}
