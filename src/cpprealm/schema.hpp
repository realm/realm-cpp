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
}

namespace {
    constexpr std::size_t split(char const *str, char sep) {
        std::size_t ret{1u};

        while (*str)
            if (sep == *str++)
                ++ret;

        return ret;
    }
}


    template<typename... Ts>
    constexpr auto make_subpack_tuple(Ts&&... xs)
    {
        return std::tuple<Ts...>(std::forward<Ts>(xs)...);
    }

// MARK: schema
namespace schemagen {
    template <auto Ptr, typename Class, typename Result, typename = void>
    struct property_deducer {
        static void set_object_link(internal::bridge::property& property) {}
        static void manage(Class& object, const std::string& property_name) {
            auto persisted = *(object.*Ptr);
            object.m_object->template set_column_value(property_name, internal::type_info::serialize<Result>(persisted));
        }
    };
    template <auto Ptr, typename Class, typename ValueType>
    struct property_deducer<Ptr, Class, std::optional<ValueType>, std::enable_if_t<std::negation_v<std::is_base_of<object_base, ValueType>>>> {
        static void set_object_link(internal::bridge::property& property) {}
        static void manage(Class& object, const std::string& property_name) {
            auto val = internal::type_info::serialize<std::optional<ValueType>>(*(object.*Ptr));
            if (val) {
                object.m_object->template set_column_value(property_name, *val);
            }
        }
    };
    template <auto Ptr, typename Class, typename ValueType>
    struct property_deducer<Ptr, Class, std::optional<ValueType>, std::enable_if_t<std::is_base_of_v<object, ValueType>>> {
        static void set_object_link(internal::bridge::property& property) {
            property.set_object_link(ValueType::schema.name);
        }
        static void manage(Class& object, const std::string& property_name) {
            persisted<std::optional<ValueType>, void> field = (object.*Ptr);
            if (field) {
                if (field.m_object) {
                    object.m_object->set_column_value(property_name, field.m_object->obj().get_key());
                } else {
                    auto col_key = object.m_object->obj().get_table().get_column_key(property_name);
                    auto target_table = object.m_object->obj().get_table().get_link_target(col_key);
                    ValueType target_cls = *field;

                    auto realm = object.m_object->get_realm();
                    if constexpr (decltype(ValueType::schema)::HasPrimaryKeyProperty) {
                        using Schema = decltype(ValueType::schema);
                        auto val = (*field).*Schema::PrimaryKeyProperty::ptr;
                        target_cls.m_object = internal::bridge::object(realm, target_table.create_object_with_primary_key(*val));
                    } else {
                        target_cls.m_object = internal::bridge::object(realm, target_table.create_object());
                    }
                    ValueType::schema.set(target_cls);
                    object.m_object->obj().set(col_key, target_cls.m_object->obj().get_key());
                }
            }
        }
    };
    template <auto Ptr, typename Class, typename ValueType>
    struct property_deducer<Ptr, Class, std::optional<ValueType>, std::enable_if_t<std::is_base_of_v<embedded_object, ValueType>>> {
        static void set_object_link(internal::bridge::property& property) {
            property.set_object_link(ValueType::schema.name);
        }
        static void manage(Class& object, const std::string& property_name) {
            auto field = (object.*Ptr);
            if (field.m_object) {
                object.m_object->set_column_value(property_name, field.m_object->obj().get_key());
            } else {
                auto col_key = object.m_object->obj().get_table().get_column_key(property_name);
                auto target_table = object.m_object->obj().get_table().get_link_target(col_key);
                auto target_cls = *field;

                auto realm = object.m_object->get_realm();
                target_cls.m_object = internal::bridge::object(realm,
                                                               object.m_object->obj().create_and_set_linked_object(col_key));
                ValueType::schema.set(target_cls);
            }
        }
    };
    template <auto Ptr, typename Class, typename ValueType>
    struct property_deducer<Ptr, Class, std::vector<ValueType>,
            std::enable_if_t<std::negation<std::is_base_of<object_base, ValueType>>::value>> {
        static void set_object_link(internal::bridge::property& property) {
        }
        static void manage(Class& object, const std::string& property_name) {
            auto col_key = object.m_object->obj().get_table().get_column_key(property_name);
            if constexpr (std::is_same_v<uint8_t, ValueType>) {

            } else if constexpr (std::is_same_v<std::vector<uint8_t>, ValueType>) {

            } else {
                std::vector<typename internal::type_info::type_info<ValueType>::internal_type> v;
                for (const auto& e : object.*Ptr) {
                    v.push_back(internal::type_info::serialize(e));
                }
                object.m_object->obj().set_list_values(col_key, v);
            }
        }
    };
    template <auto Ptr, typename Class, typename ValueType>
    struct property_deducer<Ptr, Class, std::vector<ValueType>, std::enable_if_t<std::disjunction_v<std::is_base_of<object, ValueType>, std::is_base_of<embedded_object, ValueType>>>> {
        static void set_object_link(internal::bridge::property& property) {
            property.set_object_link(ValueType::schema.name);
        }
        static void manage(Class& object, const std::string& property_name) {
            auto col_key = object.m_object->obj().get_table().get_column_key(property_name);
            auto table = object.m_object->obj().get_table().get_link_target(col_key);
            auto i = 0;
            for (auto& list_obj : (object.*Ptr).unmanaged) {
                if (table.is_embedded()) {
                    list_obj.m_object = internal::bridge::object(object.m_object->get_realm(),
                                                                object.m_object->obj().get_linklist(col_key).create_and_insert_linked_object(i));
                    ValueType::schema.set(list_obj);
                    i++;
                } else {
                    ValueType::schema.add(list_obj, table, object.m_object->get_realm());
                    object.m_object->obj().get_linklist(col_key).add(list_obj.m_object->obj().get_key());
                }
            }
        }
    };
    template <auto Ptr, typename Class, typename ValueType>
    struct property_deducer<Ptr, Class, std::map<std::string, ValueType>> {
        static void set_object_link(internal::bridge::property& property) {
            if constexpr (std::is_base_of_v<object_base, ValueType>)
                property.set_object_link(ValueType::schema.name);
        }
        static void manage(Class& object, const std::string& property_name) {
            auto val = (object.*Ptr);
            auto dictionary = object.m_object->obj().get_dictionary(property_name);
            for (auto [k, v] : val.unmanaged) {
                internal::bridge::mixed mixed = internal::type_info::serialize(v);
                dictionary.insert(k, mixed);
            }
        }
    };

