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

#ifndef realm_persisted_hpp
#define realm_persisted_hpp

//#include <cpprealm/notifications.hpp>
#include <cpprealm/type_info.hpp>

#include <realm/query.hpp>

#include <realm/object-store/list.hpp>
#include <realm/object-store/shared_realm.hpp>

#include <realm/util/functional.hpp>

namespace realm {

namespace internal {
    template <typename Class, typename ...Properties>
    struct schema;
}

struct FieldValue;
template <typename T, typename = void>
struct persisted;
struct notification_token;

template <typename T>
using Equatable = typename std::enable_if<
        std::declval<T>() == std::declval<T>(),
        T
>::type;

class rbool;

template <typename, typename = void>
struct persisted_base;

template <typename T>
struct persisted_base<T, realm::type_info::Persistable<T>> {
    using Result = T;
    using type = typename realm::type_info::persisted_type<T>::type;

    persisted_base(const T& value);
    persisted_base(T&& value);
//
//    template <typename S, std::enable_if_t<type_info::ObjectPersistableConcept<S>::value && std::is_same_v<S, typename T::value_type>>>
//    persisted_base(std::optional<S> value) {
//        unmanaged = value;
//    }
//    template <typename S, type_info::ObjectBasePersistable<T>>
//    persisted_base(std::optional<S> value);

    persisted_base();
    ~persisted_base();
    persisted_base(const persisted_base& o) {
        *this = o;
    }
    persisted_base(persisted_base&& o) {
        *this = std::move(o);
    }

    persisted_base& operator=(const T& o) {
        if (auto obj = m_object) {
            if constexpr (type_info::PrimitivePersistableConcept<T>::value) {
                obj->obj().template set<type>(managed, o);
            } else if constexpr (type_info::MixedPersistableConcept<T>::value) {
                obj->obj().set_any(managed, type_info::persisted_type<T>::convert_if_required(o));
            }
            // if parent is managed...
            else if constexpr (type_info::EmbeddedObjectPersistableConcept<T>::value) {
                // if non-null object is being assigned...
                if (o.m_object) {
                    // if object is managed, we will to set the link
                    // to the new target's key
                    obj->obj().template set<type>(managed, o.m_object->obj().get_key());
                } else {
                    // else new unmanaged object is being assigned.
                    // we must assign the values to this object's fields
                    // TODO:
                    REALM_UNREACHABLE();
                }
            } else if constexpr (type_info::OptionalObjectPersistableConcept<T>::value) {
                // if object...
                if (auto link = o) {
                    // if non-null object is being assigned...
                    if (link->m_object) {
                        // if object is managed, we will to set the link
                        // to the new target's key
                        obj->obj().template set<ObjKey>(managed, link->m_object->obj().get_key());
                    } else {
                        // else new unmanaged object is being assigned.
                        // we must assign the values to this object's fields
                        // TODO:
                        REALM_UNREACHABLE();
                    }
                } else {
                    // else null link is being assigned to this field
                    // e.g., `person.dog = std::nullopt;`
                    // set the parent column to null and unset the co
                    obj->obj().set_null(managed);
                }
            } else if constexpr (type_info::OptionalPersistableConcept<T>::value) {
                using UnwrappedType = typename type_info::persisted_type<typename T::value_type>::type;
                if (o) {
                    if constexpr (type_info::EnumPersistableConcept<typename T::value_type>::value) {
                        obj->obj().template set<Int>(managed, static_cast<Int>(*o));
                    } else {
                        obj->obj().template set<UnwrappedType>(managed,
                                                               type_info::persisted_type<UnwrappedType>::convert_if_required(
                                                                       *o));
                    }
                } else {
                    obj->obj().set_null(managed);
                }
            } else {
                obj->obj().template set<type>(managed, o);
            }
        } else {
            new (&unmanaged) T(o);
        }
        return *this;
    }
    persisted_base& operator=(const persisted_base& o) {
        if (auto obj = o.m_object) {
            m_object = obj;
            new (&managed) ColKey(o.managed);
        } else {
            new (&unmanaged) T(o.unmanaged);
        }
        return *this;
    }
    persisted_base& operator=(persisted_base&& o) {
        if (o.m_object) {
            m_object = o.m_object;
            new (&managed) ColKey(std::move(o.managed));
        } else {
            new (&unmanaged) T(std::move(o.unmanaged));
        }
        return *this;
    }

