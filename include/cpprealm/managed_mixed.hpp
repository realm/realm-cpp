////////////////////////////////////////////////////////////////////////////
//
// Copyright 2024 Realm Inc.
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

#ifndef CPPREALM_MANAGED_MIXED_HPP
#define CPPREALM_MANAGED_MIXED_HPP

#include <cpprealm/macros.hpp>
#include <cpprealm/rbool.hpp>
#include <cpprealm/types.hpp>

namespace realm {

//    using mixed = std::variant<
//            std::monostate,
//            int64_t,
//            bool,
//            std::string,
//            double,
//            std::vector<uint8_t>,
//            std::chrono::time_point<std::chrono::system_clock>,
//            uuid,
//            object_id,
//            decimal128>;

    struct mixed {
        mixed() = default;
        mixed(const mixed&) = default;
        mixed(mixed&&) = default;
        mixed& operator=(const mixed&) = default;
        mixed& operator=(mixed&&) = default;

        mixed(std::monostate) { }
        mixed(int64_t) { }
        mixed(bool) { }
        mixed(std::string) { }
        mixed(double) { }
        mixed(std::vector<uint8_t>) { }
        mixed(std::chrono::time_point<std::chrono::system_clock>) { }
        mixed(uuid) { }
        mixed(object_id) { }
        mixed(decimal128) { }
        // Links
        // unmanaged
        template<typename T>
        mixed(const T&) {
            static_assert(sizeof(managed<T>), "Must declare schema for T");
        }
        // managed with pointer
        template<typename T, typename ManagedObject = managed<T>>
        mixed(const ManagedObject*&) {
            static_assert(sizeof(managed<T>), "Must declare schema for T");
        }
        template<typename T, typename ManagedObject = managed<T>>
        mixed(std::unique_ptr<ManagedObject>&&) {
            static_assert(sizeof(managed<T>), "Must declare schema for T");
        }
        template<typename T, typename ManagedObject = managed<T>>
        mixed(const std::shared_ptr<ManagedObject>&) {
            static_assert(sizeof(managed<T>), "Must declare schema for T");
        }
        // managed with const ref
        template<typename T, typename ManagedObject = managed<T>>
        mixed(const ManagedObject&) {
            static_assert(sizeof(managed<T>), "Must declare schema for T");
        }

        mixed(std::vector<mixed>) { }
        mixed(std::map<std::string, mixed>) { }
    };


    template<>
    struct managed<mixed> : public managed_base {
        using managed<mixed>::managed_base::operator=;

        enum stored_type {
            Int = 0,
            Bool = 1,
            String = 2,
            Binary = 4,
            Mixed = 6,
            Timestamp = 8,
            Float = 9,
            Double = 10,
            Decimal = 11,
            Link = 12,
            LinkList = 13,
            ObjectId = 15,
            TypedLink = 16,
            UUID = 17,
            Null = 18,
        };

        managed& operator =(const mixed&) {
//            m_obj->set(m_key, std::visit([](auto&& arg) {
//                           using M = typename internal::type_info::type_info<std::decay_t<decltype(arg)>>::internal_type;
//                           return internal::bridge::mixed(M(arg));
//                       }, v));
            return *this;
        }

        [[nodiscard]] stored_type get_stored_type() const {
            auto val = m_obj->get<realm::internal::bridge::mixed>(m_key);
            if (val.is_null()) {
                return stored_type::Null;
            } else {
                return static_cast<stored_type>(val.type());
            }
        }

        [[nodiscard]] mixed detach() const {
            return mixed();
//            return deserialize<T>(m_obj->get<realm::internal::bridge::mixed>(m_key));
        }

        [[nodiscard]] mixed operator *() const {
            return detach();
        }

        //MARK: -   comparison operators
        rbool operator==(const mixed&) const noexcept {
//            if (this->m_rbool_query) {
//                return this->m_rbool_query->mixed_equal(m_key, serialize(rhs));
//            }
//            return detach() == rhs;
            return true;
        }

        rbool operator!=(const mixed&) const noexcept {
//            if (this->m_rbool_query) {
//                return this->m_rbool_query->mixed_not_equal(m_key, serialize(rhs));
//            }
//            return detach() != rhs;
            return true;
        }

        rbool operator==(const std::nullopt_t&) const noexcept {
//            if (this->m_rbool_query) {
//                return this->m_rbool_query->mixed_equal(m_key, internal::bridge::mixed(std::monostate()));
//            }
//            return detach() == T(std::monostate());
            return true;
        }

        rbool operator!=(const std::nullopt_t&) const noexcept {
//            if (this->m_rbool_query) {
//                return this->m_rbool_query->mixed_not_equal(m_key, internal::bridge::mixed(std::monostate()));
//            }
//            return detach() != T(std::monostate());
            return true;
        }

        bool has_link() const {
            return (get_stored_type() == stored_type::TypedLink);
        }

