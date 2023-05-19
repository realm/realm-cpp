#ifndef REALM_TYPE_INFO_HPP
#define REALM_TYPE_INFO_HPP

#include <cpprealm/internal/bridge/property.hpp>
#include <cpprealm/internal/bridge/uuid.hpp>
#include <cpprealm/internal/bridge/binary.hpp>
#include <cpprealm/internal/bridge/timestamp.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/obj_key.hpp>
#include <cpprealm/internal/bridge/list.hpp>
#include <cpprealm/internal/bridge/dictionary.hpp>
#include <cpprealm/internal/bridge/object_id.hpp>

#include <vector>
#include <map>

namespace realm::experimental {
    template <typename>
    struct link;
}
namespace realm::internal::type_info {
    template <typename T, typename = void>
    struct is_optional : std::false_type {
        using underlying = T;
    };
    template <template <typename> typename Optional, typename T>
    struct is_optional<Optional<T>,
                       std::enable_if_t<std::is_same_v<std::optional<T>, Optional<T>>>> : std::true_type {
        using underlying = T;
    };
    template <typename T, typename = void>
    struct is_vector : std::false_type {
        static constexpr auto value = false;
    };
    template <typename T>
    struct is_vector<std::vector<T>> : std::true_type {
        static constexpr auto value = true;
    };
    template <typename T, typename = void>
    struct is_map : std::false_type {
        using value_type = T;
    };
    template <typename T>
    struct is_map<std::map<std::string, T>> : std::true_type {
        using value_type = T;
    };
    template <typename T, typename = void>
    struct persisted;
    template <typename T, typename = void>
    struct is_persisted : std::false_type {
    };
    template <typename T>
    struct is_persisted<persisted<T>> : std::true_type {
    };

    template <typename T, typename = void>
    struct type_info;