    T operator *() const
    {
        if (m_object) {
            if constexpr (type_info::OptionalPersistableConcept<T>::value || type_info::EmbeddedObjectPersistableConcept<T>::value) {
                if constexpr (type_info::EmbeddedObjectPersistableConcept<T>::value) {
                    return T::schema.create(m_object->obj().get_linked_object(managed), m_object->get_realm());
                } else if constexpr (type_info::ObjectBasePersistableConcept<typename T::value_type>::value) {
                    if (m_object->obj().is_null(managed)) {
                        return std::nullopt;
                    } else {
                        return T::value_type::schema.create(m_object->obj().get_linked_object(managed),
                                                            m_object->get_realm());
                    }
                } else {
                    using UnwrappedType = typename type_info::persisted_type<typename T::value_type>::type;
                    // convert optionals
                    if (m_object->obj().is_null(managed)) {
                        return std::nullopt;
                    } else {
                        auto value = m_object->obj().template get<UnwrappedType>(managed);
                        if constexpr (std::is_same_v<UnwrappedType, BinaryData>) {
                            return std::vector<u_int8_t>(value.data(), value.data() + value.size());
                        } else if constexpr (type_info::EnumPersistableConcept<typename T::value_type>::value) {
                            return std::optional<typename T::value_type>(static_cast<typename T::value_type>(value));
                        } else {
                            return T(value);
                        }
                    }
                }
            } else {
                if constexpr (std::is_same_v<realm::BinaryData, type>) {
                    realm::BinaryData binary = m_object->obj().template get<type>(managed);
                    return std::vector<u_int8_t>(binary.data(), binary.data() + binary.size());
                } else if constexpr (type_info::MixedPersistableConcept<T>::value) {
                    Mixed mixed = m_object->obj().template get<type>(managed);
                    return type_info::mixed_to_variant<T>(mixed);
                } else {
                    return static_cast<T>(m_object->obj().template get<type>(managed));
                }
            }
        } else {
            return unmanaged;
        }
    }
protected:
    union {
        T unmanaged;
        realm::ColKey managed;
    };

    template <typename Class, typename ...Properties>
    friend struct internal::schema;
    template <auto Ptr, bool IsPrimaryKey>
    friend struct property;
    template <typename>
    friend struct query;
    type as_core_type() const
    {
        if constexpr (type_info::ObjectBasePersistableConcept<T>::value) {
            if (m_object) {
                return m_object->obj().template get<type>(managed);
            } else {
                REALM_ASSERT(false);
            }
        } else {
            if (m_object) {
                if constexpr (type_info::PrimitivePersistableConcept<T>::value) {
                    return m_object->obj().template get<type>(managed);
                } else if  constexpr (type_info::ListPersistableConcept<T>::value) {
                    return m_object->obj().template get_list_values<typename type_info::persisted_type<typename T::value_type>::type>(managed);
                } else if constexpr (type_info::OptionalPersistableConcept<T>::value) {
                    using UnwrappedType = typename type_info::persisted_type<typename T::value_type>::type;
                    if (m_object->obj().is_null(managed)) {
                        return std::nullopt;
                    } else {
                        return m_object->obj().template get<UnwrappedType>(managed);
                    }
                } else {
                    return m_object->obj().template get<type>(managed);
                }
            } else {
                return type_info::persisted_type<T>::convert_if_required(unmanaged);
            }
        }
    }
    void assign(const Obj& object, const ColKey& col_key, SharedRealm realm)
    {
        m_object = Object(realm, object);
        managed = ColKey(col_key);
    }
    std::optional<Object> m_object;

    // MARK: Queries
    bool should_detect_usage_for_queries = false;
    Query* query;
    bool is_and;
    bool is_or;

    void prepare_for_query(Query& query_builder) {
        should_detect_usage_for_queries = true;
        query = &query_builder;
    }

