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

#ifndef realm_type_info_hpp
#define realm_type_info_hpp

#include <optional>
#include <variant>

#include <realm/object-store/property.hpp>
#include <realm/obj.hpp>

namespace realm {
    using uuid = realm::UUID;
    struct object_base;
    struct object;
    struct embedded_object;
}
namespace realm::type_info {


template <typename T>
using AddAssignable = typename std::enable_if<
    T::operator +=
>::type;
template <typename T>
using ComparableConcept = std::conjunction<
        std::is_integral<decltype(std::declval<T>() > std::declval<T>())>,
        std::is_integral<decltype(std::declval<T>() < std::declval<T>())>
>;
template <typename, typename = void>
struct persisted_type;

// MARK: Int
template <typename T>
using IntPersistableConcept =
    std::conjunction<
            std::negation<std::is_same<T, bool>>,
            std::is_integral<T>,
            std::is_convertible<Int, T>>;
template <typename T>
using IntPersistable = std::enable_if_t<IntPersistableConcept<T>::value>;
template <typename T>
struct persisted_type<T, IntPersistable<T>> {
    using type = realm::Int;
    static constexpr PropertyType property_type() {
        return PropertyType::Int;
    }
    static constexpr type convert_if_required(const T& a)
    {
        return a;
    }
};

// MARK: Bool
template <typename T>
using BoolPersistableConcept = std::is_same<T, bool>;
template <typename T>
using BoolPersistable = std::enable_if_t<BoolPersistableConcept<T>::value>;
template <typename T>
struct persisted_type<T, BoolPersistable<T>> {
    using type = realm::Bool;
    static constexpr PropertyType property_type() {
        return PropertyType::Bool;
    }
    static constexpr type convert_if_required(const T& a)
    {
        return a;
    }
};

// MARK: Enum
template <typename T>
using EnumPersistableConcept =
        std::conjunction<
                std::is_enum<T>
                >;
template <typename T>
using EnumPersistable = std::enable_if_t<
        EnumPersistableConcept<T>::value
>;
template <typename T>
struct persisted_type<T, EnumPersistable<T>> {
    using type = realm::Int;
    static constexpr PropertyType property_type() {
        return PropertyType::Int;
    }
    static constexpr type convert_if_required(const T& a)
    {
        return static_cast<std::underlying_type_t<T>>(a);
    }
};

// MARK: String
template <typename T>
using StringPersistableConcept = std::conjunction<
        std::is_convertible<std::string, T>,
        std::is_convertible<const char*, T>,
        std::is_constructible<std::string_view, T>,
        std::is_default_constructible<T>
>;
template <typename T>
using StringPersistable = std::enable_if_t<
        StringPersistableConcept<T>::value
>;
template <typename T>
struct persisted_type<T, StringPersistable<T>> {
    using type = StringData;

