#ifndef CPP_REALM_UUID_HPP
#define CPP_REALM_UUID_HPP

#include <string>
#include <cpprealm/type_info.hpp>
#include <cpprealm/persisted.hpp>

namespace realm {
    struct uuid {
        explicit uuid(const std::string &);
        [[nodiscard]] std::string to_string() const;
        uuid() = default;
    private:
        uuid(const internal::bridge::uuid&);
        template <typename T, typename>
        friend typename internal::type_info::type_info<std::decay_t<T>>::internal_type internal::type_info::serialize(T);
        template <typename T, typename>
        friend T internal::type_info::deserialize(typename internal::type_info::type_info<T>::internal_type&&);
        internal::bridge::uuid m_uuid;
        template <typename mapped_type>
        friend struct box_base;
    };

    rbool operator ==(const uuid& lhs, const uuid& rhs);
    rbool operator !=(const uuid& lhs, const uuid& rhs);

    template <>
    struct persisted<uuid> : persisted_base<uuid> {
        using persisted_base<uuid>::persisted_base;
    };
}

#endif // CPP_REALM_UUID_HPP
