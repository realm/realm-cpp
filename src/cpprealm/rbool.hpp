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

#ifndef CPPREALM_RBOOL_HPP
#define CPPREALM_RBOOL_HPP

#include <cpprealm/db.hpp>
#include <cpprealm/schema.hpp>

#include <cpprealm/internal/bridge/query.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    template<typename>
    struct box_base;
}

namespace realm {
    // MARK: rbool

    class rbool {
        bool is_for_queries = false;
        friend rbool operator&&(const rbool &lhs, const rbool &rhs);

        template<typename T>
        friend struct results;

        friend rbool operator||(const rbool &lhs, const rbool &rhs);

    public:
        ~rbool() {
            if (is_for_queries)
                q.~query();
        }
        operator bool() const {
            return b;
        }
        rbool operator!() const {
            if (is_for_queries) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
                new(&q) internal::bridge::query(q.negate());
#else
                q = internal::bridge::query(q.negate());
#endif
                return *this;
            }
            return !b;
        }
        union {
            bool b;
            mutable internal::bridge::query q;
        };

        rbool(internal::bridge::query &&q) : q(q), is_for_queries(true) {}
        rbool(bool b) : b(b) {}
        rbool(const rbool &r) {
            if (r.is_for_queries) {
                new (&q) internal::bridge::query(r.q);
                is_for_queries = true;
            } else
                b = r.b;
        }
    };

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

#endif //CPPREALM_RBOOL_HPP