    template <auto Ptr, bool IsPrimaryKey = false>
    struct property {
        using Result = typename ptr_type_extractor<Ptr>::member_type::Result;
        using Class = typename ptr_type_extractor<Ptr>::class_type;
//        using Ptr = decltype(Ptr);

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
            if constexpr (realm::internal::type_info::is_optional<Result>::value) {
                if constexpr (std::is_base_of_v<object_base, typename Result::value_type>) {
                    property.set_object_link(Result::value_type::schema.name);
                }
            } else if constexpr (realm::internal::type_info::is_vector<Result>::value) {
                if constexpr (std::is_base_of_v<object_base, typename Result::value_type>) {
                    property.set_object_link(Result::value_type::schema.name);
                }
            }
            return property;
        }

//        static void assign(Class& object, internal::bridge::col_key col_key, internal::bridge::realm realm) {
//            (object.*Ptr).manage(object.m_object->obj(), col_key, realm);
//        }
//
//        static void set(Class& object, const std::string& property_name) {
//            property_deducer<Ptr, Class, Result>::manage(object, property_name);
//        }
        static constexpr auto Class::*ptr = Ptr;
        internal::bridge::property::type type;
        static constexpr bool is_primary_key = IsPrimaryKey;
    };


    template <auto Ptr, bool IsPrimaryKey>
    internal::bridge::property to_property(schemagen::property<Ptr, IsPrimaryKey> persisted_property,
                                           const char* name);

    template<typename Class, typename ...Properties>
    struct schema {
        const char *name;
        const char *names[sizeof...(Properties)] = {};
        const char *primary_key_name = "";

        static constexpr std::tuple<Properties...> properties{};

        using variant_t = typename std::variant<typename Properties::Result...>;

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
        static constexpr bool IsEmbedded = std::is_base_of_v<embedded_object, Class>;

        template<size_t N, typename P>
        constexpr auto to_property(internal::bridge::object_schema &schema, P &p) const {
            if constexpr (N + 1 == sizeof...(Properties)) {
                schema.add_property(p.to_property(names[N]));
                return;
            } else {
                schema.add_property(p.to_property(names[N]));
                return to_property<N + 1>(schema, std::get<N + 1>(properties));
            }
        }

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

        void set(Class &cls) const {
            std::apply([&cls](auto&&... p) {
                ((cls.*(std::decay_t<decltype(p)>::ptr)).manage(&*cls.m_object, cls.m_object->obj().get_table().get_column_key(p.name)), ...);
            }, ps);
        }

        template<size_t N, typename P>
        constexpr variant_t
        property_value_for_name(std::string_view property_name, const Class &cls, P &property) const {
            if constexpr (N + 1 == sizeof...(Properties)) {
                if (property_name == std::string_view(names[N])) {
                    return variant_t { std::in_place_index<N>, *(cls.*property.ptr) };
                }
                return variant_t{};
            } else {
                return property_value_for_name<N + 1>(property_name, cls, std::get<N + 1>(properties));
            }
        }

        constexpr auto property_value_for_name(std::string_view property_name, const Class &cls) const {
            return property_value_for_name<0>(property_name, cls, std::get<0>(properties));
        }

        template<size_t N, typename P>
        constexpr auto assign(Class &object, P &property) const {
            if constexpr (N + 1 == sizeof...(Properties)) {
                property.assign(object, object.m_object->obj().get_table().get_column_key(names[N]),
                                object.m_object->get_realm());
                return;
            } else {
                property.assign(object, object.m_object->obj().get_table().get_column_key(names[N]),
                                object.m_object->get_realm());
                return assign<N + 1>(object, std::get<N + 1>(properties));
            }
        }

        Class create(internal::bridge::obj &&obj, const internal::bridge::realm &realm) const {
            Class object;
            object.m_object = internal::bridge::object(realm, obj);

            std::apply([&object](auto&&... p) {
                ((object.*(std::decay_t<decltype(p)>::ptr))
                        .manage(&*object.m_object, object.m_object->obj().get_table().get_column_key(p.name)), ...);
            }, ps);
            return object;
        }

        std::unique_ptr<Class> create_unique(internal::bridge::obj &&obj, const internal::bridge::realm &realm) const {
            auto cls = std::make_unique<Class>();
            cls->m_object = internal::bridge::object(realm, obj);
            assign<0>(*cls, std::get<0>(properties));
            return cls;
        }

        void add(Class &object, internal::bridge::table table, internal::bridge::realm realm) const {
            if constexpr (HasPrimaryKeyProperty) {
                auto pk = *(object.*PrimaryKeyProperty::ptr);
                object.m_object = internal::bridge::object(realm, table.create_object_with_primary_key(pk));
            } else {
                object.m_object = internal::bridge::object(realm, table.create_object());
            }

            std::apply([&object](auto&&... p) {
                ((object.*(std::decay_t<decltype(p)>::ptr))
                    .manage(&*object.m_object, object.m_object->obj().get_table().get_column_key(p.name)), ...);
            }, ps);
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
    using i2 = typename decltype(i)::Class;
    return schemagen::schema<i2, T...>(name, std::move(props)...);
}

}

#endif /* realm_schema_hpp */
