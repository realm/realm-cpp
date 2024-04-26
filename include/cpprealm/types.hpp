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

#ifndef CPPREALM_TYPES_HPP
#define CPPREALM_TYPES_HPP

#include <array>
#include <optional>
#include <set>
#include <stdexcept>

#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/bridge/schema.hpp>
#include <cpprealm/internal/type_info.hpp>

namespace realm {

    const size_t not_in_collection = -1;

    struct uuid {
        explicit uuid(const std::string &);
        uuid(const std::array<uint8_t, 16>&);
        uuid() = default;
        [[nodiscard]] std::string to_string() const;
        [[nodiscard]] std::string to_base64() const;
        [[nodiscard]] std::array<uint8_t, 16> to_bytes() const;
        uuid(const internal::bridge::uuid &);
        internal::bridge::uuid m_uuid;
        friend struct internal::bridge::uuid;
        template <typename mapped_type>
        friend struct box_base;
    };

    inline bool operator ==(const uuid& lhs, const uuid& rhs) { return lhs.m_uuid == rhs.m_uuid; }
    inline bool operator !=(const uuid& lhs, const uuid& rhs) { return lhs.m_uuid != rhs.m_uuid; }
    inline bool operator >(const uuid& lhs, const uuid& rhs) { return lhs.m_uuid > rhs.m_uuid; }
    inline bool operator >=(const uuid& lhs, const uuid& rhs) { return lhs.m_uuid >= rhs.m_uuid; }
    inline bool operator <(const uuid& lhs, const uuid& rhs) { return lhs.m_uuid < rhs.m_uuid; }
    inline bool operator <=(const uuid& lhs, const uuid& rhs) { return lhs.m_uuid <= rhs.m_uuid; }

    struct object_id {
        explicit object_id(const std::string &);
        object_id() = default;
        [[nodiscard]] static object_id generate();
        [[nodiscard]] std::string to_string() const;
        object_id(const internal::bridge::object_id &);
        internal::bridge::object_id m_object_id;
        friend struct internal::bridge::object_id;
        template <typename mapped_type>
        friend struct box_base;
    };

    inline bool operator ==(const object_id& lhs, const object_id& rhs) { return lhs.m_object_id == rhs.m_object_id; }
    inline bool operator !=(const object_id& lhs, const object_id& rhs) { return lhs.m_object_id != rhs.m_object_id; }
    inline bool operator <(const object_id& lhs, const object_id& rhs) { return lhs.m_object_id < rhs.m_object_id; }
    inline bool operator >(const object_id& lhs, const object_id& rhs) { return lhs.m_object_id > rhs.m_object_id; }
    inline bool operator <=(const object_id& lhs, const object_id& rhs) { return lhs.m_object_id <= rhs.m_object_id; }
    inline bool operator >=(const object_id& lhs, const object_id& rhs) { return lhs.m_object_id >= rhs.m_object_id; }

    struct decimal128 {
        explicit decimal128(const std::string &);
        decimal128(const double &);
        decimal128() = default;
        [[nodiscard]] std::string to_string() const;
        [[nodiscard]] bool is_NaN() const;
        decimal128(const internal::bridge::decimal128 &);
        internal::bridge::decimal128 m_decimal;
        friend struct internal::bridge::decimal128;
        template <typename mapped_type>
        friend struct box_base;

        decimal128 operator-(const decimal128& o);
        decimal128 operator+(const decimal128& o);
        decimal128 operator*(const decimal128& o);
        decimal128 operator/(const decimal128& o);

        decimal128& operator-=(const decimal128& o);
        decimal128& operator+=(const decimal128& o);
        decimal128& operator*=(const decimal128& o);
        decimal128& operator/=(const decimal128& o);
    };

