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

#include <cpprealm/internal/type_info.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/query.hpp>

#include <cpprealm/schema.hpp>

#define __friend_rbool_operators__(type, op) \
        friend rbool operator op(const persisted<type>& a, const type& b); \

#define __friend_rbool_numeric_operators(type, op) \
    template <typename V> \
    friend std::enable_if_t<std::negation_v<std::is_same<V, bool>>, rbool> operator op(const persisted<type>& a, const V& b);

#define __friend_rbool_timestamp_operators__(op) \
        template <typename VV> \
        friend rbool operator op(const persisted<std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration>>& a, const VV& b); \

#define __friend_rbool_mixed_operators(op) \
    template <template <typename ...> typename Variant, typename ...Ts, typename V> \
    std::enable_if_t<internal::type_info::is_variant_t<Variant<Ts...>>::value, rbool> \
    friend operator op(const persisted<Variant<Ts...>>& a, V&& b);

#define __friend_rbool_const_char_operators(op) \
    friend rbool operator op(const persisted<std::string>& a, const char* b);

#define __friend_rbool_enum_operators(op) \
    template <typename TT, typename VV>                                      \
    friend std::enable_if_t<std::is_enum_v<TT>, rbool> operator op(const persisted<TT>& a, const VV& b);

#define __cpp_realm_friends \
        template <typename, typename ...> \
        friend struct realm::schemagen::schema; \
        template <typename> friend struct query;\
        template <typename> \
        friend struct persisted_base;     \
        template <typename> \
        friend struct persisted_primitive_base; \
        __friend_rbool_operators__(int64_t, >)  \
        __friend_rbool_operators__(int64_t, <) \
        __friend_rbool_operators__(int64_t, >=)   \
        __friend_rbool_operators__(int64_t, <=) \
        __friend_rbool_operators__(int64_t, ==) \
        __friend_rbool_operators__(int64_t, !=) \
        __friend_rbool_operators__(double, >)  \
        __friend_rbool_operators__(double, <) \
        __friend_rbool_operators__(double, >=)   \
        __friend_rbool_operators__(double, <=) \
        __friend_rbool_operators__(double, ==) \
        __friend_rbool_operators__(double, !=)  \
                            \
        __friend_rbool_operators__(bool, ==) \
        __friend_rbool_operators__(bool, !=)  \
                            \
        __friend_rbool_operators__(uuid, >)  \
        __friend_rbool_operators__(uuid, <) \
        __friend_rbool_operators__(uuid, >=)   \
        __friend_rbool_operators__(uuid, <=) \
        __friend_rbool_operators__(uuid, ==) \
        __friend_rbool_operators__(uuid, !=)  \
        \
        __friend_rbool_operators__(std::string, !=) \
        __friend_rbool_operators__(std::string, ==) \
                            \
        __friend_rbool_const_char_operators(==) \
        __friend_rbool_const_char_operators(!=) \
                            \
        __friend_rbool_operators__(std::vector<uint8_t>, !=) \
        __friend_rbool_operators__(std::vector<uint8_t>, ==) \
        __friend_rbool_timestamp_operators__(>) \
        __friend_rbool_timestamp_operators__(>=)\
        __friend_rbool_timestamp_operators__(<)   \
        __friend_rbool_timestamp_operators__(<=)\
        __friend_rbool_timestamp_operators__(!=)\
        __friend_rbool_timestamp_operators__(==)\
        __friend_rbool_mixed_operators(==)\
                            \
        __friend_rbool_enum_operators(==) \
        __friend_rbool_enum_operators(!=) \
        __friend_rbool_enum_operators(>) \
        __friend_rbool_enum_operators(<) \
        __friend_rbool_enum_operators(>=) \
        __friend_rbool_enum_operators(<=) \
    template <typename TT> \
    friend rbool operator==(const persisted<std::optional<TT>>& a, const std::optional<TT>& b); \
    template <typename TT, typename VV> \
    friend rbool operator==(const persisted<std::optional<TT>>& a, const VV& b);                \
    template <typename S> \
    friend inline persisted<std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration>>& operator +=( \
        persisted<std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration>>& lhs, \
        const std::chrono::duration<S>& rhs);   \
    template <typename, typename> \
    friend struct persisted;\
    template <typename U> \
    friend rbool inline operator==(const persisted<std::optional<U>>& a, const char* b);               \
    template <typename> \
    friend struct box_base; \
    template <typename> \
    friend class persisted_map_base;      \
    friend struct internal::bridge::obj;  \
    friend struct internal::bridge::list;

#define __cpp_realm_generate_operator(type, op, name) \
    inline rbool operator op(const persisted<type>& a, const type& b) { \
        if (a.should_detect_usage_for_queries) { \
            auto query = internal::bridge::query(a.query->get_table()); \
            auto serialized = persisted<type>::serialize(b);     \
            query.name(a.managed, serialized);    \
            return query; \
        } \
        return *a op b; \
    } \
    inline rbool operator op(const persisted<type>& a, const persisted<type>& b) { \
        return a op *b; \
    }

