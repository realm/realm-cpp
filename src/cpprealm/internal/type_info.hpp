#ifndef REALM_TYPE_INFO_HPP
#define REALM_TYPE_INFO_HPP

#include <cpprealm/internal/bridge/property.hpp>
#include <cpprealm/internal/bridge/uuid.hpp>
#include <cpprealm/internal/bridge/binary.hpp>
#include <cpprealm/internal/bridge/timestamp.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/obj_key.hpp>

#include <vector>
#include <cpprealm/type_info.hpp>

namespace realm::internal::type_info {
    namespace {
        template <typename T, typename = void>
        struct type_info;

        template <typename T>
        using is_primitive = std::negation<std::disjunction<
                std::is_same<typename type_info<T>::internal_type, bridge::obj_key>,
                std::is_same<typename type_info<T>::internal_type, bridge::list>,
                std::is_same<typename type_info<T>::internal_type, bridge::dictionary>
        >>;

        template <typename T, typename = void>
        struct is_variant_t : std::false_type {};
        template <template <typename ...> typename Variant, typename ...Ts>
        struct is_variant_t<Variant<Ts...>, std::enable_if_t<std::is_same_v<std::variant<Ts...>, Variant<Ts...>>>>
                : std::true_type {
        };

        template<size_t N, typename Variant>
        constexpr bool check_variant_types() {
            if constexpr (!is_variant_t<Variant>::value) {
                return false;
            } else if constexpr (N >= std::variant_size_v<Variant>) {
                return true;
            } else {
                if constexpr (!is_primitive<std::variant_alternative_t<N, Variant>>::value) {
                    return false;
                } else {
                    return check_variant_types<N + 1, Variant>();
                }
            }
        }

        template<typename T>
        using MixedPersistableConcept =
                std::conjunction<is_variant_t<T>,
                        std::conditional_t<check_variant_types<0, T>(), std::true_type, std::false_type>
                >;

        template <>
        struct type_info<std::string> {
            using internal_type = std::string_view;
            static constexpr bridge::property::type type() {
                return bridge::property::type::String;
            }
        };
        template <>
        struct type_info<const char*> {
            using internal_type = std::string_view;
            static constexpr bridge::property::type type() {
                return bridge::property::type::String;
            }
        };
        template <>
        struct type_info<int64_t> {
            using internal_type = int64_t;
            static constexpr bridge::property::type type() {
                return bridge::property::type::Int;
            }
        };
        template <>
        struct type_info<double> {
            using internal_type = double;
            static constexpr bridge::property::type type() {
                return bridge::property::type::Double;
            }
        };
        template <>
        struct type_info<bool> {
            using internal_type = bool;
            static constexpr bridge::property::type type() {
                return bridge::property::type::Bool;
            }
        };
        template <>
        struct type_info<uuid> {
            using internal_type = bridge::uuid;
            static constexpr bridge::property::type type() {
                return bridge::property::type::UUID;
            }
        };
        template <>
        struct type_info<std::vector<uint8_t>> {
            using internal_type = bridge::binary;
            static constexpr bridge::property::type type() {
                return bridge::property::type::Data;
            }
        };
        template <typename E>
        struct type_info<E, std::enable_if_t<std::is_enum_v<E>>> {
            using internal_type = int64_t;
            static constexpr bridge::property::type type() {
                return bridge::property::type::Int;
            }
        };
        template <typename ValueType>
        struct type_info<std::map<std::string, ValueType>> {
            using internal_type = internal::bridge::dictionary;
            static constexpr bridge::property::type type() {
                return bridge::property::type::Dictionary | type_info<ValueType>::type();
            }
        };
        template <typename T>
        struct type_info<T, std::enable_if_t<MixedPersistableConcept<T>::value>> {
            using internal_type = bridge::mixed;

            static constexpr auto type() {
                return bridge::property::type::Mixed;
            }
        };
        template <typename T>
        struct type_info<T, std::enable_if_t<std::is_base_of_v<object_base, T>>> {
            using internal_type = bridge::obj_key;
            static constexpr bridge::property::type type() {
                return bridge::property::type::Object;
            }
        };
        template <typename T>
        struct type_info<std::optional<T>> {
            using internal_type = std::optional<typename type_info<T>::internal_type>;
            static constexpr auto type() {
                return type_info<T>::type() | bridge::property::type::Nullable;
            }
        };
        template <typename C, typename D>
        struct type_info<std::chrono::time_point<C, D>> {
            using internal_type = bridge::timestamp;
            static constexpr auto type() {
                return bridge::property::type::Date;
            }
        };
        template <typename V>
        struct type_info<std::vector<V>> {
            using internal_type = bridge::list;
            static constexpr auto type() {
                return type_info<V>::type() | bridge::property::type::Array;
            }
        };