    namespace {
        template <typename T>
        using is_primitive = std::negation<std::disjunction<
                std::is_same<typename type_info<T>::internal_type, std::optional<bridge::obj_key>>,
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
                if constexpr (std::is_same_v<std::variant_alternative_t<N, Variant>, std::monostate>) {
                    return check_variant_types<N + 1, Variant>();
                } else if constexpr (is_primitive<std::variant_alternative_t<N, Variant>>::value) {
                    return check_variant_types<N + 1, Variant>();
                } else if constexpr (std::is_base_of_v<object<std::variant_alternative_t<N, Variant>>, std::variant_alternative_t<N, Variant>>) {
                    return check_variant_types<N + 1, Variant>();
                } else {
                    return false;
                }
            }
        }

        template<typename T>
        using MixedPersistableConcept =
                std::conjunction<is_variant_t<T>,
                                 std::conditional_t<check_variant_types<0, T>(), std::true_type, std::false_type>
                                 >;

        namespace {
            static_assert(std::conjunction<
                          std::is_convertible<int, int64_t>,
                          std::is_constructible<int, int64_t>>::value);
            static_assert(std::conjunction<
                          std::is_convertible<const char*, std::string>,
                          std::is_constructible<std::string, const char*>>::value);

            template <class Custom, class Underlying>
            using is_persistable = std::conjunction<
                    std::is_convertible<Custom, Underlying>,
                    std::is_constructible<Custom, Underlying>,
                    std::negation<std::is_same<Custom, Underlying>>>;
            template <class Custom>
            using is_int_persistable = std::conjunction<
                    std::is_convertible<Custom, int64_t>,
                    std::is_constructible<Custom, int64_t>,
                    std::negation<std::is_same<Custom, bool>>,
                    std::negation<std::is_same<Custom, int64_t>>>;
            template <class Custom>
            using is_double_persistable = std::conjunction<
                    std::is_convertible<Custom, double>,
                    std::is_constructible<Custom, double>,
                    std::negation<std::is_same<Custom, double>>>;
            template <class Custom>
            using is_string_persistable = is_persistable<Custom, std::string>;
            template <class Custom>
            using is_bool_persistable = std::conjunction<
                    std::is_convertible<Custom, bool>,
                    std::is_constructible<Custom, bool>,
                    std::negation<std::is_integral<Custom>>,
                    std::negation<std::is_same<Custom, bool>>>;
            template <class Custom>
            using is_uuid_persistable = is_persistable<Custom, uuid>;
            template <class Custom>
            using is_object_id_persistable = is_persistable<Custom, object_id>;
            template <class Custom>
            using is_binary_persistable = is_persistable<Custom, std::vector<uint8_t>>;
            template <class Custom, typename C = std::chrono::system_clock, typename D = typename C::duration>
            using is_time_point_persistable = is_persistable<Custom, std::chrono::time_point<C, D>>;
            template <class Custom>
            using is_null_persistable = is_persistable<Custom, std::monostate>;

            template <typename Custom, typename = void>
            struct is_custom_persistable : std::false_type {};
            template <typename Custom>
            struct is_custom_persistable<Custom, std::enable_if_t<is_string_persistable<Custom>::value>> :
                std::true_type {
                using underlying = std::string;
            };
            template <typename Custom>
            struct is_custom_persistable<Custom, std::enable_if_t<is_int_persistable<Custom>::value>> :
                std::true_type {
                using underlying = int64_t;
            };
            template <typename Custom>
            struct is_custom_persistable<Custom, std::enable_if_t<is_double_persistable<Custom>::value>> :
                std::true_type {
                using underlying = double;
            };
            template <typename Custom>
            struct is_custom_persistable<Custom, std::enable_if_t<is_bool_persistable<Custom>::value>> :
                std::true_type {
                using underlying = bool;
            };
            template <typename Custom>
            struct is_custom_persistable<Custom, std::enable_if_t<is_uuid_persistable<Custom>::value>> :
                std::true_type {
                using underlying = uuid;
            };
            template <typename Custom>
            struct is_custom_persistable<Custom, std::enable_if_t<is_object_id_persistable<Custom>::value>> :
                std::true_type {
                using underlying = object_id;
            };
            template <typename Custom>
            struct is_custom_persistable<Custom, std::enable_if_t<is_binary_persistable<Custom>::value>> :
                std::true_type {
                using underlying = std::vector<uint8_t>;
            };
            template <typename Custom>
            struct is_custom_persistable<Custom, std::enable_if_t<is_time_point_persistable<Custom>::value>> :
                std::true_type {
                using underlying = std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration>;
            };
            template <typename Custom>
            struct is_custom_persistable<Custom, std::enable_if_t<is_null_persistable<Custom>::value>> :
                std::true_type {
                using underlying = std::monostate;
            };
        }
    }

    template <typename T>
    struct type_info<experimental::link<T>> {
        using internal_type = bridge::obj_key;
        static constexpr bridge::property::type type() {
            return bridge::property::type::Object;
        }
    };
    template <typename T>
    struct is_link : std::false_type {
        static constexpr auto value = false;
    };
    template <typename T>
    struct is_link<experimental::link<T>> : std::true_type {
        static constexpr auto value = true;
    };

    template <>
    struct type_info<std::monostate> {
        using internal_type = std::monostate;
        static constexpr bridge::property::type type() {
            return bridge::property::type::Mixed;
        }
    };
    template <>
    struct type_info<std::string> {
        using internal_type = std::string;
        static constexpr bridge::property::type type() {
            return bridge::property::type::String;
        }
    };
    template <>
    struct type_info<const char*> {
        using internal_type = std::string;
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
    struct type_info<int> {
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
    struct type_info<object_id> {
        using internal_type = bridge::object_id;
        static constexpr bridge::property::type type() {
            return bridge::property::type::ObjectId;
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
            return bridge::property::type::Mixed | bridge::property::type::Nullable;
        }
    };
    template <typename T>
    struct type_info<T, std::enable_if_t<std::is_base_of_v<object_base<T>, T>>> {
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
    struct type_info<std::vector<V>, std::enable_if_t<std::negation_v<std::is_same<V, uint8_t>>>> {
        using internal_type = bridge::list;
        static constexpr auto type() {
            return type_info<V>::type() | bridge::property::type::Array;
        }
    };
    template <typename T>
    struct type_info<T, std::enable_if_t<is_custom_persistable<T>::value>> :
        public type_info<typename is_custom_persistable<T>::underlying> {
    };
}
#endif //REALM_TYPE_INFO_HPP