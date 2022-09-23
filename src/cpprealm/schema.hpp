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

//#include <cpprealm/object.hpp>
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
using IsPrimaryKey = bool;
template <StringLiteral Name, auto Ptr, IsPrimaryKey IsPrimaryKey = false>
struct property {
    using Result = typename ptr_type_extractor<Ptr>::member_type::Result;
    using Class = typename ptr_type_extractor<Ptr>::class_type;

    constexpr property()
    : type(type_info::property_type<Result>())
    {
    }

    explicit operator property() {
        return property();
    }

    explicit operator realm::Property() const {
        if constexpr (type_info::OptionalPersistable<Result> || type_info::EmbeddedObjectPersistable<Result>) {
            if constexpr (type_info::EmbeddedObjectPersistable<Result>) {
                return realm::Property(name, type, Result::schema::name);
            } else if constexpr (type_info::ObjectBasePersistable<typename Result::value_type>) {
                return realm::Property(name, type, Result::value_type::schema::name);
            } else {
                return realm::Property(name, type);
            }
        } else if constexpr (type_info::ListPersistable<Result>) {
            if constexpr (type_info::ObjectBasePersistable<typename Result::value_type>) {
                return realm::Property(name, type, Result::value_type::schema::name);
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
        object.m_object->template set_column_value(property_name, (object.*ptr).as_core_type());
    }

    static void set(Class& object, const std::string& property_name) requires (type_info::ListPersistable<Result>) {
        auto col_key = object.m_object->obj().get_table()->get_column_key(property_name);
        if constexpr (type_info::ObjectBasePersistable<typename Result::value_type>) {
            auto table = object.m_object->obj().get_table()->get_link_target(col_key);
            for (auto& list_obj : (object.*ptr).unmanaged) {
                Result::value_type::schema::add(list_obj, table, object.m_object->get_realm());
                object.m_object->obj().get_linklist(col_key).add(list_obj.m_object->obj().get_key());
            }
        } else {
            object.m_object->obj().set_list_values(col_key, (object.*ptr).as_core_type());
        }
    }

    static void set(Class& object, const std::string& property_name) requires (type_info::OptionalObjectPersistable<Result>) {
        auto field = (object.*ptr);
        if (*field) {
            if (field.m_object) {
                object.m_object->set_column_value(property_name, field.m_object->obj().get_key());
            } else {
                auto col_key = object.m_object->obj().get_table()->get_column_key(property_name);
                auto target_table = object.m_object->obj().get_table()->get_link_target(col_key);
                auto target_cls = *field;

                auto realm = object.m_object->get_realm();
                if constexpr (Result::value_type::schema::HasPrimaryKeyProperty) {
                    target_cls->m_object = Object(realm, target_table->create_object_with_primary_key((**field).*Result::value_type::schema::PrimaryKeyProperty::ptr));
                } else {
                    target_cls->m_object = Object(realm, target_table->create_object());
                }
                Result::value_type::schema::set(*target_cls);
                object.m_object->obj().set(col_key, target_cls->m_object->obj().get_key());
            }
        }
    }
    static void set(Class& object, const std::string& property_name) requires (type_info::EmbeddedObjectPersistable<Result>) {
        auto field = (object.*ptr);
        if (field.m_object) {
            object.m_object->set_column_value(property_name, field.m_object->obj().get_key());
        } else {
            auto col_key = object.m_object->obj().get_table()->get_column_key(property_name);
            auto target_table = object.m_object->obj().get_table()->get_link_target(col_key);
            auto target_cls = *field;

            auto realm = object.m_object->get_realm();
            target_cls.m_object = Object(realm, object.m_object->obj().create_and_set_linked_object(col_key));
            Result::schema::set(target_cls);
//            object.*ptr = target_cls;
        }
    }
    static constexpr const char* name = Name.value;
    static constexpr persisted<Result> Class::*ptr = Ptr;
    PropertyType type;
    static constexpr bool is_primary_key = IsPrimaryKey;
};

// MARK: schema
template <StringLiteral Name, type_info::Propertyable ...Properties>
struct schema {
    using Class = typename std::tuple_element_t<0, std::tuple<Properties...>>::Class;
    static constexpr const char* name = Name.value;
    static constexpr const std::tuple<Properties...> properties{};

    template <size_t N, type_info::Propertyable P>
    static constexpr auto primary_key(P&)
    {
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
    static realm::ObjectSchema to_core_schema()
    {
        realm::ObjectSchema schema;
        schema.name = Name;
        (schema.persisted_properties.push_back(static_cast<realm::Property>(Properties())) , ...);
        if constexpr (HasPrimaryKeyProperty) {
            schema.primary_key = PrimaryKeyProperty::name;
        }
        if constexpr (IsEmbedded) {
            schema.table_type = ObjectSchema::ObjectType::Embedded;
        }
        return schema;
    }

    static void set(Class& cls)
    {
        (Properties::set(cls, Properties::name), ...);
    }

    static Class create(Obj&& obj, SharedRealm realm)
    {
        Class cls;
        cls.m_object = Object(realm, std::move(obj));
        (Properties::assign(cls, cls.m_object->obj().get_table()->get_column_key(Properties::name), realm), ...);
        return cls;
    }

    static std::unique_ptr<Class> create_unique(Obj&& obj, SharedRealm realm)
    {
        auto cls = std::make_unique<Class>();
        cls->m_object = Object(realm, std::move(obj));
        (Properties::assign(*cls, cls->m_object->obj().get_table()->get_column_key(Properties::name), realm), ...);
        return cls;
    }

    static void add(Class& object, TableRef table, SharedRealm realm)
    {
        if constexpr (HasPrimaryKeyProperty) {
            auto pk = *(object.*PrimaryKeyProperty::ptr);
            object.m_object = Object(realm, table->create_object_with_primary_key(pk));
        } else {
            object.m_object = Object(realm, table->create_object(ObjKey{}));
        }

        set(object);
        (Properties::assign(object, object.m_object->obj().get_table()->get_column_key(Properties::name), realm), ...);
    }
};

}

#endif /* realm_schema_hpp */