    static constexpr PropertyType property_type() {
        return PropertyType::String;
    }
    static type convert_if_required(const T& a) {
        return static_cast<std::string_view>(a);
    }
};

// MARK: Double
template <typename T>
using DoublePersistableConcept =
        std::conjunction<std::is_floating_point<T>, std::is_convertible<realm::Double, T>>;
template <typename T>
using DoublePersistable = std::enable_if_t<DoublePersistableConcept<T>::value>;
template <typename T>
struct persisted_type<T, DoublePersistable<T>> {
    using type = double;
    static constexpr PropertyType property_type() {
        return PropertyType::Double;
    }
    static type convert_if_required(const T& a) {
        return a;
    }
};

// MARK: Timestamp
template <typename C>
using TimestampPersistableConcept =
        std::is_constructible<realm::Timestamp, C>;
template <typename C = std::chrono::system_clock, typename D = typename C::duration>
using TimestampPersistable = std::enable_if_t<
        TimestampPersistableConcept<std::chrono::time_point<C, D>>::value
>;
template <typename T>
struct persisted_type<T, TimestampPersistable<T>> {
    using type = realm::Timestamp;
    static constexpr PropertyType property_type() {
        return PropertyType::Date;
    }
    static type convert_if_required(const T& a) {
        return a;
    }
};

// MARK: UUID
template <typename T>
using UUIDPersistableConcept = std::is_same<T, uuid>;
template <typename T>
using UUIDPersistable = std::enable_if_t<UUIDPersistableConcept<T>::value>;
template <typename T>
struct persisted_type<T, UUIDPersistable<T>> {
    using type = realm::UUID;
    static constexpr PropertyType property_type() {
        return PropertyType::UUID;
    }
    static type convert_if_required(const T& a) {
        return a;
    }
};

// MARK: Binary
template <typename T>
using BinaryPersistableConcept = std::is_same<T, std::vector<std::uint8_t>>;
template <typename T>
using BinaryPersistable = std::enable_if_t<BinaryPersistableConcept<T>::value>;

template <typename T>
struct persisted_type<T, BinaryPersistable<T>> {
    using type = realm::BinaryData;
    static constexpr PropertyType property_type() {
        return PropertyType::Data;
    }
    static type convert_if_required(const T& a)
    {
        if (a.empty()) {
            return BinaryData("");
        }
        return BinaryData(reinterpret_cast<const char *>(a.data()), a.size());
    }
};


template <class T>
struct is_optional : std::false_type {
    using type = std::false_type::value_type;
    static constexpr bool value = std::false_type::value;
};

template <class T>
struct is_optional<std::optional<T>> : std::true_type {
    using type = std::true_type::value_type;
    static constexpr bool value = std::true_type::value;
};

template <typename T>
using Optional = typename std::enable_if<is_optional<T>::value>::type;

// MARK: Object
template <typename T>
using ObjectBasePersistableConcept = std::is_base_of<realm::object_base, T>;
template <typename T>
using ObjectBasePersistable = std::enable_if_t<
    ObjectBasePersistableConcept<T>::value
>;
template <typename T>
using ObjectPersistableConcept = std::conjunction<
        ObjectBasePersistableConcept<T>, std::is_base_of<realm::object, T>>;
template <typename T>
using ObjectPersistable = std::enable_if_t<
        ObjectPersistableConcept<T>::value, T
>;
template <typename T>
using EmbeddedObjectPersistableConcept = std::conjunction<
    std::is_base_of<realm::embedded_object, T>
>;
template <typename T>
using EmbeddedObjectPersistable = typename std::enable_if<ObjectBasePersistableConcept<T>::value &&
    std::is_base_of_v<realm::embedded_object, T>
>::type;
template <typename T>
struct persisted_type<T, ObjectBasePersistable<T>> {
    using type = realm::ObjKey;
    static constexpr type convert_if_required(const T& a) {
        return a.m_object ? a.m_object->obj().get_key() : ObjKey{};
    }
    static constexpr PropertyType property_type() {
        return PropertyType::Object | PropertyType::Nullable;
    }
};
template <typename T>
using PrimitivePersistableConcept =
        std::conjunction<std::negation<is_optional<T>>,
        std::disjunction<
        IntPersistableConcept<T>,
        BoolPersistableConcept<T>,
        StringPersistableConcept<T>,
        EnumPersistableConcept<T>,
        DoublePersistableConcept<T>,
        TimestampPersistableConcept<T>,
        UUIDPersistableConcept<T>,
        BinaryPersistableConcept<T>>>;
template <typename T>
using PrimitivePersistable = std::enable_if_t<PrimitivePersistableConcept<T>::value>;

