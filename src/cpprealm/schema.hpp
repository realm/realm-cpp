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

#ifndef realm_schema_hpp
#define realm_schema_hpp

#include <variant>
#include <cpprealm/internal/bridge/lnklst.hpp>
#include <cpprealm/internal/bridge/object_schema.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/type_info.hpp>
#include "cpprealm/experimental/link.hpp"

#include <type_traits>

namespace realm {
    template <typename, typename>
    struct persisted;
    namespace {
        template <typename T>
        struct ptr_type_extractor_base;
        template <typename Result, typename Class>
        struct ptr_type_extractor_base<Result Class::*>
        {
            using class_type = Class;
            using member_type = Result;
        };

        template <auto T>
        struct ptr_type_extractor : ptr_type_extractor_base<decltype(T)> {
        };

        template<typename... Ts>
        constexpr auto make_subpack_tuple(Ts&&... xs)
        {
            return std::tuple<Ts...>(std::forward<Ts>(xs)...);
        }

        template <typename T>
        struct persisted_type_extractor {
            using Result = T;
        };
        template <typename T>
        struct persisted_type_extractor<realm::persisted<T, void>> {
            using Result = T;
        };
    }


// MARK: schema
    namespace schemagen {
        template <auto Ptr, bool IsPrimaryKey = false>
        struct property {
            using Result = typename persisted_type_extractor<typename ptr_type_extractor<Ptr>::member_type>::Result;
            using Class = typename ptr_type_extractor<Ptr>::class_type;
            static constexpr auto Class::*ptr = Ptr;
            static constexpr bool is_primary_key = IsPrimaryKey;
            internal::bridge::property::type type;
            const char* name = "";

            constexpr property() : type(internal::type_info::type_info<Result>::type())
            {
            }
            explicit constexpr property(const char* actual_name)
                : type(internal::type_info::type_info<Result>::type())
            {
                name = actual_name;
            }

            operator internal::bridge::property() const {
                internal::bridge::property property(name, type, is_primary_key);
                if constexpr (std::is_base_of_v<object_base<Result>, Result>) {
                    property.set_object_link(Result::schema.name);
                    property.set_type(type | internal::bridge::property::type::Nullable);
                } else if constexpr (realm::internal::type_info::is_vector<Result>::value) {
                    if constexpr (std::is_base_of_v<object_base<typename Result::value_type>,
                                                    typename Result::value_type>) {
                        property.set_object_link(Result::value_type::schema.name);
                    } else if constexpr (internal::type_info::is_link<typename Result::value_type>::value) {
                        property.set_object_link(
                                experimental::managed<typename Result::value_type::value_type, void>::schema.name);
                    }
                } else if constexpr (realm::internal::type_info::is_map<Result>::value) {
                    if constexpr (internal::type_info::is_optional<typename Result::mapped_type>::value) {
                        if constexpr (std::is_base_of_v<object_base<typename Result::mapped_type::value_type>, typename Result::mapped_type::value_type>) {
                            property.set_object_link(Result::mapped_type::value_type::schema.name);
                            property.set_type(type | internal::bridge::property::type::Nullable);
                        } else if constexpr (internal::type_info::is_link<typename Result::mapped_type::value_type>::value) {
                            property.set_object_link(experimental::managed<typename Result::mapped_type::value_type::value_type, void>::schema.name);
                            property.set_type(type | internal::bridge::property::type::Nullable);
                        }
                    }
                } else if constexpr (internal::type_info::is_optional<Result>::value) {
                    if constexpr (std::is_base_of_v<object_base<typename Result::value_type>, typename Result::value_type>) {
                        property.set_object_link(Result::value_type::schema.name);
                        property.set_type(type | internal::bridge::property::type::Nullable);
                    }
                }
                else if constexpr (internal::type_info::is_link<Result>::value) {
                    property.set_object_link(experimental::managed<typename Result::value_type, void>::schema.name);
                    property.set_type(type | internal::bridge::property::type::Nullable);
                }
                return property;
            }
        };

        template <typename T, typename ... Types>
        struct unique_variant {
            using type = T;
        };

        template <template<typename...> typename Variant, typename... VariantTypes, typename NextType, typename... RemainingTypes>
        struct unique_variant<Variant<VariantTypes...> , NextType, RemainingTypes...>
            : std::conditional<
                      std::disjunction< std::is_same<NextType, VariantTypes> ... >::value
                      , unique_variant<Variant<VariantTypes...>, RemainingTypes...>
                      , unique_variant<Variant<VariantTypes..., NextType>, RemainingTypes...>
                      >::type
        {};

