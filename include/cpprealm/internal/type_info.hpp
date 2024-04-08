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

#ifndef CPPREALM_TYPE_INFO_HPP
#define CPPREALM_TYPE_INFO_HPP

#include <cpprealm/internal/bridge/property.hpp>
#include <cpprealm/internal/bridge/uuid.hpp>
#include <cpprealm/internal/bridge/binary.hpp>
#include <cpprealm/internal/bridge/timestamp.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/obj_key.hpp>
#include <cpprealm/internal/bridge/list.hpp>
#include <cpprealm/internal/bridge/dictionary.hpp>
#include <cpprealm/internal/bridge/object_id.hpp>
#include <cpprealm/internal/bridge/decimal128.hpp>
#include <cpprealm/internal/bridge/set.hpp>

#include <map>
#include <vector>

namespace realm {
    template <auto>
    struct linking_objects;
    template <typename>
    struct primary_key;
}
namespace realm::internal::type_info {
    template <typename T, typename = void>
    struct is_optional : std::false_type {
        using underlying = T;
    };
    template <template <typename> typename Optional, typename T>
    struct is_optional<Optional<T>,
                       std::enable_if_t<std::is_same_v<std::optional<T>, Optional<T>>>> : std::true_type {
        using underlying = T;
    };
    template <typename T, typename = void>
    struct is_vector : std::false_type {
        static constexpr auto value = false;
    };
    template <typename T>
    struct is_vector<std::vector<T>> : std::true_type {
        static constexpr auto value = true;
    };
    template <typename T, typename = void>
    struct is_set : std::false_type {
        static constexpr auto value = false;
    };
    template <typename T>
    struct is_set<std::set<T>> : std::true_type {
        static constexpr auto value = true;
    };
    template <typename T, typename = void>
    struct is_map : std::false_type {
        using value_type = T;
    };
    template <typename T>
    struct is_map<std::map<std::string, T>> : std::true_type {
        using value_type = T;
    };
    template <typename, typename>
    struct managed;
    template <typename T, typename = void>
    struct type_info;

    namespace {
        template <typename T>
        using is_primitive = std::negation<std::disjunction<
                std::is_same<typename type_info<T>::internal_type, std::optional<bridge::obj_key>>,
                std::is_same<typename type_info<T>::internal_type, bridge::obj_key>,
                std::is_same<typename type_info<T>::internal_type, bridge::list>,
                std::is_same<typename type_info<T>::internal_type, bridge::dictionary>
                >>;

        template <typename T, typename = void>
        struct is_variant_t : std::false_type {};
        template <template <typename ...> typename Variant, typename ...Ts>
        struct is_variant_t<Variant<Ts...>, std::enable_if_t<std::is_same_v<std::variant<Ts...>, Variant<Ts...>>>>
            : std::true_type {
        };

        template<size_t N, typename Variant>
        constexpr bool check_variant_types() {
            if constexpr (!is_variant_t<Variant>::value) {
                return false;
            } else if constexpr (N >= std::variant_size_v<Variant>) {
                return true;
            } else {
                if constexpr (std::is_same_v<std::variant_alternative_t<N, Variant>, std::monostate>) {
                    return check_variant_types<N + 1, Variant>();
                } else if constexpr (is_primitive<std::variant_alternative_t<N, Variant>>::value) {
                    return check_variant_types<N + 1, Variant>();
                } else {
                    return false;
                }
            }
        }

        template<typename T>
        using MixedPersistableConcept =
                std::conjunction<is_variant_t<T>,
                                 std::conditional_t<check_variant_types<0, T>(), std::true_type, std::false_type>
                                 >;

        namespace {
            static_assert(std::conjunction<
                          std::is_convertible<int, int64_t>,
                          std::is_constructible<int, int64_t>>::value);
            static_assert(std::conjunction<
                          std::is_convertible<const char*, std::string>,
                          std::is_constructible<std::string, const char*>>::value);
        }
    }
    template <typename T>
    struct type_info<T*> {
        using internal_type = bridge::obj_key;
        static constexpr bridge::property::type type() {
            return bridge::property::type::Object;
        }
    };
    template <auto T>
    struct type_info<linking_objects<T>> {
        using internal_type = bridge::obj_key;
        static constexpr bridge::property::type type() {
            return bridge::property::type::LinkingObjects | bridge::property::type::Array;
        }
    };
    template <typename T>
    struct is_link : std::false_type {
        static constexpr auto value = false;
        static constexpr auto is_managed = false;
    };
    template <typename T>
    struct is_link<managed<T*, void>> : std::true_type {
        static constexpr auto value = true;
        static constexpr auto is_managed = true;
    };
    template <typename T>
    struct is_link<T*> : std::true_type {
        static constexpr auto value = true;
        static constexpr auto is_managed = false;
    };