    namespace {
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
                if constexpr (!PrimitivePersistableConcept<std::variant_alternative_t<N, Variant>>::value) {
                    return false;
                } else {
                    return check_variant_types<N + 1, Variant>();
                }
            }
        }
    }
    template <typename T>
    using MixedPersistableConcept =
            std::conjunction<is_variant_t<T>,
                    std::conditional_t<check_variant_types<0, T>(), std::true_type, std::false_type>
    >;
    template <typename T>
    using MixedPersistable = std::enable_if_t<MixedPersistableConcept<T>::value>;
    static_assert(MixedPersistableConcept<std::variant<std::string, int, bool>>::value);

    template<size_t N, typename Variant>
    constexpr Variant check_if_returnable(const Mixed& mixed) {
        if constexpr (N == std::variant_size_v<Variant>) {
            return Variant{};
        } else {
            if constexpr (IntPersistableConcept<std::variant_alternative_t<N, Variant>>::value) {
                if (mixed.get_type().m_type == DataType::Type::Int) {
                    return mixed.get_int();
                } else {
                    return check_if_returnable<N + 1, Variant>(mixed);
                }
            } else if constexpr (BoolPersistableConcept<std::variant_alternative_t<N, Variant>>::value) {
                if (mixed.get_type().m_type == DataType::Type::Bool) {
                    return mixed.get_bool();
                } else {
                    return check_if_returnable<N + 1, Variant>(mixed);
                }
            } else if constexpr (StringPersistableConcept<std::variant_alternative_t<N, Variant>>::value) {
                if (mixed.get_type().m_type == DataType::Type::String) {
                    return mixed.get_string();
                } else {
                    return check_if_returnable<N + 1, Variant>(mixed);
                }
            } else if constexpr (TimestampPersistableConcept<std::variant_alternative_t<N, Variant>>::value) {
                if (mixed.get_type().m_type == DataType::Type::Timestamp) {
                    return mixed.get_timestamp().template get_time_point();
                } else {
                    return check_if_returnable<N + 1, Variant>(mixed);
                }
            } else if constexpr (BinaryPersistableConcept<std::variant_alternative_t<N, Variant>>::value) {
                if (mixed.get_type().m_type == DataType::Type::Binary) {
                    return std::vector<uint8_t>((unsigned char*)mixed.get_binary().data(),
                                                (unsigned char*)mixed.get_binary().data() + mixed.get_binary().size());
                } else {
                    return check_if_returnable<N + 1, Variant>(mixed);
                }
            } else if constexpr (DoublePersistableConcept<std::variant_alternative_t<N, Variant>>::value) {
                if (mixed.get_type().m_type == DataType::Type::Double) {
                    return mixed.get_double();
                } else {
                    return check_if_returnable<N + 1, Variant>(mixed);
                }
            } else if constexpr (UUIDPersistableConcept<std::variant_alternative_t<N, Variant>>::value) {
                if (mixed.get_type().m_type == DataType::Type::UUID) {
                    return mixed.get_uuid();
                } else {
                    return check_if_returnable<N + 1, Variant>(mixed);
                }
            } else if constexpr (EnumPersistableConcept<std::variant_alternative_t<N, Variant>>::value) {
                if (mixed.get_type().m_type == DataType::Type::Int) {
                    return mixed.get_int();
                } else {
                    return check_if_returnable<N + 1, Variant>(mixed);
                }
            } else {
                return check_if_returnable<N + 1, Variant>(mixed);
            }
        }
    }

    template <typename Variant>
    constexpr Variant mixed_to_variant(const Mixed& mixed) {
        return check_if_returnable<0, Variant>(mixed);
    }
    template<class> inline constexpr bool always_false_v = false;
    template <typename T>
    struct persisted_type<T, MixedPersistable<T>> {
        using type = realm::Mixed;
        static type convert_if_required(const T& a) {
            return std::visit([](auto&& arg) {
                using M = std::decay_t<decltype(arg)>;
                if constexpr (PrimitivePersistableConcept<M>::value)
                    return Mixed(persisted_type<M>::convert_if_required(arg));
                else
                    static_assert(always_false_v<M>, "non-exhaustive visitor!");
            }, a);
        }
        static constexpr PropertyType property_type() {
            return PropertyType::Mixed | PropertyType::Nullable;
        }
    };

template <typename T>
using NonOptionalPersistableConcept =
        std::disjunction<PrimitivePersistableConcept<T>, ObjectBasePersistableConcept<T>, MixedPersistableConcept<T>>;
template <typename T>
using NonOptionalPersistable = std::enable_if_t<NonOptionalPersistableConcept<T>::value>;

//// MARK: ListPersistable
namespace {
    template <typename T, typename = void>
    struct is_vector : std::false_type {};
    template <typename T>
    struct is_vector<std::vector<T>> : std::true_type {
    };
}
template <typename T>
using ListPrimitivePersistableConcept = std::conjunction<
        std::negation<std::is_same<typename T::value_type, std::uint8_t>>,
        std::is_same<std::vector<typename T::value_type>, T>,
        PrimitivePersistableConcept<typename T::value_type>
>;
    template <typename T>
    using ListObjectPersistableConcept = std::conjunction<
            std::is_same<std::vector<typename T::value_type>, T>,
            ObjectBasePersistableConcept<typename T::value_type>
    >;
template <typename T>
using ListPrimitivePersistable = std::enable_if_t<
        ListPrimitivePersistableConcept<T>::value
>;
template <typename T>
using ListObjectPersistable = std::enable_if_t<
        ListObjectPersistableConcept<T>::value
