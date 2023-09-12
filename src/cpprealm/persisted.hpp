////////////////////////////////////////////////////////////////////////////
//
// Copyright 2022 Realm Inc.
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

#ifndef realm_persisted_hpp
#define realm_persisted_hpp

#include <cpprealm/internal/type_info.hpp>
#include <cpprealm/internal/bridge/object.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/rbool.hpp>

namespace realm::experimental {
    template<typename>
    struct box_base;
}

namespace realm {
    namespace cpprealm {
        constexpr size_t npos = size_t(-1);
    }
    namespace {
        template<typename T>
        using is_optional = internal::type_info::is_optional<T>;
    }
    namespace schemagen {
        template <typename Class, typename ...Properties>
        struct schema;
        template <auto Ptr, bool IsPrimaryKey>
        struct property;
    }

//// MARK: Equatable

inline rbool operator &&(const rbool& lhs, const rbool& rhs) {
    if (lhs.is_for_queries) {
        lhs.q.and_query(rhs.q);
        return lhs;
    }
    return lhs.b && rhs.b;
}
inline rbool operator ||(const rbool& lhs, const rbool& rhs) {
    if (lhs.is_for_queries) {
        lhs.q = lhs.q || rhs.q;
        return lhs;
    }
    return lhs.b && rhs.b;
}

}

#endif /* realm_persisted_hpp */