        template<typename U>
        typename managed<U>::ref_type get_stored_link() const {
            std::terminate();
//            m_realm->read_group();
//            realm::internal::bridge::mixed m = m_obj->get<realm::internal::bridge::mixed>(m_key);
//
//            auto obj = internal::bridge::object(*m_realm, m.operator internal::bridge::obj_link());
//            uint32_t alternative_key = m_realm->table_for_object_type(managed<std::remove_pointer_t<U>>::schema.name).get_key();
//            uint32_t stored_table = obj.get_obj().get_table().get_key();
//
//            if (alternative_key != stored_table) {
//                throw std::runtime_error("Different link type stored in mixed type. Stored type: " + obj.get_object_schema().get_name());
//            }
//            return typename managed<U>::ref_type(managed<std::remove_pointer_t<U>>(obj.get_obj(), *m_realm));
        }

        template <typename U>
        void set_link(U &&) {
//            static_assert(sizeof(managed<U>), "Must declare schema for T");
//            static_assert(managed<U>::object_type == ObjectType::TopLevel, "Mixed properties can only store Top Level objects.");
//            auto table = m_realm->table_for_object_type(managed<U>::schema.name);
//            internal::bridge::obj o;
//            if constexpr (managed<U>::schema.HasPrimaryKeyProperty) {
//                auto pk = v.*(managed<U>::schema.primary_key().ptr);
//                o = table.create_object_with_primary_key(realm::internal::bridge::mixed(serialize(pk.value)));
//            } else {
//                o = table.create_object();
//            }
//
//            std::apply([&o, &v, this](auto && ...p) {
//                (accessor<typename std::decay_t<decltype(p)>::Result>::set(
//                         o, o.get_table().get_column_key(p.name), *this->m_realm, v.*(std::decay_t<decltype(p)>::ptr)
//                                 ), ...);
//            }, managed<U>::schema.ps);
//            m_obj->set(m_key, internal::bridge::mixed(o.get_link()));
        }

        template<typename U>
        std::enable_if_t<managed<U>::is_object && managed<U>::object_type == ObjectType::TopLevel, void>
        set_link(managed<U>&) {
//            m_obj->set(m_key, internal::bridge::mixed(internal::bridge::obj_link(link.m_obj.get_table().get_key(), link.m_obj.get_key())));
        }

    private:
        managed() = default;
        managed(const managed&) = delete;
        managed(managed &&) = delete;
        managed& operator=(const managed&) = delete;
        managed& operator=(managed&&) = delete;
        template<typename, typename>
        friend struct managed;
    };

    template<typename T, typename Types>
    bool holds_alternative(const realm::managed<Types>& v) noexcept {
        auto val = v.get_stored_type();
        switch (val) {
            case realm::managed<Types>::stored_type::Int:
                if constexpr (std::is_same_v<T, int64_t>)
                    return true;
                break;
            case realm::managed<Types>::stored_type::Bool:
                if constexpr (std::is_same_v<T, bool>)
                    return true;
                break;
            case realm::managed<Types>::stored_type::String:
                if constexpr (std::is_same_v<T, std::string>)
                    return true;
                break;
            case realm::managed<Types>::stored_type::Binary:
                if constexpr (std::is_same_v<T, std::vector<uint8_t>>)
                    return true;
                break;
            case realm::managed<Types>::stored_type::Mixed:
                if constexpr (std::is_same_v<T, Types>)
                    return true;
                break;
            case realm::managed<Types>::stored_type::Timestamp:
                if constexpr (std::is_same_v<T, std::chrono::time_point<std::chrono::system_clock>>)
                    return true;
                break;
            case realm::managed<Types>::stored_type::Float:
                if constexpr (std::is_same_v<T, float>)
                    return true;
                break;
            case realm::managed<Types>::stored_type::Double:
                if constexpr (std::is_same_v<T, double>)
                    return true;
                break;
            case realm::managed<Types>::stored_type::Decimal:
                if constexpr (std::is_same_v<T, realm::decimal128>)
                    return true;
                break;
            case realm::managed<Types>::stored_type::ObjectId:
                if constexpr (std::is_same_v<T, realm::object_id>)
                    return true;
                break;

            case realm::managed<Types>::stored_type::UUID:
                if constexpr (std::is_same_v<T, realm::uuid>)
                    return true;
                break;
            case realm::managed<Types>::stored_type::Null:
                if constexpr (std::is_same_v<T, std::monostate>)
                    return true;
                break;
            case realm::managed<Types>::stored_type::TypedLink: {
                if constexpr (std::is_pointer_v<T>) {
                    auto m = v.m_obj->template get<internal::bridge::mixed>(v.m_key);
                    uint32_t alternative_key = v.m_realm->table_for_object_type(managed<std::remove_pointer_t<T>>::schema.name).get_key();
                    uint32_t stored_key = internal::bridge::object(*v.m_realm, m.operator internal::bridge::obj_link()).get_object_schema().table_key();
                    return alternative_key == stored_key;
                }
                break;
            }
            case realm::managed<Types>::stored_type::Link:
            case realm::managed<Types>::stored_type::LinkList:
                break;
            default:
                break;
        }

        return false;
    }
}

#endif//CPPREALM_MANAGED_MIXED_HPP