    inline bool operator >(const decimal128& lhs, const decimal128& rhs) { return lhs.m_decimal > rhs.m_decimal; }
    inline bool operator >=(const decimal128& lhs, const decimal128& rhs) { return lhs.m_decimal >= rhs.m_decimal; }
    inline bool operator <(const decimal128& lhs, const decimal128& rhs) { return lhs.m_decimal < rhs.m_decimal; }
    inline bool operator <=(const decimal128& lhs, const decimal128& rhs) { return lhs.m_decimal <= rhs.m_decimal; }
    inline bool operator ==(const decimal128& lhs, const decimal128& rhs) { return lhs.m_decimal == rhs.m_decimal; }
    inline bool operator !=(const decimal128& lhs, const decimal128& rhs) { return lhs.m_decimal != rhs.m_decimal; }
}

namespace realm {

    static inline int64_t serialize(int64_t v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline int64_t deserialize(int64_t v) {
        return v;
    }

    static inline std::optional<int64_t> serialize(const std::optional<int64_t>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline std::optional<int64_t> deserialize(const std::optional<int64_t>& v) {
        return v;
    }

    static inline double serialize(const double& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline double deserialize(const double& v) {
        return v;
    }

    static inline std::optional<double> serialize(const std::optional<double>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline std::optional<double> deserialize(const std::optional<double>& v) {
        return v;
    }

    static inline bool serialize(bool v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline bool deserialize(bool v) {
        return v;
    }

    static inline std::optional<bool> serialize(std::optional<bool> v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline std::optional<bool> deserialize(std::optional<bool> v) {
        return v;
    }

    template<typename T>
    static inline typename std::enable_if_t<std::is_enum_v<T>, int64_t> serialize(const T& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return static_cast<int64_t>(v);
    }
    template<typename T, std::enable_if_t<std::is_enum_v<T>>>
    static inline T deserialize(int64_t v) {
        return static_cast<T>(v);
    }

    template<typename T>
    static inline std::enable_if_t<std::is_enum_v<typename T::value_type>, std::optional<int64_t>> serialize(const T& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        if (v) {
            return static_cast<int64_t>(*v);
        } else {
            return std::nullopt;
        }
    }
    template<typename T, std::enable_if_t<std::is_enum_v<typename T::value_type>>>
    static inline std::optional<int64_t> deserialize(std::optional<int64_t> v) {
        if (v) {
            return static_cast<T>(*v);
        } else {
            return std::nullopt;
        }
    }

    static inline std::string serialize(const std::string& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline std::string deserialize(const std::string& v) {
        return v;
    }

    static inline std::optional<std::string> serialize(const std::optional<std::string>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline std::optional<std::string> deserialize(const std::optional<std::string>& v) {
        return v;
    }

    static inline internal::bridge::uuid serialize(const uuid& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline uuid deserialize(const internal::bridge::uuid& v) {
        return v.operator ::realm::uuid();
    }

    static inline std::optional<internal::bridge::uuid> serialize(const std::optional<uuid>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline std::optional<uuid> deserialize(const std::optional<internal::bridge::uuid>& v) {
        if (v) {
            return v->operator ::realm::uuid();
        } else {
            return std::nullopt;
        }
    }

    static inline internal::bridge::binary serialize(const std::vector<uint8_t>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline std::vector<uint8_t> deserialize(const internal::bridge::binary& v) {
        return v.operator std::vector<uint8_t>();
    }

    static inline std::optional<internal::bridge::binary> serialize(const std::optional<std::vector<uint8_t>>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline std::optional<std::vector<uint8_t>> deserialize(const std::optional<internal::bridge::binary>& v) {
        if (v) {
            return v->operator std::vector<uint8_t>();
        } else {
            return std::nullopt;
        }
    }

    static inline internal::bridge::timestamp serialize(const std::chrono::time_point<std::chrono::system_clock>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline std::chrono::time_point<std::chrono::system_clock> deserialize(const internal::bridge::timestamp& v) {
        return v.operator std::chrono::time_point<std::chrono::system_clock>();
    }

    static inline std::optional<internal::bridge::timestamp> serialize(const std::optional<std::chrono::time_point<std::chrono::system_clock>>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline std::optional<std::chrono::time_point<std::chrono::system_clock>> deserialize(const std::optional<internal::bridge::timestamp>& v) {
        if (v) {
            return v->operator std::chrono::time_point<std::chrono::system_clock>();
        } else {
            return std::nullopt;
        }
    }

    static inline internal::bridge::object_id serialize(const realm::object_id& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline realm::object_id deserialize(const internal::bridge::object_id& v) {
        return v.operator ::realm::object_id();
    }

    static inline std::optional<internal::bridge::object_id> serialize(const std::optional<realm::object_id>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline std::optional<realm::object_id> deserialize(const std::optional<internal::bridge::object_id>& v) {
        if (v) {
            return v->operator ::realm::object_id();
        } else {
            return std::nullopt;
        }
    }

    static inline internal::bridge::decimal128 serialize(const realm::decimal128& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline realm::decimal128 deserialize(const internal::bridge::decimal128& v) {
        return v.operator ::realm::decimal128();
    }

    static inline std::optional<internal::bridge::decimal128> serialize(const std::optional<realm::decimal128>& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return v;
    }
    static inline std::optional<realm::decimal128> deserialize(const std::optional<internal::bridge::decimal128>& v) {
        if (v) {
            return v->operator ::realm::decimal128();
        } else {
            return std::nullopt;
        }
    }

    template <typename T>
    static inline typename std::enable_if_t<internal::type_info::MixedPersistableConcept<T>::value, internal::bridge::mixed> serialize(const T& v, const std::optional<internal::bridge::realm>& = std::nullopt) {
        return std::visit([&](auto&& arg) {
            using StoredType = std::decay_t<decltype(arg)>;
            using M = typename internal::type_info::type_info<StoredType>::internal_type;
            return internal::bridge::mixed(M(arg));
        }, v);
    }

    template <typename T>
    static inline typename std::enable_if_t<!internal::type_info::MixedPersistableConcept<T>::value, T>
    deserialize(const internal::bridge::mixed& value) {
        if constexpr (internal::type_info::is_optional<T>::value) {
            return std::nullopt;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return value.operator int64_t();
        } else if constexpr (std::is_same_v<T, bool>) {
            return value.operator bool();
        } else if constexpr (std::is_same_v<T, std::string>) {
            return static_cast<std::string>(value);
        } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
            return static_cast<std::vector<uint8_t>>(static_cast<internal::bridge::binary>(value));
        } else if constexpr (std::is_same_v<T, std::chrono::time_point<std::chrono::system_clock>>) {
            return static_cast<internal::bridge::timestamp>(value);
        } else if constexpr (std::disjunction_v<std::is_same<T, double>, std::is_same<T, float>>) {
            return static_cast<double>(value);
        } else if constexpr (std::is_same_v<T, realm::uuid>) {
            return value.operator internal::bridge::uuid().operator ::realm::uuid();
        } else if constexpr (std::is_same_v<T, realm::object_id>) {
            return value.operator internal::bridge::object_id().operator ::realm::object_id();
        } else if constexpr (std::is_same_v<T, realm::decimal128>) {
            return value.operator internal::bridge::decimal128() .operator ::realm::decimal128();
        } else if constexpr (std::is_enum_v<T>) {
            return static_cast<T>(value.operator int64_t());
        } else {
            abort();
        }
    }

    template <typename T>
    static inline typename std::enable_if_t<internal::type_info::MixedPersistableConcept<T>::value, T>
            deserialize(const internal::bridge::mixed& value) {
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
                return value.operator internal::bridge::uuid().operator ::realm::uuid();
            case internal::bridge::data_type::ObjectId:
                return value.operator internal::bridge::object_id().operator ::realm::object_id();
            case internal::bridge::data_type::Decimal:
                return value.operator internal::bridge::decimal128().operator ::realm::decimal128();
            case internal::bridge::data_type::TypedLink:
                throw std::runtime_error("Objects stored in mixed properties must be accessed via `get_object_value()`");
            default:
                throw std::runtime_error("Unknown datatype while deserializing mixed property.");
        }
    }

} // namespace realm


#endif//CPPREALM_TYPES_HPP
