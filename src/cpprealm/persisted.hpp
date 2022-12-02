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

#include <cpprealm/type_info.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/query.hpp>

#include <cpprealm/schema.hpp>

namespace realm {

namespace schemagen {
    template <typename Class, typename ...Properties>
    struct schema;
    template <auto Ptr, bool IsPrimaryKey>
    struct property;
}

struct FieldValue;
template <typename T, typename = void>
struct persisted;
struct notification_token;

class rbool;

namespace {
    template <typename T, typename Enable = void>
    struct is_optional : std::false_type {
        using underlying = T;
    };

    template <typename T>
    struct is_optional<std::optional<T> > : std::true_type {
        using underlying = T;
    };
}
template <typename T>
struct persisted_base {
    static_assert(std::is_destructible_v<T>);
    using Result = T;

    persisted_base(const T& value) {
        unmanaged = value;
    }
    persisted_base(T&& value) {
        unmanaged = std::move(value);
    }

    persisted_base() {}
    ~persisted_base() { }
    persisted_base(const persisted_base& o) {
        *this = o;
    }
    persisted_base(persisted_base&& o) {
        *this = std::move(o);
    }

    persisted_base& operator=(const persisted_base& o) noexcept {
        if (auto obj = o.m_object) {
            m_object = obj;
            new (&managed) internal::bridge::col_key(o.managed);
        } else {
            new (&unmanaged) T(o.unmanaged);
        }
        return *this;
    }
    persisted_base& operator=(persisted_base&& o) noexcept {
        if (o.m_object) {
            m_object = o.m_object;
            new (&managed) internal::bridge::col_key(std::move(o.managed));
        } else {
            new (&unmanaged) T(std::move(o.unmanaged));
        }
        return *this;
    }

    virtual std::conditional_t<is_optional<T>::value,
            typename is_optional<T>::underlying,
            typename is_optional<T>::underlying> operator *() const {
        if (this->is_managed()) {
            auto val = internal::type_info::deserialize<T>(this->m_object->obj().template
                    get<typename internal::type_info::type_info<T>::internal_type>(this->managed));
            if constexpr (is_optional<T>::value) {
                return *val;
            } else {
                return val;
            }
        } else {
            if constexpr (is_optional<T>::value) {
                return *this->unmanaged;
            } else {
                return this->unmanaged;
            }
        }
    }
//    {
//        if (m_object) {
//            if constexpr (type_info::OptionalPersistableConcept<T>::value || type_info::EmbeddedObjectPersistableConcept<T>::value) {
//                if constexpr (type_info::EmbeddedObjectPersistableConcept<T>::value) {
//                    return T::schema.create(m_object->obj().get_linked_object(managed), m_object->get_realm());
//                } else if constexpr (type_info::ObjectBasePersistableConcept<typename T::value_type>::value) {
//                    if (m_object->obj().is_null(managed)) {
//                        return std::nullopt;
//                    } else {
//                        return T::value_type::schema.create(m_object->obj().get_linked_object(managed),
//                                                            m_object->get_realm());
//                    }
//                } else {
//                    using UnwrappedType = typename type_info::persisted_type<typename T::value_type>::type;
//                    // convert optionals
//                    if (m_object->obj().is_null(managed)) {
//                        return std::nullopt;
//                    } else {
//                        auto value = m_object->obj().template get<UnwrappedType>(managed);
//                        if constexpr (std::is_same_v<UnwrappedType, BinaryData>) {
//                            return std::vector<u_int8_t>(value.data(), value.data() + value.size());
//                        } else if constexpr (type_info::EnumPersistableConcept<typename T::value_type>::value) {
//                            return std::optional<typename T::value_type>(static_cast<typename T::value_type>(value));
//                        } else {
//                            return T(value);
//                        }
//                    }
//                }
//            } else {
//                if constexpr (!type_info::PrimitivePersistableConcept<T>::value
//                                && !type_info::MixedPersistableConcept<T>::value) {
//                    if constexpr (type_info::ListPersistableConcept<T>::value) {
//                        T v;
//                        auto lst = m_object->obj().template get_list_values<typename type_info::persisted_type<typename T::value_type>::type>(
//                                managed);
//                        for (size_t i; i < lst.size(); i++) {
//                            if constexpr (type_info::ObjectBasePersistableConcept<typename T::value_type>::value) {
//                                auto obj = lst.get_object(i);
//                                v.push_back(T::value_type::schema::create(obj, obj.get_table(), nullptr));
//                            } else {
//                                v.push_back(static_cast<typename T::value_type>(lst[i]));
//                            }
//                        }
//
//                        return v;
//                    } else {
//                        REALM_UNREACHABLE();
//                    }
//                }
//                if constexpr (std::is_same_v<realm::BinaryData, type>) {
//                    realm::BinaryData binary = m_object->obj().template get<type>(managed);
//                    return std::vector<u_int8_t>(binary.data(), binary.data() + binary.size());
//                } else if constexpr (type_info::MixedPersistableConcept<T>::value) {
//                    Mixed mixed = m_object->obj().template get<type>(managed);
//                    return type_info::mixed_to_variant<T>(mixed);
//                } else {
//                    return static_cast<T>(m_object->obj().template get<type>(managed));
//                }
//            }
//        } else {
//            return unmanaged;
//        }
//    }
protected:
    union {
        T unmanaged;
        internal::bridge::col_key managed;
    };