    template <typename V>
    friend rbool operator==(const persisted<V>& a, const V& b);
    template <typename V>
    friend rbool operator==(const persisted<V>& a, const persisted<V>& b);
    template <typename V>
    friend rbool operator==(const persisted<V>& a, const char* b);
    template <typename V>
    friend rbool operator!=(const persisted<V>& a, const V& b);
    template <typename V>
    friend rbool operator!=(const persisted<V>& a, const persisted<V>& b);
    template <typename V>
    friend rbool operator!=(const persisted<V>& a, const char* b);
    template <template <typename ...> typename Variant, typename ...Ts, typename V>
    std::enable_if_t<type_info::is_variant_t<Variant<Ts...>>::value, rbool>
    friend operator==(const persisted<Variant<Ts...>>& a, const V& b);
    template <template <typename ...> typename Variant, typename ...Ts, typename V>
    std::enable_if_t<type_info::is_variant_t<Variant<Ts...>>::value, rbool>
    friend operator==(const persisted<Variant<Ts...>>& a, V&& b);
    template <typename V>
    std::enable_if_t<type_info::ComparableConcept<V>::value, rbool>
    friend inline operator <(const persisted<V>& lhs, const V& a);
    template <typename V>
    std::enable_if_t<type_info::ComparableConcept<V>::value, rbool>
    friend inline operator >(const persisted<V>& lhs, const V& a);
    template <typename V>
    std::enable_if_t<type_info::ComparableConcept<V>::value, rbool>
    friend inline operator <=(const persisted<V>& lhs, const V& a);
    template <typename V>
    std::enable_if_t<type_info::ComparableConcept<V>::value, rbool>
    friend inline operator >=(const persisted<V>& lhs, const V& a);
    template <typename V>
    std::enable_if_t<type_info::ComparableConcept<V>::value, rbool>
    friend inline operator >(const persisted<V>& lhs, const persisted<V>& a);
    template <typename V>
    std::enable_if_t<type_info::ComparableConcept<V>::value, rbool>
    friend inline operator <(const persisted<V>& lhs, const persisted<V>& a);
    template <typename V>
    std::enable_if_t<type_info::ComparableConcept<V>::value, rbool>
    friend inline operator >=(const persisted<V>& lhs, const persisted<V>& a);
    template <typename V>
    std::enable_if_t<type_info::ComparableConcept<V>::value, rbool>
    friend inline operator <=(const persisted<V>& lhs, const persisted<V>& a);
    template <typename V>
    std::enable_if_t<type_info::is_optional<V>::value, rbool>
    friend inline operator==(const persisted<V>& a, const std::nullopt_t&);
    template <typename V>
    std::enable_if_t<type_info::is_optional<V>::value, rbool>
    friend inline operator==(const persisted<V>& a, const V&);
    template <typename V>
    std::enable_if_t<type_info::is_optional<V>::value, rbool>
    friend inline operator==(const persisted<V>& a, const typename V::value_type&);
};

/// `realm::persisted` is used to declare properties on `realm::object` subclasses which should be
/// managed by Realm.
///
/// Example of usage:
/// ```cpp
/// class MyModel: realm::object {
///     // A basic property declaration. A property with no
///     // default value supplied will default to `nil` for
///     // Optional types, zero for numeric types, false for Bool,
///     // an empty string/data, and a new random value for UUID
///     // and ObjectID.
///     realm::persisted<int> basic_int_property;
///
///     // Custom default values can be specified with the
///     // standard c++ syntax
///     realm::persisted<int> int_with_custom_default = 5;
///
///     // Primary key properties can be picked in schema.
///     realm::persisted<int> var _id;
///
///     // Properties which are not marked with `persisted` will
///     // be ignored entirely by Realm.
///     bool ignoredProperty = true
///
///     using schema = realm::schema<"MyModel",
///                             realm::property<"basic_int_property", &MyModel::basic_int_property>,
///                             realm::property<"int_with_custom_default", &MyModel::int_with_custom_default>,
///                             realm::property<"_id", &MyModel::_id, true>>; // primary key property
/// }
/// ```
///
///  A property can be set as the class's primary key by passing `true`
///  into its schema property. Compound primary keys are not supported, and setting
///  more than one property as the primary key will throw an exception at
///  runtime. Only Int, String, UUID and ObjectID properties can be made the
///  primary key, and when using MongoDB Realm, the primary key must be named
///  `_id`. The primary key property can only be mutated on unmanaged objects,
///  and mutating it on an object which has been added to a Realm will throw an
///  exception.
///
///  Properties can optionally be given a default value using the standard C++20
///  syntax. If no default value is given, a value will be generated on first
///  access: `nil` for all Optional types, zero for numeric types, false for
///  Bool, an empty string/data, and a new random value for UUID and ObjectID.
///  Doing so will work, but will result in worse performance when accessing
///  objects managed by a Realm. Similarly, ObjectID properties *should not* be initialized to
///  `ObjectID.generate()`, as doing so will result in extra ObjectIDs being
///  generated and then discarded when reading from a Realm.
template <typename T, typename = type_info::NonContainerPersistable<T>>
struct persisted_noncontainer_base : public persisted_base<T> {
    using persisted_base<T>::persisted_base;
    using persisted_base<T>::operator=;
    using persisted_base<T>::operator*;
    using type = typename realm::type_info::persisted_type<T>::type;
};

template <typename T, typename = type_info::NonContainerPersistable<T>>
struct persisted_add_assignable_base : public persisted_noncontainer_base<T> {
    using persisted_noncontainer_base<T>::persisted_noncontainer_base;
    using persisted_noncontainer_base<T>::operator=;
    using persisted_noncontainer_base<T>::operator*;

