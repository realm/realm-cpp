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
#include <cpprealm/persisted.hpp>

#include <realm/object-store/object_schema.hpp>
#include <realm/object-store/shared_realm.hpp>

namespace realm {

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

// MARK: property

template <auto Ptr, bool IsPrimaryKey = false>
struct property {
    using Result = typename ptr_type_extractor<Ptr>::member_type::Result;
    using Class = typename ptr_type_extractor<Ptr>::class_type;
    const char* name = "";
    constexpr property()
            : type(type_info::persisted_type<Result>::property_type())
    {
    }
    constexpr property(const char* name)
    : type(type_info::persisted_type<Result>::property_type())
    , name(name)
    {
    }

    realm::Property to_property(const char* name) const {
        if constexpr (type_info::NonOptionalPersistableConcept<Result>::value
                      || type_info::EmbeddedObjectPersistableConcept<Result>::value
                      || type_info::OptionalPersistableConcept<Result>::value) {
            if constexpr (type_info::EmbeddedObjectPersistableConcept<Result>::value) {
                return realm::Property(name, type, Result::schema.name);
            } else if constexpr (type_info::is_optional<Result>::value) {
                if constexpr (type_info::ObjectBasePersistableConcept<typename Result::value_type>::value) {
                    return realm::Property(name, type, Result::value_type::schema.name);
                } else if constexpr (type_info::PrimitivePersistableConcept<typename Result::value_type>::value ||
                                    type_info::MixedPersistableConcept<typename Result::value_type>::value) {
                    return realm::Property(name, type);
                }
            } else {
                return realm::Property(name, type, is_primary_key);
            }
        } else if constexpr (type_info::ListPersistableConcept<Result>::value) {
            if constexpr (type_info::EmbeddedObjectPersistableConcept<Result>::value) {
                return realm::Property(name, type, Result::schema.name);
            } else if constexpr (type_info::ObjectBasePersistableConcept<typename Result::value_type>::value) {
                return realm::Property(name, type, Result::value_type::schema.name);
            } else {
                return realm::Property(name, type, is_primary_key);
            }
        } else {
            return realm::Property(name, type, is_primary_key);
        }
    }

    static void assign(Class& object, ColKey col_key, SharedRealm realm) {
        (object.*Ptr).assign(object.m_object->obj(), col_key, realm);
    }

    static void set(Class& object, const std::string& property_name) {
        if constexpr (type_info::PrimitivePersistableConcept<Result>::value
                        || type_info::MixedPersistableConcept<Result>::value) {
            object.m_object->template set_column_value(property_name, (object.*ptr).as_core_type());
        }
        else if constexpr (type_info::EmbeddedObjectPersistableConcept<Result>::value) {
            auto field = (object.*ptr);
            if (field.m_object) {
                object.m_object->set_column_value(property_name, field.m_object->obj().get_key());
            } else {
                auto col_key = object.m_object->obj().get_table()->get_column_key(property_name);
                auto target_table = object.m_object->obj().get_table()->get_link_target(col_key);
                auto target_cls = *field;

                auto realm = object.m_object->get_realm();
                target_cls.m_object = Object(realm, object.m_object->obj().create_and_set_linked_object(col_key));
                Result::schema.set(target_cls);
            }
        } else if constexpr (type_info::OptionalObjectPersistableConcept<Result>::value) {
            auto field = (object.*ptr);
            if (*field) {
                if (field.m_object) {
                    object.m_object->set_column_value(property_name, field.m_object->obj().get_key());
                } else {
                    auto col_key = object.m_object->obj().get_table()->get_column_key(property_name);
                    auto target_table = object.m_object->obj().get_table()->get_link_target(col_key);
                    auto target_cls = *field;

                    auto realm = object.m_object->get_realm();
                    if constexpr (decltype(Result::value_type::schema)::HasPrimaryKeyProperty) {
                        using Schema = decltype(Result::value_type::schema);
                        auto val = (**field).*Schema::PrimaryKeyProperty::ptr;
                        target_cls->m_object = Object(realm, target_table->create_object_with_primary_key(*val));
                    } else {
                        target_cls->m_object = Object(realm, target_table->create_object());
                    }
                    Result::value_type::schema.set(*target_cls);
                    object.m_object->obj().set(col_key, target_cls->m_object->obj().get_key());
                }
            }
        } else if constexpr (type_info::ListPersistableConcept<Result>::value) {
            auto col_key = object.m_object->obj().get_table()->get_column_key(property_name);
            if constexpr (type_info::ObjectBasePersistableConcept<typename Result::value_type>::value) {
                auto table = object.m_object->obj().get_table()->get_link_target(col_key);
                auto i = 0;
                for (auto& list_obj : (object.*ptr).unmanaged) {
                    if (table->is_embedded()) {
                        list_obj.m_object = Object(object.m_object->get_realm(),
                                                   object.m_object->obj().get_linklist(col_key).create_and_insert_linked_object(i));
                        Result::value_type::schema.set(list_obj);
                        i++;
                    } else {
                        Result::value_type::schema.add(list_obj, table, object.m_object->get_realm());
                        object.m_object->obj().get_linklist(col_key).add(list_obj.m_object->obj().get_key());
                    }
                }
            } else {
                object.m_object->obj().set_list_values(col_key, (object.*ptr).as_core_type());
            }
        } else if constexpr (type_info::OptionalPersistableConcept<Result>::value) {
            auto val = (object.*ptr).as_core_type();
            if (val) {
                object.m_object->template set_column_value(property_name, *val);
            }
        }
        else {
            object.m_object->template set_column_value(property_name, (object.*ptr).as_core_type());
        }
    }
//    static constexpr const char* name = Name;
    static constexpr persisted</*typename*/ Result/*::value*/> Class::*ptr = Ptr;
    PropertyType type;
    static constexpr bool is_primary_key = IsPrimaryKey;
};
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
namespace internal {
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

