#ifndef CPP_REALM_UUID_HPP
#define CPP_REALM_UUID_HPP

#include <string>
#include <cpprealm/persisted.hpp>

namespace realm {
    struct uuid {
        explicit uuid(const std::string &);
        uuid() = default;
        [[nodiscard]] std::string to_string() const;
        [[nodiscard]] std::string to_base64() const;
        [[nodiscard]] std::array<uint8_t, 16> to_bytes() const;
    private:
        uuid(const internal::bridge::uuid&); //NOLINT(google-explicit-constructor)
        internal::bridge::uuid m_uuid;
        friend struct internal::bridge::uuid;
        template <typename mapped_type>
        friend struct box_base;
        friend inline bool operator ==(const uuid& lhs, const uuid& rhs);
        friend inline bool operator !=(const uuid& lhs, const uuid& rhs);
        friend inline bool operator <(const uuid& lhs, const uuid& rhs);
        friend inline bool operator >(const uuid& lhs, const uuid& rhs);
        friend inline bool operator <=(const uuid& lhs, const uuid& rhs);
        friend inline bool operator >=(const uuid& lhs, const uuid& rhs);
        __cpp_realm_friends
    };

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
