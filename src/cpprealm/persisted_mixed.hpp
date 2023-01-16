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

#ifndef REALM_PERSISTED_MIXED_HPP
#define REALM_PERSISTED_MIXED_HPP

#include <cpprealm/persisted.hpp>
#include <cpprealm/persisted_uuid.hpp>
#include <cpprealm/persisted_object_id.hpp>
#include <cpprealm/persisted_custom.hpp>

namespace realm {
    using mixed = std::variant<
            int64_t,
            bool,
            std::string,
            double,
            std::vector<uint8_t>,
            std::chrono::time_point<std::chrono::system_clock>,
            uuid,
            object_id>;

    template<typename T>
    struct persisted<T, std::enable_if_t<realm::internal::type_info::MixedPersistableConcept<T>::value>> final :
            public persisted_primitive_base<T> {
        using persisted_primitive_base<T>::operator=;
    protected:
        static internal::bridge::mixed serialize(const T& value) {
            return std::visit([](auto& arg) {
                using M = std::decay_t<decltype(arg)>;
                return internal::bridge::mixed(static_cast<M>(arg));
            }, value);
        }
        static T deserialize(const internal::bridge::mixed& value) {
            switch (value.type()) {
                case internal::bridge::data_type::Int: return value.operator int64_t();
                case internal::bridge::data_type::Bool: return value.operator bool();
                case internal::bridge::data_type::String: return static_cast<std::string>(value);
                case internal::bridge::data_type::Binary:
                    return static_cast<std::vector<uint8_t>>(static_cast<internal::bridge::binary>(value));
                case internal::bridge::data_type::Timestamp: return static_cast<internal::bridge::timestamp>(value);
                case internal::bridge::data_type::Float:
                case internal::bridge::data_type::Double: return static_cast<double>(value);
                case internal::bridge::data_type::UUID:
                    return static_cast<uuid>(static_cast<internal::bridge::uuid>(value));
                case internal::bridge::data_type::ObjectId:
                    return static_cast<object_id>(static_cast<internal::bridge::object_id>(value));
                default: abort();
            }
        }
        __cpp_realm_friends
    };

#define __cpp_realm_generate_mixed_operator(op) \
    template <template <typename ...> typename Variant, typename ...Ts, typename V> \
    std::enable_if_t<internal::type_info::is_variant_t<Variant<Ts...>>::value, rbool> \
    operator op(const persisted<Variant<Ts...>>& a, V&& b) \
    { \
        if (a.should_detect_usage_for_queries) { \
            auto query = internal::bridge::query(a.query->get_table()); \
            query.equal(a.managed, internal::bridge::mixed(persisted<std::decay_t<V>>::serialize(b))); \
            return query; \
        } \
        return std::visit([&b](auto&& arg) { \
            using M = std::decay_t<decltype(arg)>; \
            if constexpr (std::is_convertible_v<M, V>) { \
                return arg op b; \
            } else { \
                return false; \
            } \
        }, *a); \
    }

    __cpp_realm_generate_mixed_operator(==)

    template <typename T>
    inline std::ostream& operator<< (std::ostream& stream, const persisted<T, std::enable_if_t<realm::internal::type_info::MixedPersistableConcept<T>::value>>& value)
    {
        std::visit([&stream](auto&& arg) {
            using M = std::decay_t<decltype(arg)>;
            if constexpr (internal::type_info::is_vector<M>::value) {
                stream << "{ ";
                for (size_t i = 0; i < arg.size(); i++) {
                    stream << arg[i];
                    if (i < arg.size() - 1) stream << ",";
                }
                stream << " }";
            } else {
                stream << static_cast<M>(arg);
            }
        }, *value);
        return stream;
    }

    namespace {
        static_assert(sizeof(persisted<std::variant<int64_t, std::string>>{}));
    }
}

#endif //REALM_PERSISTED_MIXED_HPP