    [[nodiscard]] bool is_managed() const {
        return m_object.has_value();
    }
    void manage(const internal::bridge::obj& object,
                const internal::bridge::col_key& col_key,
                const internal::bridge::realm& realm)
    {
        m_object = internal::bridge::object(realm, object);
        managed = col_key;
    }
    std::optional<internal::bridge::object> m_object;

    // MARK: Queries
    bool should_detect_usage_for_queries = false;
    internal::bridge::query* query;
    bool is_and;
    bool is_or;

    void prepare_for_query(internal::bridge::query& query_builder) {
        should_detect_usage_for_queries = true;
        query = &query_builder;
    }
    template <typename Class, typename ...Properties>
    friend struct schemagen::schema;
    template <auto Ptr, bool IsPrimaryKey>
    friend struct schemagen::property;
    template <typename>
    friend struct query;
    template <typename U, typename V>
    friend rbool operator==(const persisted<U>& a, const V& b);
    template <auto Ptr, typename Class, typename Result, typename>
    friend struct realm::schemagen::property_deducer;
    template <template <typename ...> typename Variant, typename ...Ts, typename V>
    std::enable_if_t<internal::type_info::is_variant_t<Variant<Ts...>>::value, rbool>
    friend operator==(const persisted<Variant<Ts...>>& a, V&& b);
    template <template <typename ...> typename Variant, typename ...Ts, typename V>
    std::enable_if_t<internal::type_info::is_variant_t<Variant<Ts...>>::value, rbool>
    friend operator==(const persisted<Variant<Ts...>>& a, const V& b);
    friend inline rbool operator==(const persisted<std::string>& a, const char* b);
    template <typename U, typename V>
    friend rbool operator!=(const persisted<U>& a, const V& b);
//    template <typename V>
//    inline rbool operator==(const persisted<V>& lhs, const persisted<V>& rhs);
    template <typename R>
    friend inline rbool operator==(const persisted<R>& lhs, const persisted<R>& rhs);
    template <typename U, typename V>
    friend rbool operator>(const persisted<U>& a, const V& b);
    template <typename U, typename V>
    friend rbool operator<(const persisted<U>& a, const V& b);
    template <typename U, typename V>
    friend rbool operator<=(const persisted<U>& a, const V& b);
    template <typename U, typename V>
    friend rbool operator>=(const persisted<U>& a, const V& b);
};

//// MARK: Implementation
//
//template <typename T>
//persisted_base<T, realm::type_info::Persistable<T>>::persisted_base() {
//    new (&unmanaged) T();
//}
//template <typename T>
//persisted_base<T, realm::type_info::Persistable<T>>::persisted_base(const T& value) {
//    unmanaged = value;
//}
//template <typename T>
//persisted_base<T, realm::type_info::Persistable<T>>::persisted_base(T&& value) {
//    unmanaged = std::move(value);
//}
//
//template <typename T>
//persisted_base<T, realm::type_info::Persistable<T>>::~persisted_base()
//{
//    if (should_detect_usage_for_queries) {
//        return;
//    }
//    if constexpr (!type_info::PrimitivePersistableConcept<T>::value) {
//        if constexpr (type_info::ListPersistableConcept<T>::value) {
//            using std::vector;
//            if (!m_object) {
//                unmanaged.clear();
//            }
//        }
//    } else if constexpr (std::is_same_v<T, std::string>) {
//        if (!m_object) {
//            using std::string;
//            unmanaged.~string();
//        }
//    }
//}

// MARK: rbool

class rbool {
    bool is_for_queries = false;
    rbool(internal::bridge::query&& q) : q(q), is_for_queries(true) {}
    rbool(bool b) : b(b) {}
    rbool(const rbool& r) {
        if (r.is_for_queries) {
            new (&q) internal::bridge::query(r.q);
            is_for_queries = true;
        }
        else
            b = r.b;
    }
    friend rbool operator &&(const rbool& lhs, const rbool& rhs);
    template <typename T>
    friend rbool operator==(const persisted<T>& a, const T& b);
    template <typename T>
    friend rbool operator==(const persisted<T>& a, const persisted<T>& b);
    template <typename T>
    friend rbool operator==(const persisted<T>& a, const char* b);

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