        template <typename Class, typename ...Properties>
        struct schema {
            const char *name;
            const char *names[sizeof...(Properties)] = {};
            const char *primary_key_name = "";

            static constexpr std::tuple<Properties...> properties{};
            using variant_t = typename unique_variant<std::variant<>, typename Properties::Result...>::type;

            template<size_t N, typename P>
            constexpr auto apply_name(const std::tuple<Properties...> &tup, P &&prop) {
                if constexpr (N + 1 == sizeof...(Properties)) {
                    names[N] = std::get<N>(tup).name;
                    if (std::get<N>(tup).is_primary_key) {
                        primary_key_name = std::get<N>(tup).name;
                    }
                    return;
                } else {
                    names[N] = std::get<N>(tup).name;
                    if (std::get<N>(tup).is_primary_key) {
                        primary_key_name = std::get<N>(tup).name;
                    }
                    return apply_name<N + 1>(tup, std::get<N + 1>(tup));
                }
            }

            constexpr auto apply_name(const std::tuple<Properties...> &tup) {
                return apply_name<0>(tup, std::get<0>(tup));
            }

            std::tuple<Properties...> ps;

            explicit constexpr schema(const char *name_, Properties &&... props)
                : name(name_)
                  , ps(props...) {
                auto tup = std::make_tuple(props...);
                apply_name(tup);
            }
            explicit constexpr schema(const char *name_, std::tuple<Properties...>&& props)
                : name(name_)
                  , ps(props) {
                apply_name(props);
            }
            template<size_t N, typename P>
            static constexpr auto primary_key(P &) {
                if constexpr (P::is_primary_key) {
                    return P();
                } else {
                    if constexpr (N + 1 == sizeof...(Properties)) {
                        return;
                    } else {
                        return primary_key<N + 1>(std::get<N + 1>(properties));
                    }
                }
            }

            static constexpr auto primary_key() {
                return primary_key<0>(std::get<0>(properties));
            }

            using PrimaryKeyProperty = decltype(primary_key());
            static constexpr bool HasPrimaryKeyProperty = !std::is_void_v<PrimaryKeyProperty>;
            static constexpr bool IsEmbedded = std::is_base_of_v<embedded_object<Class>, Class>;

            [[nodiscard]] internal::bridge::object_schema to_core_schema() const {
                internal::bridge::object_schema schema;
                schema.set_name(name);
                std::apply([&schema](auto&&... p) {
                    (schema.add_property(p), ...);
                }, ps);

                if constexpr (HasPrimaryKeyProperty) {
                    schema.set_primary_key(primary_key_name);
                }
                if constexpr (IsEmbedded) {
                    schema.set_object_type(internal::bridge::object_schema::object_type::Embedded);
                }
                return schema;
            }

            template<size_t N, typename P>
            constexpr auto set(Class &object, P &property) const {
                if constexpr (N + 1 == sizeof...(Properties)) {
                    property.set(object, names[N]);
                    return;
                } else {
                    property.set(object, names[N]);
                    return set<N + 1>(object, std::get<N + 1>(properties));
                }
            }

            template<size_t N, typename P>
            constexpr variant_t
            property_value_for_name(std::string_view property_name, const Class &cls, P &property) const {
                if constexpr (N + 1 == sizeof...(Properties)) {
                    if (property_name == std::string_view(names[N])) {
                        return *(cls.*property.ptr);
                    }
                    return variant_t{};
                } else {
                    if (property_name == std::string_view(names[N])) {
                        return *(cls.*property.ptr);
                    }
                    return property_value_for_name<N + 1>(property_name, cls, std::get<N + 1>(properties));
                }
            }

            constexpr auto property_value_for_name(std::string_view property_name, const Class &cls) const {
                return property_value_for_name<0>(property_name, cls, std::get<0>(properties));
            }
        };
    }
template <auto Ptr, bool IsPrimaryKey = false>
    static constexpr auto property(const char* name)
    {
        return schemagen::property<Ptr, IsPrimaryKey>(name);
    }

    template <typename ...T>
    static constexpr auto schema(const char * name,
                                 T&&... props) {
        auto tup = make_subpack_tuple(props...);
        auto i = std::get<0>(tup);
        using Cls = typename decltype(i)::Class;
        return schemagen::schema<Cls, T...>(name, std::move(props)...);
    }

    template <typename ...T>
    static constexpr auto schema(const char * name,
                                 std::tuple<T...>&& props) {
        auto i = std::get<0>(props);
        using Cls = typename decltype(i)::Class;
        return schemagen::schema<Cls, T...>(name, std::move(props));
    }
}

#endif /* realm_schema_hpp */
