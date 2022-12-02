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


#ifndef REALM_PERSISTED_INT_HPP
#define REALM_PERSISTED_INT_HPP

#include <cpprealm/type_info.hpp>
#include <cpprealm/persisted.hpp>
#include <cpprealm/persisted_custom.hpp>

namespace realm {
    namespace internal {
//        template <>
//        struct type_info<int64_t> {
//            using internal_type = int64_t;
//            static constexpr bridge::property::type type() {
//                return bridge::property::type::Int;
//            }
//            static constexpr internal_type serialize(int64_t v) {
//                return v;
//            }
//            static constexpr int64_t deserialize(const internal_type& v) {
//                return v;
//            }
//        };
//        template <>
//        struct type_info<double> {
//            using internal_type = double;
//            static constexpr bridge::property::type type() {
//                return bridge::property::type::Double;
//            }
//            static constexpr internal_type serialize(double v) {
//                return v;
//            }
//            static constexpr double deserialize(const internal_type& v) {
//                return v;
//            }
//        };
    }

    template <>
    struct persisted<int64_t> : public persisted_base<int64_t> {
        using persisted_base<int64_t>::persisted_base;

        template <typename T>
        std::enable_if_t<std::is_arithmetic_v<T>, persisted&> // NOLINT(misc-unconventional-assign-operator)
        operator=(const T& o) {
            if (this->is_managed()) {
                this->m_object->obj().template set<int64_t>(
                        this->managed,
                        static_cast<int64_t>(o));
            } else {
                new (&this->unmanaged) T(o);
            }
            return *this;
        }
    };

    template <>
    struct persisted<double> : persisted_base<double> {
        template <typename T>
        std::enable_if_t<std::is_arithmetic_v<T>, persisted&> // NOLINT(misc-unconventional-assign-operator)
        operator=(const T& o) {
            if (this->is_managed()) {
                this->m_object->obj().template set<double>(
                        this->managed,
                        static_cast<double>(o));
            } else {
                new (&this->unmanaged) T(o);
            }
            return *this;
        }
    };

    template <typename T, typename V>
    rbool operator>(const persisted<T>& a, const V& b)
    {
        if (a.should_detect_usage_for_queries) {
            auto query = internal::bridge::query(a.query->get_table());
            query.greater(a.managed, internal::type_info::serialize<V>(b));
            return {std::move(query)};
        }
        return *a > b;
    }
    template <typename T>
    rbool operator>(const persisted<T>& a, const persisted<T>& b)
    {
        return a > *b;
    }
    template <typename T, typename V>
    rbool operator>=(const persisted<T>& a, const V& b)
    {
        if (a.should_detect_usage_for_queries) {
            auto query = internal::bridge::query(a.query->get_table());
            query.greater_equal(a.managed, internal::type_info::serialize<V>(b));
            return {std::move(query)};
        }
        return *a >= b;
    }

    template <typename T>
    rbool operator>=(const persisted<T>& a, const persisted<T>& b)
    {
        return a >= *b;
    }
    template <typename T, typename V>
    rbool operator<(const persisted<T>& a, const V& b)
    {
        if (a.should_detect_usage_for_queries) {
            auto query = internal::bridge::query(a.query->get_table());
            query.less(a.managed, internal::type_info::serialize<V>(b));
            return {std::move(query)};
        }
        return *a < b;
    }
    template <typename T>
    rbool operator<(const persisted<T>& a, const persisted<T>& b)
    {
        return a < *b;
    }
    template <typename T, typename V>
    rbool operator<=(const persisted<T>& a, const V& b)
    {
        if (a.should_detect_usage_for_queries) {
            auto query = internal::bridge::query(a.query->get_table());
            query.less_equal(a.managed, internal::type_info::serialize<V>(b));
            return {std::move(query)};
        }
        return *a <= b;
    }
    template <typename T>
    rbool operator<=(const persisted<T>& a, const persisted<T>& b)
    {
        return a <= *b;
    }
    namespace {
        static_assert(sizeof(persisted<int64_t>{}.operator=(1)));
        static_assert(sizeof(persisted<int64_t>{} == 1));
        static_assert(sizeof(persisted<int64_t>{} == 1.0));
        static_assert(sizeof(persisted<double>{} == 1));
        static_assert(sizeof(persisted<double>{} == 1.0));
        static_assert(sizeof(persisted<int64_t>{}.operator=(1.0)));
        static_assert(sizeof(persisted<double>{}.operator=(1)));
        static_assert(sizeof(persisted<double>{}.operator=(1.0)));
    }
}



#endif //REALM_PERSISTED_INT_HPP
