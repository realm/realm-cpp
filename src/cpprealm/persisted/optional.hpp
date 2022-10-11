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

#ifndef REALM_PERSISTED_OPTIONAL_HPP
#define REALM_PERSISTED_OPTIONAL_HPP

#include <cpprealm/persisted.hpp>

namespace realm {
    template<typename T>
    struct persisted<T, type_info::OptionalPersistable<T>> : public persisted_noncontainer_base<T> {
        using persisted_noncontainer_base<T>::persisted_noncontainer_base;
        using persisted_noncontainer_base<T>::operator=;
        using persisted_noncontainer_base<T>::operator*;
    };
    template <typename T>
    std::enable_if_t<type_info::is_optional<T>::value, rbool>
    inline operator==(const persisted<T>& a, const T& b)
    {
        if (a.should_detect_usage_for_queries) {
            auto query = Query(a.query->get_table());
            if (b) {
                query.equal(a.managed, type_info::persisted_type<T>::convert_if_required(*b));
            } else {
                query.equal(a.managed, realm::null{});
            }
            return {std::move(query)};
        }

        if (a && b) {
            return *a == *b;
        } else if (!a && !b) {
            return true;
        } else {
            return false;
        }
    }
    template <typename T>
    std::enable_if_t<type_info::is_optional<T>::value, rbool>
    inline operator==(const persisted<T>& a, const typename T::value_type& b)
    {
        if (a.should_detect_usage_for_queries) {
            auto query = Query(a.query->get_table());
            query.equal(a.managed, type_info::persisted_type<typename T::value_type>::convert_if_required(b));
            return {std::move(query)};
        }

        if (*a) {
            return **a == b;
        } else {
            return false;
        }
    }
    template <typename T>
    std::enable_if_t<type_info::is_optional<T>::value, rbool>
    inline operator==(const persisted<T>& a, const std::nullopt_t&)
    {
        if (a.should_detect_usage_for_queries) {
            auto query = Query(a.query->get_table());
            query.equal(a.managed, std::nullopt);
            return {std::move(query)};
        }

        return !*a;
    }
}

#endif //REALM_PERSISTED_OPTIONAL_HPP