        namespace {
            static_assert(std::conjunction<
                    std::is_convertible<int, int64_t>,
                    std::is_constructible<int, int64_t>>::value);
            static_assert(std::conjunction<
                    std::is_convertible<const char*, std::string>,
                    std::is_constructible<std::string, const char*>>::value);
            template <typename Custom, typename Underlying, typename = void>
            struct is_custom : std::false_type {};
            template <typename Custom, typename Underlying>
            struct is_custom<Custom, Underlying, std::enable_if_t<std::conjunction<
                    std::is_convertible<Custom, Underlying>,
                    std::is_constructible<Custom, Underlying>,
                    std::negation<std::is_same<Custom, Underlying>>>::value>> : std::true_type {
                using underlying = Underlying;
            };
            template <typename Custom, typename = void>
            struct is_custom_type : is_custom<Custom, void> {};
            template <typename Custom>
            struct is_custom_type<Custom, std::enable_if_t<is_custom<Custom, std::string>::value>> :
                    is_custom<Custom, std::string> {
            };
            template <typename Custom>
            struct is_custom_type<Custom, std::enable_if_t<is_custom<Custom, int64_t>::value>> :
                    is_custom<Custom, int64_t> {
            };
            template <typename Custom>
            struct is_custom_type<Custom, std::enable_if_t<is_custom<Custom, bool>::value>> :
                    is_custom<Custom, bool> {
            };
            template <typename Custom>
            struct is_custom_type<Custom, std::enable_if_t<is_custom<Custom, uuid>::value>> :
                    is_custom<Custom, uuid> {
            };
            template <typename Custom>
            struct is_custom_type<Custom, std::enable_if_t<is_custom<Custom, std::vector<uint8_t>>::value>> :
                    is_custom<Custom, std::vector<uint8_t>> {
            };
            template <typename Custom>
            struct is_custom_type<Custom, std::enable_if_t<is_custom<Custom, std::chrono::time_point<std::chrono::system_clock>>::value>> :
                    is_custom<Custom, std::chrono::time_point<std::chrono::system_clock>> {
            };
        }
    }
    template <typename T>
    struct type_info<T, std::enable_if_t<is_custom_type<T>::value>> :
            public type_info<typename is_custom_type<T>::underlying> {
    };

    template <typename T, typename = void>
    typename type_info<std::decay_t<T>>::internal_type serialize(T);
    template <>
    std::string_view serialize(const char* c);
    template <>
    int64_t          serialize(int64_t);
    template <typename T>
    int64_t serialize(std::enable_if_t<std::is_integral_v<T>, T> v) {
        return static_cast<int64_t>(v);
    }

    template <>
    bool             serialize(bool c);
    template <>
    std::string_view serialize(const std::string&);
    template <>
    bridge::uuid     serialize(const realm::uuid& c);
    template <>
    bridge::binary   serialize(std::vector<uint8_t> v);
    template <typename T, std::enable_if_t<std::is_base_of_v<object_base, T>>>
    bridge::obj_key serialize(const T& o) {
        return o.m_object->obj().get_key();
    }
    template <typename C, typename D>
    bridge::timestamp serialize(const std::chrono::time_point<C, D>& ts) {
        return ts;
    }
    template <typename E, std::enable_if_t<std::is_enum_v<E>>>
    int64_t serialize(const E& e) {
        return static_cast<int64_t>(e);
    }

    template <typename T>
    std::vector<typename type_info<T>::internal_type> serialize(const std::vector<T>& v) {
        std::vector<typename type_info<T>::internal_type> v2;
        for (auto& a : v) {
            v2.push_back(serialize(a));
        }
        return v2;
    }
    template <typename T>
    std::optional<typename type_info<T>::internal_type> serialize(std::optional<T>&& c) {
        if (c) {
            return serialize<T>(*c);
        }
        return std::nullopt;
    }

    // MARK: Mixed
    template <typename ...Ts>
    bridge::mixed serialize(const std::variant<Ts...>& v) {
        return std::visit([](auto&& arg) {
            using M = std::decay_t<decltype(arg)>;
            return bridge::mixed(serialize<M>(arg));
        }, v);
    }
    template <typename T, std::enable_if_t<is_custom_type<T>::value>>
    typename type_info<typename is_custom_type<T>::underlying>::internal_type serialize(T s) {
        return serialize<typename is_custom_type<T>::underlying>(s);
    }

    template <typename T, typename = void>
    T deserialize(typename type_info<T>::internal_type&&);
    template <>
    int64_t deserialize(int64_t&&);
    template <>
    uuid deserialize(bridge::uuid&&);
    template <typename C, typename D>
    std::chrono::time_point<C, D> deserialize(bridge::timestamp&&);
    template <>
    std::string deserialize(std::string_view&&);
    template <>
    std::vector<uint8_t> deserialize(bridge::binary&&);

    template <typename T>
    T deserialize(const bridge::mixed& v) {
        return deserialize<T>(static_cast<typename type_info<T>::internal_type>(v));
    }
    template <typename C, typename D>
    std::chrono::time_point<C, D> deserialize(const bridge::timestamp & ts) {
        return ts.template get_time_point<C, D>();
    }
    template <typename T, typename V>
    std::enable_if_t<is_custom_type<V>::value, V> deserialize(const T& s) {
        return deserialize<typename is_custom_type<V>::underlying>(s);
    }
}
#endif //REALM_TYPE_INFO_HPP
