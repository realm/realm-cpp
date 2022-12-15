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

        // Assigning an object type to the rhs
        persisted<T, type_info::OptionalPersistable<T>>& operator=(typename T::value_type& o) {
            if (auto obj = this->m_object) {
                if constexpr (type_info::PrimitivePersistableConcept<T>::value) {
                    if constexpr (type_info::EnumPersistableConcept<T>::value) {
                        obj->obj().template set<Int>(this->managed, static_cast<Int>(o));
                    } else {
                        obj->obj().template set<this->type>(this->managed, o);
                    }
                } else if constexpr (type_info::MixedPersistableConcept<T>::value) {
                    obj->obj().set_any(this->managed, type_info::persisted_type<T>::convert_if_required(o));
                }
                    // if parent is managed...
                else if constexpr (type_info::EmbeddedObjectPersistableConcept<T>::value) {
                    // if non-null object is being assigned...
                    if (o.m_object) {
                        // if object is managed, we will to set the link
                        // to the new target's key
                        obj->obj().template set<this->type>(this->managed, o.m_object->obj().get_key());
                    } else {
                        // else new unmanaged object is being assigned.
                        // we must assign the values to this object's fields
                        // TODO:
                        REALM_UNREACHABLE();
                    }
                }
                else if constexpr (type_info::OptionalObjectPersistableConcept<T>::value) {
                        if (o.m_object) {
                            // if object is managed, we will to set the link
                            // to the new target's key
                            obj->obj().template set<ObjKey>(this->managed, o.m_object->obj().get_key());
                        } else {
                            auto actual_schema = *obj->get_realm()->schema().find(T::value_type::schema.name);
                            auto& group = obj->get_realm()->read_group();
                            auto table = group.get_table(actual_schema.table_key);
                            T::value_type::schema.add(o, table, obj->get_realm());
                            obj->obj().template set<ObjKey>(this->managed, o.m_object->obj().get_key());
                        }
                }
                else if constexpr (type_info::OptionalPersistableConcept<T>::value) {
                    using UnwrappedType = typename type_info::persisted_type<typename T::value_type>::type;
                    if constexpr (type_info::EnumPersistableConcept<typename T::value_type>::value) {
                        obj->obj().template set<Int>(this->managed, static_cast<Int>(o));
                    } else {
                        obj->obj().template set<UnwrappedType>(this->managed, type_info::persisted_type<typename T::value_type>::convert_if_required(o));
                    }
                }
                else {
                    obj->obj().template set<this->type>(this->managed, o);
                }
            } else {
                new (&this->unmanaged) T(o);
            }
            return *this;
        }
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
