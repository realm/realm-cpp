#ifndef CPPREALM_RBOOL_HPP
#define CPPREALM_RBOOL_HPP

#include <cpprealm/schema.hpp>
#include <cpprealm/internal/bridge/query.hpp>

namespace realm::experimental {
    template<typename>
    struct box_base;
}
#define __friend_rbool_operators__(type, op) \
        friend rbool operator op(const persisted<type, void>& a, const type& b); \

#define __friend_rbool_numeric_operators(type, op) \
    template <typename V> \
    friend std::enable_if_t<std::negation_v<std::is_same<V, bool>>, rbool> operator op(const persisted<type, void>& a, const V& b);

#define __friend_rbool_timestamp_operators__(op) \
        template <typename VV> \
        friend rbool operator op(const persisted<std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration>, void>& a, const VV& b); \

#define __friend_rbool_mixed_operators(op) \
    template <template <typename ...> typename Variant, typename ...Ts, typename V> \
    std::enable_if_t<internal::type_info::is_variant_t<Variant<Ts...>>::value, rbool> \
    friend operator op(const persisted<Variant<Ts...>, void>& a, V&& b);

#define __friend_rbool_nullable_mixed_operators(op) \
    template <template <typename ...> typename Variant, typename ...Ts> \
    std::enable_if_t<internal::type_info::is_variant_t<Variant<Ts...>>::value, rbool> \
    friend operator op(const persisted<Variant<Ts...>, void>& a, const std::nullopt_t& b);

#define __friend_rbool_const_char_operators(op) \
    friend rbool operator op(const persisted<std::string, void>& a, const char* b);

#define __friend_rbool_enum_operators(op) \
    template <typename TT, typename VV>                                      \
    friend std::enable_if_t<std::is_enum_v<TT>, rbool> operator op(const persisted<TT, void>& a, const VV& b);

#define __cpp_realm_friends \
        template <typename> \
        friend struct realm::experimental::box_base; \
        template <typename ...> \
        friend struct db;                    \
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
        __friend_rbool_operators__(bool, !=) \
                            \
        __friend_rbool_operators__(uuid, >)  \
        __friend_rbool_operators__(uuid, <) \
        __friend_rbool_operators__(uuid, >=)   \
        __friend_rbool_operators__(uuid, <=) \
        __friend_rbool_operators__(uuid, ==) \
        __friend_rbool_operators__(uuid, !=)  \
                            \
        __friend_rbool_operators__(object_id, >)  \
        __friend_rbool_operators__(object_id, <) \
        __friend_rbool_operators__(object_id, >=)   \
        __friend_rbool_operators__(object_id, <=) \
        __friend_rbool_operators__(object_id, ==) \
        __friend_rbool_operators__(object_id, !=)    \
                            \
        __friend_rbool_operators__(decimal128, >)  \
        __friend_rbool_operators__(decimal128, <) \
        __friend_rbool_operators__(decimal128, >=)   \
        __friend_rbool_operators__(decimal128, <=) \
        __friend_rbool_operators__(decimal128, ==) \
        __friend_rbool_operators__(decimal128, !=) \
                            \
        __friend_rbool_operators__(std::string, !=) \
        __friend_rbool_operators__(std::string, ==) \
                            \
        __friend_rbool_const_char_operators(==) \
        __friend_rbool_const_char_operators(!=) \
                            \
        __friend_rbool_operators__(std::vector<uint8_t>, !=) \
        __friend_rbool_operators__(std::vector<uint8_t>, ==) \
        __friend_rbool_operators__(std::chrono::time_point<std::chrono::system_clock>, ==) \
        __friend_rbool_operators__(std::chrono::time_point<std::chrono::system_clock>, !=)\
        __friend_rbool_operators__(std::chrono::time_point<std::chrono::system_clock>, >)   \
        __friend_rbool_operators__(std::chrono::time_point<std::chrono::system_clock>, <) \
        __friend_rbool_operators__(std::chrono::time_point<std::chrono::system_clock>, >=)\
        __friend_rbool_operators__(std::chrono::time_point<std::chrono::system_clock>, <=)\
        __friend_rbool_mixed_operators(==) \
        __friend_rbool_mixed_operators(!=)   \
        __friend_rbool_nullable_mixed_operators(==) \
        __friend_rbool_nullable_mixed_operators(!=) \
                            \
        __friend_rbool_enum_operators(==) \
        __friend_rbool_enum_operators(!=) \
        __friend_rbool_enum_operators(>) \
        __friend_rbool_enum_operators(<) \
        __friend_rbool_enum_operators(>=) \
        __friend_rbool_enum_operators(<=)    \
                            \
    template <typename TT> \
    friend rbool operator==(const persisted<std::optional<TT>, void>& a, const std::nullopt_t& b); \
    template <typename TT> \
    friend rbool operator!=(const persisted<std::optional<TT>, void>& a, const std::nullopt_t& b); \
    template <typename TT> \
    friend rbool operator==(const persisted<std::optional<TT>, void>& a, const std::optional<TT>& b); \
    template <typename TT, typename VV> \
    friend rbool operator==(const persisted<std::optional<TT>, void>& a, const VV& b);                \
    template <typename S> \
    friend inline persisted<std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration>, void>& operator +=( \
        persisted<std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration>, void>& lhs, \
        const std::chrono::duration<S>& rhs);   \
    template <typename, typename> \
    friend struct persisted;\
    template <typename U> \
    friend rbool inline operator==(const persisted<std::optional<U>, void>& a, const char* b);               \
    template <typename> \
    friend struct box_base; \
    template <typename> \
    friend class persisted_map_base;      \
    friend struct internal::bridge::obj;  \
    friend struct internal::bridge::list;    \
    friend struct internal::bridge::mixed; \
    template <typename> friend struct object_base;\
    template <typename> friend struct asymmetric_object;  \
    template <typename, typename> friend struct results;                  \
    template <typename TT> \
    friend inline typename std::enable_if<std::is_base_of<realm::object_base<TT>, TT>::value, std::ostream>::type& \
    operator<< (std::ostream& stream, const TT& object); \
    template <typename, typename> friend struct thead_safe_reference;                      \

namespace realm {
    // MARK: rbool

    class rbool {
        bool is_for_queries = false;
        friend rbool operator&&(const rbool &lhs, const rbool &rhs);

        template<typename T, typename>
        friend struct results;

        friend rbool operator||(const rbool &lhs, const rbool &rhs);

        __cpp_realm_friends;

    public:
        ~rbool() {
            if (is_for_queries)
                q.~query();
        }
        operator bool() const {
            return b;
        }
        rbool operator!() const {
            if (is_for_queries) {
                new (&q) internal::bridge::query(q.negate());
                return *this;
            }
            return !b;
        }
        union {
            bool b;
            mutable internal::bridge::query q;
        };

        rbool(internal::bridge::query &&q) : q(q), is_for_queries(true) {}
        rbool(bool b) : b(b) {}
        rbool(const rbool &r) {
            if (r.is_for_queries) {
                new (&q) internal::bridge::query(r.q);
                is_for_queries = true;
            } else
                b = r.b;
        }
    };
}

#endif //CPPREALM_RBOOL_HPP
