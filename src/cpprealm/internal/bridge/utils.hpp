#ifndef REALM_BRIDGE_UTILS_HPP
#define REALM_BRIDGE_UTILS_HPP

#include <functional>
#include <string_view>
#include <string>
#include <type_traits>
#include <realm/utilities.hpp>

namespace realm::internal::bridge {
    template <size_t lhs, size_t rhs, typename = void>
    struct SizeCheck;
    template <size_t lhs, size_t rhs>
    struct SizeCheck<lhs, rhs, std::enable_if_t<(lhs == rhs)>> : std::true_type {
    };

    template <typename T>
    struct core_binding {
        using underlying = T;
        virtual operator T() const = 0; //NOLINT(google-explicit-constructor);
    };
}

#endif //REALM_BRIDGE_UTILS_HPP
