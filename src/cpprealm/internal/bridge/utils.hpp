#ifndef REALM_BRIDGE_UTILS_HPP
#define REALM_BRIDGE_UTILS_HPP

#include <functional>
#include <string_view>
#include <string>
#include <type_traits>
#include <realm/utilities.hpp>

#if __has_include(<cpprealm/internal/bridge/bridge_types.hpp>)
#include <cpprealm/internal/bridge/bridge_types.hpp>
#endif

namespace realm::internal::bridge {
    template <typename Left, typename Right, typename = void>
    struct LayoutCheck;
    template <typename Left, typename Right>
    struct LayoutCheck<Left, Right, std::enable_if_t<(sizeof(Left) == sizeof(Right) && alignof(Left) == alignof(Right))>> : std::true_type {
    };

    template <typename T>
    struct core_binding {
        using underlying = T;
        virtual operator T() const = 0; //NOLINT(google-explicit-constructor);
    };
}

#endif //REALM_BRIDGE_UTILS_HPP