    template <typename T>
    struct is_backlink : std::false_type {
        static constexpr auto value = false;
    };
    template <auto T>
    struct is_backlink<linking_objects<T>> : std::true_type {
        static constexpr auto value = true;
    };
    template <>
    struct type_info<std::monostate> {
        using internal_type = std::monostate;
        static constexpr bridge::property::type type() {
            return bridge::property::type::Mixed;
        }
    };
    template <>
    struct type_info<std::string> {
        using internal_type = std::string;
        static constexpr bridge::property::type type() {
            return bridge::property::type::String;
        }
    };
    template <>
    struct type_info<const char*> {
        using internal_type = std::string;
        static constexpr bridge::property::type type() {
            return bridge::property::type::String;
        }
    };
    template <>
    struct type_info<int64_t> {
        using internal_type = int64_t;
        static constexpr bridge::property::type type() {
            return bridge::property::type::Int;
        }
    };
    template <>
    struct type_info<int> {
        using internal_type = int64_t;
        static constexpr bridge::property::type type() {
            return bridge::property::type::Int;
        }
    };
    template <>
    struct type_info<double> {
        using internal_type = double;
        static constexpr bridge::property::type type() {
            return bridge::property::type::Double;
        }
    };
    template <>
    struct type_info<bool> {
        using internal_type = bool;
        static constexpr bridge::property::type type() {
            return bridge::property::type::Bool;
        }
    };
    template <>
    struct type_info<uuid> {
        using internal_type = bridge::uuid;
        static constexpr bridge::property::type type() {
            return bridge::property::type::UUID;
        }
    };
    template <>
    struct type_info<object_id> {
        using internal_type = bridge::object_id;
        static constexpr bridge::property::type type() {
            return bridge::property::type::ObjectId;
        }
    };
    template <>
    struct type_info<decimal128> {
        using internal_type = bridge::decimal128;
        static constexpr bridge::property::type type() {
            return bridge::property::type::Decimal;
        }
    };
    template <>
    struct type_info<std::vector<uint8_t>> {
        using internal_type = bridge::binary;
        static constexpr bridge::property::type type() {
            return bridge::property::type::Data;
        }
    };
    template <typename E>
    struct type_info<E, std::enable_if_t<std::is_enum_v<E>>> {
        using internal_type = int64_t;
        static constexpr bridge::property::type type() {
            return bridge::property::type::Int;
        }
    };
    template <typename ValueType>
    struct type_info<std::map<std::string, ValueType>> {
        using internal_type = internal::bridge::dictionary;
        static constexpr bridge::property::type type() {
            return bridge::property::type::Dictionary | type_info<ValueType>::type();
        }
    };
    template <typename ValueType>
    struct type_info<std::set<ValueType>> {
        using internal_type = bridge::set;
        static constexpr bridge::property::type type() {
            return bridge::property::type::Set | type_info<ValueType>::type();
        }
    };
    template <typename T>
    struct type_info<T, std::enable_if_t<MixedPersistableConcept<T>::value>> {
        using internal_type = bridge::mixed;

        static constexpr auto type() {
            return bridge::property::type::Mixed | bridge::property::type::Nullable;
        }
    };
    template <typename T>
    struct type_info<std::optional<T>> {
        using internal_type = std::optional<typename type_info<T>::internal_type>;
        static constexpr auto type() {
            return type_info<T>::type() | bridge::property::type::Nullable;
        }
    };
    template <typename C, typename D>
    struct type_info<std::chrono::time_point<C, D>> {
        using internal_type = bridge::timestamp;
        static constexpr auto type() {
            return bridge::property::type::Date;
        }
    };
    template <typename V>
    struct type_info<std::vector<V>, std::enable_if_t<std::negation_v<std::is_same<V, uint8_t>>>> {
        using internal_type = bridge::list;
        static constexpr auto type() {
            return type_info<V>::type() | bridge::property::type::Array;
        }
    };

    template <typename T>
    struct is_primary_key : std::false_type {
        static constexpr auto value = false;
    };
    template <typename T>
    struct is_primary_key<primary_key<T>> : std::true_type {
        static constexpr auto value = true;
    };

    template <typename T>
    struct type_info<primary_key<T>, void> {
        using internal_type = typename type_info<T>::internal_type;
        static constexpr bridge::property::type type() {
            return type_info<T>::type();
        }
    };

}
#endif //CPPREALM_TYPE_INFO_HPP