    void operator --() {
        *this -= 1;
    }
    T operator -() {
        return *this * -1;
    }
    T operator -(const T& a) {
        return *this - 1;
    }
    void operator -=(const T& a) {
        if (this->m_object) {
            this->m_object->obj().template set<typename persisted_base<T>::type>(this->managed, *(*this) - a);
        } else {
            this->unmanaged -= a;
        }
    }
    T operator +(const T& a) {
        return *this + 1;
    }
    void operator +=(const T& a) {
        if (this->m_object) {
            this->m_object->obj().template set<typename persisted_base<T>::type>(this->managed, *(*this) + a);
        } else {
            this->unmanaged += a;
        }
    }
    void operator ++() {
        *this += 1;
    }
    T operator *(const T& a);
    void operator *=(const T& a);
    T operator /(const T& a);
    void operator /=(const T& a);
};

//// MARK: Implementation

template <typename T>
persisted_base<T, realm::type_info::Persistable<T>>::persisted_base() {
    new (&unmanaged) T();
}
template <typename T>
persisted_base<T, realm::type_info::Persistable<T>>::persisted_base(const T& value) {
    unmanaged = value;
}
template <typename T>
persisted_base<T, realm::type_info::Persistable<T>>::persisted_base(T&& value) {
    unmanaged = std::move(value);
}

template <typename T>
persisted_base<T, realm::type_info::Persistable<T>>::~persisted_base()
{
    if (should_detect_usage_for_queries) {
        return;
    }
    if constexpr (!type_info::PrimitivePersistableConcept<T>::value) {
        if constexpr (type_info::ListPersistableConcept<T>::value) {
            using std::vector;
            if (!m_object) {
                unmanaged.clear();
            }
        }
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (!m_object) {
            using std::string;
            unmanaged.~string();
        }
    }
}

// MARK: rbool

class rbool {
    bool is_for_queries = false;
    friend rbool operator &&(const rbool& lhs, const rbool& rhs);
    template <typename T>
    friend rbool operator==(const persisted<T>& a, const T& b);
    template <typename T>
    friend rbool operator==(const persisted<T>& a, const persisted<T>& b);
    template <typename T>
    friend rbool operator==(const persisted<T>& a, const char* b);
    template <template <typename ...> typename Variant, typename ...Ts, typename V>
    std::enable_if_t<type_info::is_variant_t<Variant<Ts...>>::value, rbool>
    friend operator==(const persisted<Variant<Ts...>>& a, const V& b);
    template <template <typename ...> typename Variant, typename ...Ts, typename V>
    std::enable_if_t<type_info::is_variant_t<Variant<Ts...>>::value, rbool>
    friend operator==(const persisted<Variant<Ts...>>& a, V&& b);

    template <typename T, typename>
    friend struct persisted_noncontainer_base;
    template <typename T, typename>
    friend struct persisted;
    template <typename T>
    friend struct results;

    template <typename T>
    friend rbool operator!=(const persisted<T>& a, const T& b);
    template <typename T>
    friend rbool operator!=(const persisted<T>& a, const persisted<T>& b);
    template <typename T>
    friend rbool operator!=(const persisted<T>& a, const char* b);