        explicit constexpr schema(const char *name_, Properties &&... props)
                : name(name_) {
            auto tup = std::make_tuple(props...);
            apply_name(tup);
        }

        template<size_t N, typename P>
        static constexpr auto primary_key(P &) {
//        static_assert(type_info::Propertyable<P>::value, "Type P is not a supported Property type");
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
        constexpr auto to_property(realm::ObjectSchema &schema, P &p) const {
            if constexpr (N + 1 == sizeof...(Properties)) {
                schema.persisted_properties.push_back(p.to_property(names[N]));
                return;
            } else {
                schema.persisted_properties.push_back(p.to_property(names[N]));
                return to_property<N + 1>(schema, std::get<N + 1>(properties));
            }
        }

        realm::ObjectSchema to_core_schema() const {
            realm::ObjectSchema schema;
            schema.name = name;
            to_property<0>(schema, std::get<0>(properties));
            if constexpr (HasPrimaryKeyProperty) {
                schema.primary_key = primary_key_name;
            }
            if constexpr (IsEmbedded) {
                schema.table_type = ObjectSchema::ObjectType::Embedded;
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
            set<0>(cls, std::get<0>(properties));
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
                property.assign(object, object.m_object->obj().get_table()->get_column_key(names[N]),
                                object.m_object->realm());
                return;
            } else {
                property.assign(object, object.m_object->obj().get_table()->get_column_key(names[N]),
                                object.m_object->realm());
                return assign<N + 1>(object, std::get<N + 1>(properties));
            }
        }

        Class create(Obj &&obj, const SharedRealm &realm) const {
            Class cls;
            cls.m_object = Object(realm, obj);
            assign<0>(cls, std::get<0>(properties));
            return cls;
        }

        std::unique_ptr<Class> create_unique(Obj &&obj, const SharedRealm &realm) const {
            auto cls = std::make_unique<Class>();
            cls->m_object = Object(realm, obj);
            assign<0>(*cls, std::get<0>(properties));
            return cls;
        }

        void add(Class &object, TableRef table, SharedRealm realm) const {
            if constexpr (HasPrimaryKeyProperty) {
                auto pk = *(object.*PrimaryKeyProperty::ptr);
                object.m_object = Object(realm, table->create_object_with_primary_key(pk));
            } else {
                object.m_object = Object(realm, table->create_object(ObjKey{}));
            }

            set(object);
            assign<0>(object, std::get<0>(properties));
        }
    };
}

template <typename ...T>
static constexpr auto schema(const char * name,
                                T&&... props) {
    auto tup = make_subpack_tuple(props...);
    auto i = std::get<0>(tup);
    using i2 = typename decltype(i)::Class;
    return internal::schema<i2, T...>(name, std::move(props)...);
}
}

#endif /* realm_schema_hpp */
