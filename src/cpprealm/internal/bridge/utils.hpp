#ifndef REALM_UTILS_HPP
#define REALM_UTILS_HPP

#include <functional>

namespace realm::internal::bridge {
    template <size_t lhs, size_t rhs, typename = void>
    struct SizeCheck;
    template <size_t lhs, size_t rhs>
    struct SizeCheck<lhs, rhs, std::enable_if_t<(lhs == rhs)>> : std::true_type {
    };
}

#endif //REALM_UTILS_HPP
