#ifndef CPP_REALM_UUID_HPP
#define CPP_REALM_UUID_HPP

#include <string>
#include <cpprealm/internal/bridge/uuid.hpp>
#include <cpprealm/persisted.hpp>
#include <cpprealm/experimental/types.hpp>

namespace realm {

    inline std::ostream& operator<< (std::ostream& stream, const uuid& value)
    {
        return stream << value.to_string();
    }

    inline bool operator ==(const uuid& lhs, const uuid& rhs) { return lhs.m_uuid == rhs.m_uuid; }
    inline bool operator !=(const uuid& lhs, const uuid& rhs) { return lhs.m_uuid != rhs.m_uuid; }
    inline bool operator >(const uuid& lhs, const uuid& rhs) { return lhs.m_uuid > rhs.m_uuid; }
    inline bool operator <(const uuid& lhs, const uuid& rhs) { return lhs.m_uuid < rhs.m_uuid; }
    inline bool operator >=(const uuid& lhs, const uuid& rhs) { return lhs.m_uuid >= rhs.m_uuid; }
    inline bool operator <=(const uuid& lhs, const uuid& rhs) { return lhs.m_uuid <= rhs.m_uuid; }

    template <>
    struct persisted<uuid> : persisted_primitive_base<uuid> {
        using persisted_primitive_base<uuid>::persisted_primitive_base;

    protected:
        static internal::bridge::uuid serialize(const uuid&, const std::optional<internal::bridge::realm>& = std::nullopt);
        static uuid deserialize(const internal::bridge::uuid&);

        __cpp_realm_friends
    };

    __cpp_realm_generate_operator(uuid, >, greater)
    __cpp_realm_generate_operator(uuid, <, less)
    __cpp_realm_generate_operator(uuid, >=, greater_equal)
    __cpp_realm_generate_operator(uuid, <=, less_equal)
    __cpp_realm_generate_operator(uuid, ==, equal)
    __cpp_realm_generate_operator(uuid, !=, not_equal)
}

#endif // CPP_REALM_UUID_HPP
