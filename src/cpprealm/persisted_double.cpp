#include <cpprealm/persisted_double.hpp>

namespace realm {
    double persisted<double>::serialize(double v) {
        return v;
    }
    double persisted<double>::deserialize(double v) {
        return v;
    }
}