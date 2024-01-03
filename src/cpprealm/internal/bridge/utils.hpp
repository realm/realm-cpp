////////////////////////////////////////////////////////////////////////////
//
// Copyright 2024 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef CPPREALM_BRIDGE_UTILS_HPP
#define CPPREALM_BRIDGE_UTILS_HPP

#include <functional>
#include <string_view>
#include <string>
#include <type_traits>

#ifdef _WIN32

#include <WinSock2.h>
#include <intrin.h>
#include <BaseTsd.h>

#undef max // collides with numeric_limits::max called later in this header file
#undef min // collides with numeric_limits::min called later in this header file
#include <safeint.h>

#endif // _WIN32

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

#endif //CPPREALM_BRIDGE_UTILS_HPP
