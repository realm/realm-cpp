#ifndef CPPREALM_PERSISTED_DECIMAL128_HPP
#define CPPREALM_PERSISTED_DECIMAL128_HPP

#include <cpprealm/persisted.hpp>
#include <cpprealm/experimental/types.hpp>
namespace realm {

    inline std::ostream& operator<< (std::ostream& stream, const decimal128& value)
    {
        return stream << value.to_string();
    }

    template <>
    struct persisted<decimal128> : persisted_primitive_base<decimal128> {
        using persisted_primitive_base<decimal128>::persisted_primitive_base;

    protected:
        static internal::bridge::decimal128 serialize(const decimal128&, const std::optional<internal::bridge::realm>& = std::nullopt);
        static decimal128 deserialize(const internal::bridge::decimal128&);

        __cpp_realm_friends
    };
}

#endif // CPPREALM_PERSISTED_DECIMAL128_HPP