    template <typename T, typename V>
    friend rbool operator==(const persisted<T>& a, const V& b);
    template <typename T, typename V>
    friend rbool operator!=(const persisted<T>& a, const V& b);
    template <typename T, typename V>
    friend rbool operator>(const persisted<T>& a, const V& b);
    template <typename T, typename V>
    friend rbool operator<(const persisted<T>& a, const V& b);
    template <typename T, typename V>
    friend rbool operator>=(const persisted<T>& a, const V& b);
    template <typename T, typename V>
    friend rbool operator<=(const persisted<T>& a, const V& b);
    template <typename T>
    friend rbool inline operator==(const persisted<std::optional<T>>& a, const std::optional<T>& b);
    template <template <typename ...> typename Variant, typename ...Ts, typename V>
    std::enable_if_t<internal::type_info::is_variant_t<Variant<Ts...>>::value, rbool>
    friend operator==(const persisted<Variant<Ts...>>& a, V&& b);
    friend inline rbool operator==(const persisted<std::string>& a, const char* b);
    template <template <typename ...> typename Variant, typename ...Ts, typename V>
    std::enable_if_t<internal::type_info::is_variant_t<Variant<Ts...>>::value, rbool>
    friend operator==(const persisted<Variant<Ts...>>& a, const V& b);
    friend inline rbool operator!=(const persisted<std::string>& a, const char* b);
public:
    ~rbool() {
        if (is_for_queries)
            q.~query();
    }
    operator bool() const {
        return b;
    }
    union {
        bool b;
        mutable internal::bridge::query q;
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
inline rbool operator==(const persisted<T>& lhs, const persisted<T>& rhs) {
    if (lhs.should_detect_usage_for_queries) {
        auto query = internal::bridge::query(lhs.query->get_table());
        query.equal(lhs.managed, internal::type_info::serialize<T>(*rhs));
        return {std::move(query)};
    }
    return *lhs == *rhs;
}

//
//template <typename T>
//rbool operator==(const persisted<T>& a, const T& b)
//{
//    if (a.should_detect_usage_for_queries) {
//        auto query = internal::query(a.query->get_table());
//        query.equal(a.managed, type_info::persisted_type<T>::convert_if_required(b));
//        return {std::move(query)};
//    }
//    return *a == b;
//}
//
//template <typename T>
//rbool operator==(const persisted<T>& a, const persisted<T>& b)
//{
//    if (a.should_detect_usage_for_queries) {
//        auto query = internal::query(a.query->get_table());
//        query.equal(a.managed, b.managed);
//        return {std::move(query)};
//    }
//    return *a == *b;
//}
//
//template <typename T>
//rbool operator!=(const persisted<T>& a, const T& b)
//{
//    if (a.should_detect_usage_for_queries) {
//        auto query = internal::query(a.query->get_table());
//        query.not_equal(a.managed, type_info::persisted_type<T>::convert_if_required(b));
//        return {std::move(query)};
//    }
//    return !(a == b);
//}
//
//template <typename T>
//rbool operator!=(const persisted<T>& a, const persisted<T>& b)
//{
//    if (a.should_detect_usage_for_queries) {
//        auto query = internal::query(a.query->get_table());
//        query.not_equal(a.managed, b.managed);
//        return {std::move(query)};
//    }
//    return !(a == b);
//}
//
//template <typename T>
//rbool operator==(const persisted<T>& a, const char* b)
//{
//    if (a.should_detect_usage_for_queries) {
//        auto query = internal::query(a.query->get_table());
//        query.equal(a.managed, b);
//        return {std::move(query)};
//    }
//    return type_info::persisted_type<T>::convert_if_required(*a) == b;
//}
//
//template <typename T>
//rbool operator!=(const persisted<T>& a, const char* b)
//{
//    if (a.should_detect_usage_for_queries) {
//        auto query = internal::query(a.query->get_table());
//        query.not_equal(a.managed, b);
//        return {std::move(query)};
//    }
//    return !(a == b);
//}
//
//// MARK: Comparable
//template <typename T>
//std::enable_if_t<type_info::ComparableConcept<T>::value, rbool>
//inline operator <(const persisted<T>& lhs, const T& a) {
//    if (lhs.should_detect_usage_for_queries) {
//        auto query = Query(lhs.query->get_table());
//        query.less(lhs.managed, type_info::persisted_type<T>::convert_if_required(a));
//        return {std::move(query)};
//    }
//    return *lhs < a;
//}
//template <typename T>
//std::enable_if_t<type_info::ComparableConcept<T>::value, rbool>
//inline operator >(const persisted<T>& lhs, const T& a) {
//    if (lhs.should_detect_usage_for_queries) {
//        auto query = Query(lhs.query->get_table());
//        query.greater(lhs.managed, type_info::persisted_type<T>::convert_if_required(a));
//        return {std::move(query)};
//    }
//    return *lhs > a;
//}
//template <typename T>
//std::enable_if_t<type_info::ComparableConcept<T>::value, rbool>
//inline operator <=(const persisted<T>& lhs, const T& a) {
//    if (lhs.should_detect_usage_for_queries) {
//        auto query = Query(lhs.query->get_table());
//        query.less_equal(lhs.managed, type_info::persisted_type<T>::convert_if_required(a));
//        return {std::move(query)};
//    }
//    return *lhs <= a;
//}
//template <typename T>
//std::enable_if_t<type_info::ComparableConcept<T>::value, rbool>
//inline operator >=(const persisted<T>& lhs, const T& a) {
//    if (lhs.should_detect_usage_for_queries) {
//        auto query = Query(lhs.query->get_table());
//        query.greater_equal(lhs.managed, type_info::persisted_type<T>::convert_if_required(a));
//        return {std::move(query)};
//    }
//    return *lhs >= a;
//}
//
//template <typename T>
//std::enable_if_t<type_info::ComparableConcept<T>::value, rbool>
//inline operator <(const persisted<T>& lhs, const persisted<T>& a) {
//    if (lhs.should_detect_usage_for_queries) {
//        auto query = Query(lhs.query->get_table());
//        query.less(lhs.managed, a.managed);
//        return {std::move(query)};
//    }
//    return *lhs < *a;
//}
//template <typename T>
//std::enable_if_t<type_info::ComparableConcept<T>::value, rbool>
//inline operator >(const persisted<T>& lhs, const persisted<T>& a) {
//    if (lhs.should_detect_usage_for_queries) {
//        auto query = Query(lhs.query->get_table());
//        query.greater(lhs.managed, a.managed);
//        return {std::move(query)};
//    }
//    return *lhs > *a;
//}
//template <typename T>
//std::enable_if_t<type_info::ComparableConcept<T>::value, rbool>
//inline operator <=(const persisted<T>& lhs, const persisted<T>& a) {
//    if (lhs.should_detect_usage_for_queries) {
//        auto query = Query(lhs.query->get_table());
//        query.less_equal(lhs.managed, a.managed);
//        return {std::move(query)};
//    }
//    return *lhs <= *a;
//}
//template <typename T>
//std::enable_if_t<type_info::ComparableConcept<T>::value, rbool>
//inline operator >=(const persisted<T>& lhs, const persisted<T>& a) {
//    if (lhs.should_detect_usage_for_queries) {
//        auto query = Query(lhs.query->get_table());
//        query.greater_equal(lhs.managed, a.managed);
//        return {std::move(query)};
//    }
//    return *lhs >= *a;
//}
//
//template <typename T>
//std::ostream& operator<< (std::ostream& stream, const persisted<T>& value)
//{
//    return stream << type_info::persisted_type<T>::convert_if_required(*value);
//}
//
//template <typename T>
//std::ostream& operator<< (std::ostream& stream, const persisted<std::optional<T>>& value)
//{
//    if (*value) {
//        return stream << "null";
//    }
//    return stream << type_info::persisted_type<T>::convert_if_required(**value);
//}
//
//template <typename T>
//typename std::enable_if<std::is_base_of<realm::object_base, T>::value, std::ostream>::type&
//operator<< (std::ostream& stream, const T& object)
//{
//    static_assert(type_info::ObjectBasePersistableConcept<T>::value);
//    if (object.m_object) {
//        return stream << object.m_object->obj();
//    }
////    for (int i = 0; i < std::tuple_size<decltype(T::schema.properties)>{}; i++) {
////        auto props = std::get<i>(T::schema.properties);
////        auto name = T::schema.names[i];
////        stream << "{\n";
////        (stream << "\t" << name << ": " << *(object.*props.ptr) << "\n");
////        stream << "}";
////    }
//    std::apply([&stream, &object](auto&&... props) {
//        stream << "{\n";
//        ((stream << "\t" << props.name << ": " << *(object.*props.ptr) << "\n"), ...);
//        stream << "}";
//    }, T::schema.properties);
//
//    return stream;
//}
//
//template <typename T>
//typename std::enable_if_t<type_info::BinaryPersistableConcept<T>::value, std::ostream>&
//        operator<< (std::ostream& stream, const T& binary)
//{
//    return stream << binary;
//}

}

#endif /* realm_persisted_hpp */
