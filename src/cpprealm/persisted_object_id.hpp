#ifndef CPP_REALM_OBJECT_ID_HPP
#define CPP_REALM_OBJECT_ID_HPP

#include <string>
#include <cpprealm/persisted.hpp>

namespace realm {
    struct object_id {
        explicit object_id(const std::string &);
        object_id() = default;
        [[nodiscard]] static object_id generate();
        [[nodiscard]] std::string to_string() const;
    private:
        object_id(const internal::bridge::object_id&); //NOLINT(google-explicit-constructor)
        internal::bridge::object_id m_object_id;
        friend object_id generate();
        friend struct internal::bridge::object_id;
        template <typename mapped_type>
        friend struct box_base;
        friend inline bool operator ==(const object_id& lhs, const object_id& rhs);
        friend inline bool operator !=(const object_id& lhs, const object_id& rhs);
        __cpp_realm_friends
    };

    inline std::ostream& operator<< (std::ostream& stream, const object_id& value)
    {
        return stream << value.to_string();
    }

    inline bool operator ==(const object_id& lhs, const object_id& rhs) { return lhs.m_object_id == rhs.m_object_id; }
    inline bool operator !=(const object_id& lhs, const object_id& rhs) { return lhs.m_object_id != rhs.m_object_id; }

    template <>
    struct persisted<object_id> : persisted_primitive_base<object_id> {
        using persisted_primitive_base<object_id>::persisted_primitive_base;

    protected:
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