namespace realm {
    namespace {
        template<typename T>
        using is_optional = internal::type_info::is_optional<T>;
    }
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

template <typename T>
struct persisted_base {
    static_assert(std::is_destructible_v<T>);

    using Result = T;
    persisted_base() = default;
    persisted_base(const persisted_base& o) {
        *this = o;
    }
    persisted_base(persisted_base&& o) {
        *this = std::move(o);
    }

//    virtual std::conditional_t<is_optional<T>::value,
//            typename is_optional<T>::underlying,
//            typename is_optional<T>::underlying> operator *() const {
//        if (this->is_managed()) {
//            auto val = persisted<T>::deserialize(this->m_object->obj().template
//                    get<typename internal::type_info::type_info<T>::internal_type>(this->managed));
//            if constexpr (is_optional<T>::value) {
//                return *val;
//            } else {
//                return val;
//            }
//        } else {
//            if constexpr (is_optional<T>::value) {
//                return *this->unmanaged;
//            } else {
//                return this->unmanaged;
//            }
//        }
//    }
protected:
    [[nodiscard]] bool is_managed() const {
        return m_object;
    }

    virtual void manage(internal::bridge::object* object,
                        internal::bridge::col_key&& col_key) = 0;

    internal::bridge::object *m_object;

    // MARK: Queries
    bool should_detect_usage_for_queries = false;
    internal::bridge::query* query;
    bool is_and;
    bool is_or;

    void prepare_for_query(internal::bridge::query& query_builder) {
        should_detect_usage_for_queries = true;
        query = &query_builder;
    }

    __cpp_realm_friends
};

    template <typename T>
    struct persisted_primitive_base : public persisted_base<T> {
        using persisted_base<T>::persisted_base;
        persisted_primitive_base() {
            new (&unmanaged) T();
        }
        persisted_primitive_base(const persisted_primitive_base& v) {
            if (v.is_managed()) {
                this->m_object = v.m_object;
                managed = v.managed;
            } else {
                unmanaged = v.unmanaged;
            }
        }
        persisted_primitive_base(const T& value) {
            unmanaged = value;
        }
        persisted_primitive_base(T&& value) {
            unmanaged = std::move(value);
        }
        ~persisted_primitive_base() {
            if (this->is_managed()) {
                managed.~col_key();
            } else {
                unmanaged.~T();
            }
        }

        persisted_primitive_base& operator=(const T& o) noexcept {
            if (this->is_managed()) {
                this->m_object->obj().template set(managed, serialize(o));
            } else {
                new (&unmanaged) T(o);
            }
            return *this;
        }

        persisted_primitive_base& operator=(T&& o) noexcept {
            if (this->is_managed()) {
                this->m_object->obj().set(managed, o);
            } else {
                new (&unmanaged) T(std::move(o));
            }
            return *this;
        }

        persisted_primitive_base& operator=(const persisted_primitive_base& o) noexcept {
            if (o.m_object) {
                this->m_object = o.m_object;
                new (&managed) internal::bridge::col_key(o.managed);
            } else {
                new (&unmanaged) T(o.unmanaged);
            }
            return *this;
        }

        persisted_primitive_base& operator=(persisted_primitive_base&& o) noexcept {
            if (o.m_object) {
                this->m_object = o.m_object;
                new (&managed) internal::bridge::col_key(std::move(o.managed));
            } else {
                new (&unmanaged) T(std::move(o.unmanaged));
            }
            return *this;
        }

        T operator *() const {
            if (this->is_managed()) {
                return persisted<T>::deserialize(this->m_object->obj()
                        .template get<typename internal::type_info::type_info<T>::internal_type>(this->managed));
            } else {
                return this->unmanaged;
            }
        }
    protected:
        union {
            T unmanaged;
            internal::bridge::col_key managed;
        };

        void manage(internal::bridge::object* object,
                    internal::bridge::col_key&& col_key) final {
            object->obj().set(col_key, persisted<T>::serialize(unmanaged));
            this->m_object = object;
            this->managed = col_key;
        }
    };

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
    friend struct results;

    friend rbool operator ||(const rbool& lhs, const rbool& rhs);

    __cpp_realm_friends;
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
inline std::ostream& operator<< (std::ostream& stream, const persisted_primitive_base<T>& value)
{
    return stream << *value;
}
//
//template <typename T>
//std::ostream& operator<< (std::ostream& stream, const persisted<std::optional<T>>& value)
//{
//    if (*value) {
//        return stream << "null";
//    }
//    return stream << type_info::persisted_type<T>::convert_if_required(**value);
//}

template <typename T>
inline typename std::enable_if<std::is_base_of<realm::object_base, T>::value, std::ostream>::type&
operator<< (std::ostream& stream, const T& object)
{
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
//
//template <typename T>
//typename std::enable_if_t<type_info::BinaryPersistableConcept<T>::value, std::ostream>&
//        operator<< (std::ostream& stream, const T& binary)
//{
//    return stream << binary;
//}

}

#endif /* realm_persisted_hpp */
