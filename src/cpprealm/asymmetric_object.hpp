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

#ifndef realm_asymmetric_object_hpp
#define realm_asymmetric_object_hpp

#include <cpprealm/object.hpp>

namespace realm {

/**
 `asymmetric_object` is a base class used to define asymmetric Realm objects.

 Asymmetric objects can only be created using the `create(_ type:, value:)`
 function, and cannot be added, removed or queried.
 When created, asymmetric objects will be synced unidirectionally to the MongoDB
 database and cannot be accessed locally.

 Linking an asymmetric object within an `object` is not allowed and will throw an error.

 The property types supported on `asymmetric_object` are the same as for `object`,
 except for that asymmetric objects can only link to embedded objects, so `object`
 and `std::vector<object>` properties are not supported (`embedded_object` and
 `std::vector<embedded_object>` *are*).

 ```swift
 class person: asymmetric_object<person> {
     realm::persisted<std::string> name;
     realm::persisted<std::vector<addresses>> addresses;
 }
 ```
 */
    template<class T>
    struct asymmetric_object : public object_base<T> {
        void manage(const internal::bridge::table& target_table,
            const internal::bridge::realm& realm) {
            if (!this->m_object) {
                if constexpr (T::schema.HasPrimaryKeyProperty) {
                    auto val = (static_cast<T&>(*this)).*decltype(T::schema)::PrimaryKeyProperty::ptr;
                    this->m_object = internal::bridge::object(realm, target_table.create_object_with_primary_key(*val));
                } else {
                    this->m_object = internal::bridge::object(realm, target_table.create_object());
                }
                std::apply([this](auto&&... p) {
                    ((static_cast<T&>(*this).*(std::decay_t<decltype(p)>::ptr))
                            .manage(&*this->m_object, this->m_object->get_obj().get_table().get_column_key(p.name)), ...);
                }, T::schema.ps);
//                assign_accessors(*this->m_object);
            }
        }
    };
} // namespace realm

#endif
