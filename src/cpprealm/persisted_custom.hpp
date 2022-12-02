#ifndef REALM_PERSISTED_CUSTOM_HPP
#define REALM_PERSISTED_CUSTOM_HPP

#include <cpprealm/persisted.hpp>
#include <cpprealm/persisted_uuid.hpp>
#include <cpprealm/internal/type_info.hpp>

namespace realm {
    template <typename T>
    struct persisted<T, std::enable_if_t<internal::type_info::is_custom_type<T>::value>> :
            public persisted<typename internal::type_info::is_custom_type<T>::underlying> {
        using persisted<typename internal::type_info::is_custom_type<T>::underlying>::persisted;
        using persisted<typename internal::type_info::is_custom_type<T>::underlying>::operator=;
    };
}
#endif //REALM_PERSISTED_CUSTOM_HPP
