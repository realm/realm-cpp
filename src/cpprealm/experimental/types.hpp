#ifndef REALM_TYPES_HPP
#define REALM_TYPES_HPP

#include <cstdint>
#include <optional>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/type_info.hpp>

namespace realm {
    struct uuid {
        explicit uuid(const std::string &);
        uuid() = default;
        [[nodiscard]] std::string to_string() const;
        [[nodiscard]] std::string to_base64() const;
        [[nodiscard]] std::array<uint8_t, 16> to_bytes() const;
        uuid(const internal::bridge::uuid&); //NOLINT(google-explicit-constructor)
        internal::bridge::uuid m_uuid;
        friend struct internal::bridge::uuid;
        template <typename mapped_type>
        friend struct box_base;
    };

    struct object_id {
        explicit object_id(const std::string &);
        object_id() = default;
        [[nodiscard]] static object_id generate();
        [[nodiscard]] std::string to_string() const;
        object_id(const internal::bridge::object_id&); //NOLINT(google-explicit-constructor)
        internal::bridge::object_id m_object_id;
        friend object_id generate();
        friend struct internal::bridge::object_id;
        template <typename mapped_type>
        friend struct box_base;
    };
}

namespace realm::experimental {

    static int64_t serialize(const int64_t& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static int64_t deserialize(int64_t v) {
        return v;
    }

    static std::optional<int64_t> serialize(const std::optional<int64_t>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static std::optional<int64_t> deserialize(const std::optional<int64_t>& v) {
        return v;
    }

    static double serialize(const double& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static double deserialize(const double& v) {
        return v;
    }

    static std::optional<double> serialize(const std::optional<double>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static std::optional<double> deserialize(const std::optional<double>& v) {
        return v;
    }

    static bool serialize(bool v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static bool deserialize(bool v) {
        return v;
    }

    static std::optional<bool> serialize(std::optional<bool> v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static std::optional<bool> deserialize(std::optional<bool> v) {
        return v;
    }

    template<typename T>
    static typename std::enable_if_t<std::is_enum_v<T>, int64_t> serialize(const T& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return static_cast<int64_t>(v);
    }
    template<typename T, std::enable_if_t<std::is_enum_v<T>>>
    static T deserialize(int64_t v) {
        return static_cast<T>(v);
    }

    template<typename T, std::enable_if_t<std::is_enum_v<typename T::value_type>>>
    static std::optional<int64_t> serialize(const T& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        if (v) {
            return static_cast<int64_t>(*v);
        } else {
            return std::nullopt;
        }
    }
    template<typename T, std::enable_if_t<std::is_enum_v<typename T::value_type>>>
    static std::optional<int64_t> deserialize(std::optional<int64_t> v) {
        if (v) {
            return static_cast<T>(*v);
        } else {
            return std::nullopt;
        }
    }

    static std::string serialize(const std::string& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static std::string deserialize(const std::string& v) {
        return v;
    }

    static std::optional<std::string> serialize(const std::optional<std::string>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static std::optional<std::string> deserialize(const std::optional<std::string>& v) {
        return v;
    }

    static internal::bridge::uuid serialize(const uuid& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static uuid deserialize(const internal::bridge::uuid& v) {
        return v.operator ::realm::uuid();
    }

    static std::optional<internal::bridge::uuid> serialize(const std::optional<uuid>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static std::optional<uuid> deserialize(const std::optional<internal::bridge::uuid>& v) {
        if (v) {
            return v->operator ::realm::uuid();
        } else {
            return std::nullopt;
        }
    }

    static internal::bridge::binary serialize(const std::vector<uint8_t>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static std::vector<uint8_t> deserialize(const internal::bridge::binary& v) {
        return v.operator std::vector<uint8_t>();
    }

    static std::optional<internal::bridge::binary> serialize(const std::optional<std::vector<uint8_t>>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static std::optional<std::vector<uint8_t>> deserialize(const std::optional<internal::bridge::binary>& v) {
        if (v) {
            return v->operator std::vector<uint8_t>();
        } else {
            return std::nullopt;
        }
    }

    static internal::bridge::timestamp serialize(const std::chrono::time_point<std::chrono::system_clock>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static std::chrono::time_point<std::chrono::system_clock> deserialize(const internal::bridge::timestamp& v) {
        return v.operator std::chrono::time_point<std::chrono::system_clock>();
    }

    static std::optional<internal::bridge::timestamp> serialize(const std::optional<std::chrono::time_point<std::chrono::system_clock>>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static std::optional<std::chrono::time_point<std::chrono::system_clock>> deserialize(const std::optional<internal::bridge::timestamp>& v) {
        if (v) {
            return v->operator std::chrono::time_point<std::chrono::system_clock>();
        } else {
            return std::nullopt;
        }
    }

    static internal::bridge::object_id serialize(const realm::object_id& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static realm::object_id deserialize(const internal::bridge::object_id& v) {
        return v.operator ::realm::object_id();
    }

    static std::optional<internal::bridge::object_id> serialize(const std::optional<realm::object_id>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static std::optional<realm::object_id> deserialize(const std::optional<internal::bridge::object_id>& v) {
        if (v) {
            return v->operator ::realm::object_id();
        } else {
            return std::nullopt;
        }
    }

    template <typename T>
    static typename std::enable_if_t<internal::type_info::MixedPersistableConcept<T>::value, internal::bridge::mixed> serialize(const T& v, const std::optional<internal::bridge::realm>& realm = std::nullopt) {
        return std::visit([](auto&& arg) {
            using M = typename internal::type_info::type_info<std::decay_t<decltype(arg)>>::internal_type;
            return internal::bridge::mixed(M(arg));
        }, v);
    }
    template <typename T>
    static T deserialize(const internal::bridge::mixed& value) {
        if (value.is_null()) {
            return std::monostate();
        }
        switch (value.type()) {
            case internal::bridge::data_type::Int:
                return value.operator int64_t();
            case internal::bridge::data_type::Bool:
                return value.operator bool();
            case internal::bridge::data_type::String:
                return static_cast<std::string>(value);
            case internal::bridge::data_type::Binary:
                return static_cast<std::vector<uint8_t>>(static_cast<internal::bridge::binary>(value));
            case internal::bridge::data_type::Timestamp:
                return static_cast<internal::bridge::timestamp>(value);
            case internal::bridge::data_type::Float:
            case internal::bridge::data_type::Double:
                return static_cast<double>(value);
            case internal::bridge::data_type::UUID:
                return static_cast<internal::bridge::uuid>(value).operator ::realm::uuid();
            case internal::bridge::data_type::ObjectId:
                return static_cast<internal::bridge::object_id>(value).operator ::realm::object_id();
            case internal::bridge::data_type::TypedLink:
                abort();
//                REALM_TERMINATE("Objects stored in mixed properties must be accessed via `get_object_value()`");
            default:
                abort();
        }
    }

} // namespace realm::experimental


#endif//REALM_TYPES_HPP