>;
template <typename T>
constexpr auto strip_list_check() {
    if constexpr (is_vector<T>::value) {
        return !std::is_same_v<typename T::value_type, std::uint8_t> &&
                NonOptionalPersistableConcept<typename T::value_type>::value &&
                std::is_same_v<std::vector<typename T::value_type>, T>;
    } else {
        return false;
    }
}
template <typename T>
using ListPersistableConcept = std::conditional_t<
        strip_list_check<T>(), std::true_type, std::false_type
>;
static_assert(ListPersistableConcept<std::vector<int>>::value);
template <typename T>
using ListPersistable = std::enable_if_t<
       ListPersistableConcept<T>::value
>;
template <typename T>
struct persisted_type<T, ListPersistable<T>> {
    using type = std::vector<typename persisted_type<typename T::value_type>::type>;

    static constexpr PropertyType property_type() {
        if constexpr (MixedPersistableConcept<typename T::value_type>::value) {
            return PropertyType::Array | (persisted_type<typename T::value_type>::property_type());
        } else {
            return PropertyType::Array |
                   (persisted_type<typename T::value_type>::property_type() & ~PropertyType::Flags);
        }
    }

    static constexpr type convert_if_required(const T& a)
    {
        typename persisted_type<T>::type v;

        if constexpr (BoolPersistableConcept<typename T::value_type>::value) {
            std::transform(a.begin(), a.end(), std::back_inserter(v), [](auto value) {
                return static_cast<typename persisted_type<typename T::value_type>::type>(value);
            });
        } else {
            std::transform(a.begin(), a.end(), std::back_inserter(v), [](auto& value) {
                if constexpr (ObjectBasePersistableConcept<typename T::value_type>::value) {
                    return value.m_obj->get_key();
                } else if constexpr (BinaryPersistableConcept<T>::value) {
                    return BinaryData(reinterpret_cast<const char *>(value.data()), value.size());
                } else {
                    return persisted_type<typename T::value_type>::convert_if_required(value);
                }
            });
        }
        return v;
    }
};

template <typename T>
using OptionalObjectPersistableConcept = std::conjunction<
        is_optional<T>, ObjectPersistableConcept<typename T::value_type>>;

template <typename T>
using OptionalObjectPersistable = typename std::enable_if<
        Optional<T>::value && ObjectPersistable<typename T::value_type>::value>::type;


template <typename T>
constexpr auto strip_optional_check() {
    if constexpr (is_optional<T>::value) {
        return NonOptionalPersistableConcept<typename T::value_type>::value;
    } else {
        return false;
    }
}

template <typename T>
using OptionalPersistableConcept =
            typename std::conditional<strip_optional_check<T>(), std::true_type, std::false_type>::type;
template <typename T>
using OptionalPersistable = std::enable_if_t<OptionalPersistableConcept<T>::value>;
template <typename T>
struct persisted_type<T, OptionalPersistable<T>> {
    using type = util::Optional<typename persisted_type<typename T::value_type>::type>;

    static constexpr PropertyType property_type() {
        return persisted_type<typename T::value_type>::property_type() | PropertyType::Nullable;
    }
    static constexpr type convert_if_required(const T& a)
    {
        if (!a) {
            return util::none;
        }
        return type(persisted_type<typename T::value_type>::convert_if_required(*a));
    }
};
template <typename T>
struct persisted_type<T, std::optional<ObjectBasePersistable<T>>> {
    using type = realm::ObjKey;
};
template <typename T>
using NonContainerPersistableConcept = std::disjunction<
        MixedPersistableConcept<T>,
        NonOptionalPersistableConcept<T>,
        OptionalPersistableConcept<T>
>;
template <typename T>
using NonContainerPersistable = typename std::enable_if<NonContainerPersistableConcept<T>::value>::type;
template <typename T>
using PersistableConcept = std::disjunction<
        NonContainerPersistableConcept<T>,
        ListPersistableConcept<T>
>;

template <typename T>
using Persistable = std::enable_if_t<PersistableConcept<T>::value>;

template <typename T>
using Propertyable = typename std::enable_if<
    std::is_same_v<std::string, decltype(std::declval<T>().name)> &&
    std::is_same_v<typename T::Result T::Class::*, decltype(std::declval<T>().ptr)> &&
    std::is_same_v<PropertyType, decltype(std::declval<T>().type)>,
    T
>::type;
} // namespace realm::type_info

#endif /* realm_type_info_hpp */