    friend rbool operator ||(const rbool& lhs, const rbool& rhs);
    template <typename V>
    std::enable_if_t<type_info::ComparableConcept<V>::value, rbool>
    friend inline operator <(const persisted<V>& lhs, const V& a);
    template <typename V>
    std::enable_if_t<type_info::ComparableConcept<V>::value, rbool>
    friend inline operator >(const persisted<V>& lhs, const V& a);
    template <typename V>
    std::enable_if_t<type_info::ComparableConcept<V>::value, rbool>
    friend inline operator <=(const persisted<V>& lhs, const V& a);
    template <typename V>
    std::enable_if_t<type_info::ComparableConcept<V>::value, rbool>
    friend inline operator >=(const persisted<V>& lhs, const V& a);
    template <typename V>
    std::enable_if_t<type_info::ComparableConcept<V>::value, rbool>
    friend inline operator >(const persisted<V>& lhs, const persisted<V>& a);
    template <typename V>
    std::enable_if_t<type_info::ComparableConcept<V>::value, rbool>
    friend inline operator <(const persisted<V>& lhs, const persisted<V>& a);
    template <typename V>
    std::enable_if_t<type_info::ComparableConcept<V>::value, rbool>
    friend inline operator >=(const persisted<V>& lhs, const persisted<V>& a);
    template <typename V>
    std::enable_if_t<type_info::ComparableConcept<V>::value, rbool>
    friend inline operator <=(const persisted<V>& lhs, const persisted<V>& a);
    template <typename V>
    std::enable_if_t<type_info::is_optional<V>::value, rbool>
    friend inline operator==(const persisted<V>& a, const std::nullopt_t&);
    template <typename V>
    std::enable_if_t<type_info::is_optional<V>::value, rbool>
    friend inline operator==(const persisted<V>& a, const V&);
    template <typename V>
    std::enable_if_t<type_info::is_optional<V>::value, rbool>
    friend inline operator==(const persisted<V>& a, const typename V::value_type&);
public:
    ~rbool() {
        if (is_for_queries)
            q.~Query();
    }
    operator bool() const {
        return b;
    }
    union {
        bool b;
        mutable Query q;
    };

