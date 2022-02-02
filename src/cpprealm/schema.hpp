////////////////////////////////////////////////////////////////////////////
//
// Copyright 2021 Realm Inc.
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
        if constexpr (type_info::OptionalPersistable<Result>) {
            if constexpr (type_info::ObjectPersistable<typename Result::value_type>) {
                return realm::Property(name, type, Result::value_type::schema::name);
            } else {
                return realm::Property(name, type);
            }
        } else if constexpr (type_info::ListPersistable<Result>) {
            if constexpr (type_info::ObjectPersistable<typename Result::value_type>) {
                return realm::Property(name, type, Result::value_type::schema::name);
            } else {
                return realm::Property(name, type, is_primary_key);
            }
        } else {
            return realm::Property(name, type, is_primary_key);
        }
    }

    static void assign(Class& object, ColKey col_key, SharedRealm realm) {
        if constexpr (type_info::ListPersistable<Result>) {
            (object.*Ptr).assign(*object.m_obj, col_key, realm);
        } else {
            (object.*Ptr).assign(*object.m_obj, col_key);
        }
    }

    static void set(Class& object, ColKey col_key) {
        object.m_obj->template set<typename type_info::persisted_type<Result>::type>(col_key,
                                                                                     (object.*ptr).as_core_type());
    }
    static void set(Class& object, ColKey col_key) requires (type_info::ListPersistable<Result>) {
        if constexpr (type_info::ObjectPersistable<typename Result::value_type>) {
            for (auto& list_obj : (object.*ptr).unmanaged) {
                auto table = object.m_obj->get_table()->get_link_target(col_key);
                Result::value_type::schema::add(list_obj, table, object.m_realm);
                object.m_obj->get_linklist(col_key).add(list_obj.m_obj->get_key());
            }
        } else {
            object.m_obj->set_list_values(col_key, (object.*ptr).as_core_type());
        }
    }
    static void set(Class& object, ColKey col_key) requires (type_info::OptionalObjectPersistable<Result>) {
        auto field = (object.*ptr);
        if (*field) {
            if (field.m_obj) {
                object.m_obj->set(col_key, field.m_obj->get_key());
            } else {
                auto target_table = object.m_obj->get_table()->get_link_target(col_key);
                auto target_cls = *field;

                Obj obj;
                if constexpr (Result::value_type::schema::HasPrimaryKeyProperty) {
                    obj = target_table->create_object_with_primary_key((**field).*Result::value_type::schema::PrimaryKeyProperty::ptr);
                } else {
                    obj = target_table->create_object();
                }
                target_cls->m_obj = obj;
                Result::value_type::schema::set(*target_cls);
                object.m_obj->set(col_key, obj.get_key());
            }
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

    static realm::ObjectSchema to_core_schema()
    {
        realm::ObjectSchema schema;
        schema.name = Name;
        (schema.persisted_properties.push_back(static_cast<realm::Property>(Properties())) , ...);
        if constexpr (HasPrimaryKeyProperty) {
            schema.primary_key = PrimaryKeyProperty::name;
        }
        return schema;
    }

    static void set(Class& cls)
    {
        (Properties::set(cls, cls.m_obj->get_table()->get_column_key(Properties::name)), ...);
    }

    static void initialize(Class& cls, Obj&& obj, SharedRealm realm)
    {
        cls.m_obj = std::move(obj);
        cls.m_realm = realm;

        (Properties::assign(cls, cls.m_obj->get_table()->get_column_key(Properties::name), realm), ...);
    }

    static Class create(Obj&& obj, SharedRealm realm)
    {
        Class cls;
        initialize(cls, std::move(obj), realm);
        return cls;
    }
    static Class* create_new(Obj&& obj, SharedRealm realm)
    {
        auto cls = new Class();
        initialize(*cls, std::move(obj), realm);
        return cls;
    }
    static std::unique_ptr<Class> create_unique(Obj&& obj, SharedRealm realm)
    {
        auto cls = std::make_unique<Class>();
        initialize(*cls, std::move(obj), realm);
        return cls;
    }

    static void add(Class& object, TableRef table, SharedRealm realm)
    {
        Obj managed;
        if constexpr (HasPrimaryKeyProperty) {
            auto pk = *(object.*PrimaryKeyProperty::ptr);
            managed = table->create_object_with_primary_key(pk);
        } else {
            managed = table->create_object(ObjKey{});
        }
        object.m_obj = managed; // This gets set twice?
        set(object);
        initialize(object, std::move(managed), realm);
    }
};

}

#endif /* realm_schema_hpp */
