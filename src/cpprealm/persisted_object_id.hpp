#ifndef CPP_REALM_OBJECT_ID_HPP
#define CPP_REALM_OBJECT_ID_HPP

#include <string>
#include <cpprealm/persisted.hpp>
#include <cpprealm/experimental/types.hpp>
namespace realm {


    inline std::ostream& operator<< (std::ostream& stream, const object_id& value)
    {
        return stream << value.to_string();
    }

    template <>
    struct persisted<object_id> : persisted_primitive_base<object_id> {
        using persisted_primitive_base<object_id>::persisted_primitive_base;


        static internal::bridge::object_id serialize(const object_id&, const std::optional<internal::bridge::realm>& = std::nullopt);
        static object_id deserialize(const internal::bridge::object_id&);


        __cpp_realm_friends
    };

    __cpp_realm_generate_operator(object_id, >, greater)
    __cpp_realm_generate_operator(object_id, <, less)
    __cpp_realm_generate_operator(object_id, >=, greater_equal)
    __cpp_realm_generate_operator(object_id, <=, less_equal)
    __cpp_realm_generate_operator(object_id, ==, equal)
    __cpp_realm_generate_operator(object_id, !=, not_equal)
}

#endif // CPP_REALM_OBJECT_ID_HPP