    rbool(Query&& q) : q(std::move(q)), is_for_queries(true) {}
    rbool(bool b) : b(b) {}
    rbool(const rbool& r) {
        if (r.is_for_queries) {
            new (&q) Query(r.q);
            is_for_queries = true;
        }
        else
            b = r.b;
    }
};
//
//// MARK: Equatable

inline rbool operator &&(const rbool& lhs, const rbool& rhs) {
    if (lhs.is_for_queries) {
        lhs.q.and_query(rhs.q);
        return lhs;
    }
    return lhs.b && rhs.b;
}
inline rbool operator ||(const rbool& lhs, const rbool& rhs) {
    if (lhs.is_for_queries) {
        lhs.q = lhs.q || rhs.q;
        return lhs;
    }
    return lhs.b && rhs.b;
}

template <typename T>
rbool operator==(const persisted<T>& a, const T& b)
{
    if (a.should_detect_usage_for_queries) {
        auto query = Query(a.query->get_table());
        query.equal(a.managed, type_info::persisted_type<T>::convert_if_required(b));
        return {std::move(query)};
    }
    return *a == b;
}

template <typename T>
rbool operator==(const persisted<T>& a, const persisted<T>& b)
{
    if (a.should_detect_usage_for_queries) {
        auto query = Query(a.query->get_table());
        query.equal(a.managed, b.managed);
        return {std::move(query)};
    }
    return *a == *b;
}

template <typename T>
rbool operator!=(const persisted<T>& a, const T& b)
{
    if (a.should_detect_usage_for_queries) {
        auto query = Query(a.query->get_table());
        query.not_equal(a.managed, type_info::persisted_type<T>::convert_if_required(b));
        return {std::move(query)};
    }
    return !(a == b);
}

template <typename T>
rbool operator!=(const persisted<T>& a, const persisted<T>& b)
{
    if (a.should_detect_usage_for_queries) {
        auto query = Query(a.query->get_table());
        query.not_equal(a.managed, b.managed);
        return {std::move(query)};
    }
    return !(a == b);
}

template <typename T>
rbool operator==(const persisted<T>& a, const char* b)
{
    if (a.should_detect_usage_for_queries) {
        auto query = Query(a.query->get_table());
        query.equal(a.managed, b);
        return {std::move(query)};
    }
    return type_info::persisted_type<T>::convert_if_required(*a) == b;
}

template <typename T>
rbool operator!=(const persisted<T>& a, const char* b)
{
    if (a.should_detect_usage_for_queries) {
        auto query = Query(a.query->get_table());
        query.not_equal(a.managed, b);
        return {std::move(query)};
    }
    return !(a == b);
}

// MARK: Comparable
template <typename T>
std::enable_if_t<type_info::ComparableConcept<T>::value, rbool>
inline operator <(const persisted<T>& lhs, const T& a) {
    if (lhs.should_detect_usage_for_queries) {
        auto query = Query(lhs.query->get_table());
        query.less(lhs.managed, type_info::persisted_type<T>::convert_if_required(a));
        return {std::move(query)};
    }
    return *lhs < a;
}
template <typename T>
std::enable_if_t<type_info::ComparableConcept<T>::value, rbool>
inline operator >(const persisted<T>& lhs, const T& a) {
    if (lhs.should_detect_usage_for_queries) {
        auto query = Query(lhs.query->get_table());
        query.greater(lhs.managed, type_info::persisted_type<T>::convert_if_required(a));
        return {std::move(query)};
    }
    return *lhs > a;
}
template <typename T>
std::enable_if_t<type_info::ComparableConcept<T>::value, rbool>
inline operator <=(const persisted<T>& lhs, const T& a) {
    if (lhs.should_detect_usage_for_queries) {
        auto query = Query(lhs.query->get_table());
        query.less_equal(lhs.managed, type_info::persisted_type<T>::convert_if_required(a));
        return {std::move(query)};
    }
    return *lhs <= a;
}
template <typename T>
std::enable_if_t<type_info::ComparableConcept<T>::value, rbool>
inline operator >=(const persisted<T>& lhs, const T& a) {
    if (lhs.should_detect_usage_for_queries) {
        auto query = Query(lhs.query->get_table());
        query.greater_equal(lhs.managed, type_info::persisted_type<T>::convert_if_required(a));
        return {std::move(query)};
    }
    return *lhs >= a;
}

template <typename T>
std::enable_if_t<type_info::ComparableConcept<T>::value, rbool>
inline operator <(const persisted<T>& lhs, const persisted<T>& a) {
    if (lhs.should_detect_usage_for_queries) {
        auto query = Query(lhs.query->get_table());
        query.less(lhs.managed, a.managed);
        return {std::move(query)};
    }
    return *lhs < *a;
}
template <typename T>
std::enable_if_t<type_info::ComparableConcept<T>::value, rbool>
inline operator >(const persisted<T>& lhs, const persisted<T>& a) {
    if (lhs.should_detect_usage_for_queries) {
        auto query = Query(lhs.query->get_table());
        query.greater(lhs.managed, a.managed);
        return {std::move(query)};
    }
    return *lhs > *a;
}
template <typename T>
std::enable_if_t<type_info::ComparableConcept<T>::value, rbool>
inline operator <=(const persisted<T>& lhs, const persisted<T>& a) {
    if (lhs.should_detect_usage_for_queries) {
        auto query = Query(lhs.query->get_table());
        query.less_equal(lhs.managed, a.managed);
        return {std::move(query)};
    }
    return *lhs <= *a;
}
template <typename T>
std::enable_if_t<type_info::ComparableConcept<T>::value, rbool>
inline operator >=(const persisted<T>& lhs, const persisted<T>& a) {
    if (lhs.should_detect_usage_for_queries) {
        auto query = Query(lhs.query->get_table());
        query.greater_equal(lhs.managed, a.managed);
        return {std::move(query)};
    }
    return *lhs >= *a;
}

template <typename T>
std::ostream& operator<< (std::ostream& stream, const persisted<T>& value)
{
    return stream << type_info::persisted_type<T>::convert_if_required(*value);
}

template <typename T>
std::ostream& operator<< (std::ostream& stream, const persisted<std::optional<T>>& value)
{
    if (*value) {
        return stream << "null";
    }
    return stream << type_info::persisted_type<T>::convert_if_required(**value);
}

template <typename T>
typename std::enable_if<std::is_base_of<realm::object_base, T>::value, std::ostream>::type&
operator<< (std::ostream& stream, const T& object)
{
    static_assert(type_info::ObjectBasePersistableConcept<T>::value);
    if (object.m_object) {
        return stream << object.m_object->obj();
    }
//    for (int i = 0; i < std::tuple_size<decltype(T::schema.properties)>{}; i++) {
//        auto props = std::get<i>(T::schema.properties);
//        auto name = T::schema.names[i];
//        stream << "{\n";
//        (stream << "\t" << name << ": " << *(object.*props.ptr) << "\n");
//        stream << "}";
//    }
    std::apply([&stream, &object](auto&&... props) {
        stream << "{\n";
        ((stream << "\t" << props.name << ": " << *(object.*props.ptr) << "\n"), ...);
        stream << "}";
    }, T::schema.properties);

    return stream;
}

template <typename T>
typename std::enable_if_t<type_info::BinaryPersistableConcept<T>::value, std::ostream>&
        operator<< (std::ostream& stream, const T& binary)
{
    return stream << binary;
}

}

#endif /* realm_persisted_hpp */
