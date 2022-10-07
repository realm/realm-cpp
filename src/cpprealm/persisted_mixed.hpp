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

#ifndef REALM_PERSISTED_MIXED_HPP
#define REALM_PERSISTED_MIXED_HPP

#include <cpprealm/persisted.hpp>

namespace realm {
    using mixed = std::variant<
            int64_t,
            bool,
            std::string,
            double,
            std::vector<uint8_t>,
            std::chrono::time_point<std::chrono::system_clock>,
            uuid>;

    template<typename T>
    struct persisted<T, type_info::MixedPersistable<T>> : public persisted_noncontainer_base<T> {
        using persisted_noncontainer_base<T>::persisted_noncontainer_base;
        using persisted_noncontainer_base<T>::operator*;
        using persisted_noncontainer_base<T>::operator=;
    };


    template <template <typename ...> typename Variant, typename ...Ts, typename V>
    std::enable_if_t<type_info::is_variant_t<Variant<Ts...>>::value, rbool>
    operator==(const persisted<Variant<Ts...>>& a, V&& b)
    {
        if (a.should_detect_usage_for_queries) {
            auto query = Query(a.query->get_table());
            query.equal(a.managed, type_info::persisted_type<std::decay_t<V>>::convert_if_required(b));
            return {std::move(query)};
        }
        return std::visit([&b](auto&& arg) {
            using M = std::decay_t<decltype(arg)>;
            if constexpr (std::is_convertible_v<M, V>) {
                return arg == b;
            } else {
                return false;
            }
        }, *a);
    }

    template <template <typename ...> typename Variant, typename ...Ts, typename V>
    std::enable_if_t<type_info::is_variant_t<Variant<Ts...>>::value, rbool>
    operator==(const persisted<Variant<Ts...>>& a, const V& b)
    {
        if (a.should_detect_usage_for_queries) {
            auto query = Query(a.query->get_table());
            query.equal(a.managed, type_info::persisted_type<V>::convert_if_required(b));
            return {std::move(query)};
        }
        return std::visit([&b](auto&& arg) {
            using M = std::decay_t<decltype(arg)>;
            if constexpr (std::is_convertible_v<M, V>) {
                return arg == b;
            } else {
                return false;
            }
        }, *a);
    }
}

#endif //REALM_PERSISTED_MIXED_HPP
