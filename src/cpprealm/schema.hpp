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
#include <cpprealm/experimental/link.hpp>

#include <type_traits>
#include <iostream>

namespace realm {
    template <typename, typename>
    struct persisted;
    template<typename>
    struct asymmetric_object;


    namespace experimental {
        enum class BetaObjectType {
            None,
            TopLevel,
            Embedded,
            Asymmetric
        };
    }
    namespace internal {
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
            using Result = typename internal::persisted_type_extractor<typename internal::ptr_type_extractor<Ptr>::member_type>::Result;
            using VariantResult =
                    std::conditional_t<std::is_pointer_v<Result>, experimental::managed<Result>, Result>;

            using Class = typename internal::ptr_type_extractor<Ptr>::class_type;
            static constexpr auto ptr = Ptr;
            static constexpr bool is_primary_key = IsPrimaryKey || internal::type_info::is_experimental_primary_key<Result>::value;
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
                    } else if constexpr (std::is_pointer_v<typename Result::value_type>) {
                        property.set_object_link(
                                experimental::managed<std::remove_pointer_t<typename Result::value_type>, void>::schema.name);
                    }
                } else if constexpr (realm::internal::type_info::is_map<Result>::value) {
                    if constexpr (internal::type_info::is_optional<typename Result::mapped_type>::value) {
                        if constexpr (std::is_base_of_v<object_base<typename Result::mapped_type::value_type>, typename Result::mapped_type::value_type>) {
                            property.set_object_link(Result::mapped_type::value_type::schema.name);
                            property.set_type(type | internal::bridge::property::type::Nullable);
                        } else if constexpr (internal::type_info::is_link<typename Result::mapped_type::value_type>::value) {
                            property.set_object_link(experimental::managed<std::remove_pointer_t<typename Result::mapped_type::value_type>, void>::schema.name);
                            property.set_type(type | internal::bridge::property::type::Nullable);
                        }
                    } else if constexpr (std::is_pointer_v<typename Result::mapped_type>) {
                        property.set_object_link(
                                experimental::managed<std::remove_pointer_t<typename Result::mapped_type>, void>::schema.name);
                        property.set_type(type | internal::bridge::property::type::Nullable);
                    }
                } else if constexpr (internal::type_info::is_optional<Result>::value) {
                    if constexpr (std::is_base_of_v<object_base<typename Result::value_type>, typename Result::value_type>) {
                        property.set_object_link(Result::value_type::schema.name);
                        property.set_type(type | internal::bridge::property::type::Nullable);
                    }
                }
                else if constexpr (std::is_pointer_v<Result>) {
                    property.set_object_link(experimental::managed<typename std::remove_pointer_t<Result>, void>::schema.name);
                    property.set_type(type | internal::bridge::property::type::Nullable);
                } else if constexpr (internal::type_info::is_backlink<Result>::value) {
                    return internal::bridge::property{};
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
            using variant_t = typename unique_variant<std::variant<>, std::monostate, typename Properties::VariantResult...>::type;

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
            explicit constexpr schema(const char *name_, experimental::BetaObjectType beta_object_type, std::tuple<Properties...>&& props)
                : name(name_)
                  , ps(props), m_beta_object_type(beta_object_type) {
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
            static constexpr bool IsAsymmetric = std::is_base_of_v<asymmetric_object<Class>, Class>;

            bool is_embedded_experimental() const {
                return m_beta_object_type == experimental::BetaObjectType::Embedded;
            }

            [[nodiscard]] internal::bridge::object_schema to_core_schema() const {
                internal::bridge::object_schema schema;
                schema.set_name(name);

                auto add_property = [&](const internal::bridge::property &p) {
                    if (!p.name().empty()) {
                        schema.add_property(p);
                    }
                };
                std::apply([&](const auto&... p) {
                    (add_property(p), ...);
                }, ps);

                if constexpr (HasPrimaryKeyProperty) {
                    schema.set_primary_key(primary_key_name);
                }
                if constexpr (IsEmbedded) {
                    schema.set_object_type(internal::bridge::object_schema::object_type::Embedded);
                }
                if constexpr (IsAsymmetric) {
                    schema.set_object_type(internal::bridge::object_schema::object_type::TopLevelAsymmetric);
                }
                if (m_beta_object_type == experimental::BetaObjectType::Embedded) {
                    schema.set_object_type(internal::bridge::object_schema::object_type::Embedded);
                }
                if (m_beta_object_type == experimental::BetaObjectType::Asymmetric) {
                    schema.set_object_type(internal::bridge::object_schema::object_type::TopLevelAsymmetric);
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
            template<size_t N, typename P>
            constexpr variant_t
            property_value_for_name(std::string_view property_name, const experimental::managed<Class, void> &cls, P &property, bool excluding_collections = true) const {
                if (excluding_collections &&
                    (property.type == realm::internal::bridge::property::type::Array || property.type == realm::internal::bridge::property::type::Dictionary)) {
                    return variant_t{std::monostate()};
                }

                if constexpr (N + 1 == sizeof...(Properties)) {
                    if (property_name == std::string_view(names[N])) {
                        auto ptr = experimental::managed<Class, void>::template unmanaged_to_managed_pointer(property.ptr);
                        return (cls.*ptr).value();
                    }
                    return variant_t{};
                } else {
                    if (property_name == std::string_view(names[N])) {
                        auto ptr = experimental::managed<Class, void>::template unmanaged_to_managed_pointer(property.ptr);
                        return (cls.*ptr).value();
                    }
                    return property_value_for_name<N + 1>(property_name, cls, std::get<N + 1>(properties));
                }
            }
            constexpr auto property_value_for_name(std::string_view property_name, const experimental::managed<Class, void> &cls, bool excluding_collections = true) const {
                return property_value_for_name<0>(property_name, cls, std::get<0>(properties), excluding_collections);
            }
            constexpr auto property_value_for_name(std::string_view property_name, const Class &cls) const {
                return property_value_for_name<0>(property_name, cls, std::get<0>(properties));
            }

            template<size_t N, typename T, typename P>
            constexpr const char*
            name_for_property(T ptr, P &property) const {
                if constexpr (N + 1 == sizeof...(Properties)) {
                    if constexpr (std::is_same_v<decltype(ptr), std::remove_const_t<decltype(property.ptr)>>) {
                        if (ptr == property.ptr) {
                            return property.name;
                        }
                    }
                    return "";
                } else {
                    if constexpr (std::is_same_v<decltype(ptr), std::remove_const_t<decltype(property.ptr)>>) {
                        if (ptr == property.ptr) {
                            return property.name;
                        }
                    }
                    return name_for_property<N + 1>(ptr, std::get<N + 1>(ps));
                }
            }
            template <auto ptr>
            constexpr const char* name_for_property() const {
                return name_for_property<0>(ptr, std::get<0>(ps));
            }
            template <typename T>
            constexpr const char* name_for_property(T ptr) const {
                return name_for_property<0>(ptr, std::get<0>(ps));
            }
        private:
            experimental::BetaObjectType m_beta_object_type = experimental::BetaObjectType::None;
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
        auto tup = internal::make_subpack_tuple(props...);
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

    template <typename ...T>
    static constexpr auto schema(const char * name,
                                 experimental::BetaObjectType object_type,
                                 std::tuple<T...>&& props) {
        auto i = std::get<0>(props);
        using Cls = typename decltype(i)::Class;
        return schemagen::schema<Cls, T...>(name, object_type, std::move(props));
    }
}

#endif /* realm_schema_hpp */
