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

#ifndef REALM_PERSISTED_ENUM_HPP
#define REALM_PERSISTED_ENUM_HPP

#include <cpprealm/persisted.hpp>

namespace realm {
    template <typename T>
    struct persisted<T, std::enable_if_t<std::is_enum_v<T>>> : public persisted_primitive_base<T> {
        using persisted_primitive_base<T>::persisted_primitive_base;
        using persisted_primitive_base<T>::operator=;

        persisted& operator=(T&& o) noexcept {
            if (this->is_managed()) {
                this->m_object->get_obj().set(this->managed, static_cast<int64_t>(std::move(o)));
            } else {
                new (&this->unmanaged) T(std::move(o));
            }
            return *this;
        }
    protected:
        static int64_t serialize(const T& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
            return static_cast<int64_t>(v);
        }
        static T deserialize(const int64_t& v) {
            return static_cast<T>(v);
        }

        __cpp_realm_friends
    };

#define __cpp_realm_generate_enum_operator(op, name) \
    template <typename T, typename V> \
    std::enable_if_t<std::is_enum_v<T>, rbool> operator op(const persisted<T>& a, const V& b) { \
        if (a.should_detect_usage_for_queries) { \
            auto query = internal::bridge::query(a.query->get_table()); \
            query.name(a.managed, persisted<V>::serialize(b)); \
            return query; \
        } \
        return *a op b; \
    }                                          \
    template <typename T, typename V> \
    std::enable_if_t<std::is_enum_v<T>, rbool> operator op(const persisted<T>& a, const persisted<V>& b) { \
        return a op *b; \
    }

    __cpp_realm_generate_enum_operator(==, equal)
    __cpp_realm_generate_enum_operator(!=, not_equal)
    __cpp_realm_generate_enum_operator(>, greater)
    __cpp_realm_generate_enum_operator(<, less)
    __cpp_realm_generate_enum_operator(<=, less_equal)
    __cpp_realm_generate_enum_operator(>=, greater_equal)

    namespace {
        enum class test_enum {
            one, two
        };
        static_assert(sizeof(persisted<test_enum>));
        static_assert(sizeof(persisted<test_enum>{}.operator=(test_enum::one)));
    }
}

#endif //REALM_PERSISTED_ENUM_HPP
