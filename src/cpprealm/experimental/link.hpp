#ifndef CPP_REALM_BRIDGE_LINK_HPP
#define CPP_REALM_BRIDGE_LINK_HPP

#include <cpprealm/internal/type_info.hpp>

namespace realm {
    namespace experimental {
        template <typename>
        struct link;
        template <typename, typename>
        struct managed;
    }
    namespace internal::type_info {
        template <typename T>
        struct type_info<experimental::link<T>> {
            using internal_type = bridge::obj_key;
            static constexpr bridge::property::type type() {
                return bridge::property::type::Object;
            }
        };
        template <typename T>
        struct is_link : std::false_type {
            static constexpr auto value = false;
        };
        template <typename T>
        struct is_link<experimental::link<T>> : std::true_type {
            static constexpr auto value = true;
        };
    }
    namespace experimental {

    }
}
#endif //CPP_REALM_BRIDGE_LINK_HPP
