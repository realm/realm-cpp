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
#include <cpprealm/internal/bridge/schema.hpp>

namespace realm {
    using mixed_data_type = internal::bridge::data_type;
    template<typename T>
    struct persisted<T, std::enable_if_t<realm::internal::type_info::MixedPersistableConcept<T>::value>> final :
            public persisted_primitive_base<T> {
        using persisted_primitive_base<T>::operator=;

        persisted() {
            new(&this->unmanaged) T();
        }

        persisted(const T& value) {
            new(&this->unmanaged) T(value);
        }
        persisted(T&& value) {
            new(&this->unmanaged) T(std::move(value));
        }

        mixed_data_type get_data_type() const {
            if (!this->m_object) {
                REALM_TERMINATE("Object must be managed to check `data_type`.");
            }
            internal::bridge::mixed val = this->m_object->get_obj().template get<typename internal::type_info::type_info<T>::internal_type>(
                    this->managed);
            return val.type();
        }

        template<typename Object, typename = std::enable_if_t<std::is_base_of_v<object_base<Object>, Object>>>
        Object get_object_value() {
            internal::bridge::mixed val = this->m_object->get_obj().template get<typename internal::type_info::type_info<T>::internal_type>(
                    this->managed);
            Object o;
            o.assign_accessors(
                    internal::bridge::object(this->m_object->get_realm(), val.operator internal::bridge::obj_link()));
            return o;
        }
    protected:
        static internal::bridge::mixed serialize(const T& value, const std::optional<internal::bridge::realm>& realm = std::nullopt) {
            return std::visit([&realm](auto& arg) {
                using M = std::decay_t<decltype(arg)>;
                if constexpr (std::is_base_of<object_base<M>, M>::value) {
                    internal::bridge::realm r = *realm;
                    auto actual_schema = r.schema().find(M::schema.name);
                    auto group = r.read_group();
                    auto table = group.get_table(actual_schema.table_key());
                    const_cast<M&>(arg).manage(table, r);
                    return internal::bridge::mixed(const_cast<M&>(arg).m_object->get_obj().get_link());
                } else {
                    return internal::bridge::mixed(static_cast<M>(arg));
                }
            }, value);
        }
        static T deserialize(const internal::bridge::mixed& value) {
            if (value.is_null()) {
                return std::monostate();
            }
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
                case internal::bridge::data_type::TypedLink:
                    REALM_TERMINATE("Objects stored in mixed properties must be accessed via `get_object_value()`");
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
    __cpp_realm_generate_mixed_operator(!=)

    template <template <typename ...> typename Variant, typename ...Ts>
    std::enable_if_t<internal::type_info::is_variant_t<Variant<Ts...>>::value, rbool>
    operator ==(const persisted<Variant<Ts...>>& a, const std::nullopt_t& b)
    {
        if (a.should_detect_usage_for_queries) {
            auto query = internal::bridge::query(a.query->get_table());
            query.equal(a.managed, b);
            return query;
        }
        REALM_TERMINATE("Mixed property must be managed for queries.");
    }

    template <template <typename ...> typename Variant, typename ...Ts>
    std::enable_if_t<internal::type_info::is_variant_t<Variant<Ts...>>::value, rbool>
    operator !=(const persisted<Variant<Ts...>>& a, const std::nullopt_t& b)
    {
        if (a.should_detect_usage_for_queries) {
            auto query = internal::bridge::query(a.query->get_table());
            query.not_equal(a.managed, b);
            return query;
        }
        REALM_TERMINATE("Mixed property must be managed for queries.");
    }
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
            } else if constexpr (std::is_same_v<M, std::monostate>) {
                stream << "null";
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
