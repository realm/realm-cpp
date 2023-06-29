#include <cpprealm/persisted_decimal128.hpp>

namespace realm {
    internal::bridge::decimal128 persisted<decimal128>::serialize(const decimal128 &v, const std::optional<internal::bridge::realm>&) {
        return static_cast<internal::bridge::decimal128>(v);
    }

    decimal128 persisted<decimal128>::deserialize(const internal::bridge::decimal128 &v) {
        return static_cast<decimal128>(v);
    }
}