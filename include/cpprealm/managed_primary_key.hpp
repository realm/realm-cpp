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

#ifndef CPPREALM_MANAGED_PRIMARY_KEY_HPP
#define CPPREALM_MANAGED_PRIMARY_KEY_HPP

#include <cpprealm/macros.hpp>
#include <cpprealm/rbool.hpp>

namespace realm {
        template <typename, typename>
        struct managed;

        template<typename T>
        struct primary_key {
            primary_key() = default;
            primary_key(const T& v) {
                this->value = v;
            }
            T value;
            using internal_type = T;
            operator T() const {
                return value;
            }
        };

        template<>
        struct primary_key<int64_t> {
            using internal_type = int64_t;
            primary_key() = default;
            primary_key(const int64_t& v) {
                this->value = v;
            }
            primary_key(const int& v) {
                this->value = v;
            }
            int64_t value;
            operator int64_t() const {
                return value;
            }
        };

        template<>
        struct primary_key<object_id> {
            using internal_type = internal::bridge::object_id;
            primary_key() = default;
            primary_key(const object_id& v) {
                this->value = v;
            }
            object_id value;
            operator object_id() const {
                return value;
            }
        };

        template<>
        struct primary_key<std::string> {
            using internal_type = std::string;
            primary_key() = default;
            primary_key(const std::string& v) {
                this->value = v;
            }
            primary_key(const char* v) {
                this->value = v;
            }
            std::string value;
            operator std::string() const {
                return value;
            }
        };

        template<>
        struct primary_key<uuid> {
            using internal_type = internal::bridge::uuid;
            primary_key() = default;
            primary_key(const uuid& v) {
                this->value = v;
            }
            uuid value;
            operator uuid() const {
                return value;
            }
        };

        template<>
        struct primary_key<std::optional<int64_t>> {
            using internal_type = std::optional<int64_t>;
            primary_key() = default;
            primary_key(const std::nullopt_t& v) {
                this->value = v;
            }
            primary_key(const std::optional<int64_t>& v) {
                this->value = v;
            }
            primary_key(const std::optional<int>& v) {
                this->value = v;
            }
            primary_key(const int64_t& v) {
                this->value = v;
            }
            primary_key(const int& v) {
                this->value = v;
            }
            std::optional<int64_t> value;
            operator std::optional<int64_t>() const {
                return value;
            }
        };

        template<>
        struct primary_key<std::optional<object_id>> {
            using internal_type = std::optional<internal::bridge::object_id>;
            primary_key() = default;
            primary_key(const std::nullopt_t& v) {
                this->value = v;
            }
            primary_key(const std::optional<object_id>& v) {
                this->value = v;
            }
            primary_key(const object_id& v) {
                this->value = v;
            }
            std::optional<object_id> value;
            operator std::optional<object_id>() const {
                return value;
            }
        };

        template<>
        struct primary_key<std::optional<std::string>> {
            using internal_type = std::optional<std::string>;
            primary_key() = default;
            primary_key(const std::nullopt_t& v) {
                this->value = v;
            }
            primary_key(const std::optional<std::string>& v) {
                this->value = v;
            }
            primary_key(const std::string& v) {
                this->value = v;
            }
            primary_key(const char* v) {
                this->value = v;
            }
            std::optional<std::string> value;
            operator std::optional<std::string>() const {
                return value;
            }
        };

        template<>
        struct primary_key<std::optional<uuid>> {
            using internal_type = std::optional<internal::bridge::uuid>;
            primary_key() = default;
            primary_key(const std::nullopt_t& v) {
                this->value = v;
            }
            primary_key(const std::optional<uuid>& v) {
                this->value = v;
            }
            primary_key(const uuid& v) {
                this->value = v;
            }
            std::optional<uuid> value;
            operator std::optional<uuid>() const {
                return value;
            }
        };

        template<>
        struct managed<primary_key<int64_t>> final : managed_base {
            primary_key<int64_t> detach() const {
                return operator int64_t();
            }

            operator int64_t() const {
                return m_obj->template get<int64_t>(m_key);
            }

            rbool operator==(const int64_t& rhs) const noexcept;
            rbool operator!=(const int64_t& rhs) const noexcept;
            rbool operator>(const int64_t& rhs) const noexcept;
            rbool operator>=(const int64_t& rhs) const noexcept;
            rbool operator<(const int64_t& rhs) const noexcept;
            rbool operator<=(const int64_t& rhs) const noexcept;
            rbool operator==(const int& rhs) const noexcept;
            rbool operator!=(const int& rhs) const noexcept;
            rbool operator>(const int& rhs) const noexcept;
            rbool operator>=(const int& rhs) const noexcept;
            rbool operator<(const int& rhs) const noexcept;
            rbool operator<=(const int& rhs) const noexcept;

        private:
            managed() = default;
            managed(const managed&) = delete;
            managed(managed &&) = delete;
            managed& operator=(const managed&) = delete;
            managed& operator=(managed&&) = delete;
            template<typename, typename>
            friend struct managed;
        };

        template<>
        struct managed<primary_key<std::string>> final : managed_base {
            primary_key<std::string> detach() const {
                return operator std::string();
            }

            operator std::string() const {
                return m_obj->template get<std::string>(m_key);
            }

            rbool operator==(const std::string& rhs) const noexcept;
            rbool operator!=(const std::string& rhs) const noexcept;
            rbool operator==(const char* rhs) const noexcept;
            rbool operator!=(const char* rhs) const noexcept;

        private:
            managed() = default;
            managed(const managed&) = default;
            managed(managed &&) = delete;
            managed& operator=(const managed&) = delete;
            managed& operator=(managed&&) = delete;
            template<typename, typename>
            friend struct managed;
        };

        template<>
        struct managed<primary_key<realm::uuid>> : managed_base {
            primary_key<realm::uuid> detach() const {
                return operator realm::uuid();
            }

            operator realm::uuid() const {
                return m_obj->template get<internal::bridge::uuid>(m_key).operator ::realm::uuid();
            }

            rbool operator==(const realm::uuid& rhs) const noexcept;
            rbool operator!=(const realm::uuid& rhs) const noexcept;

        private:
            managed() = default;
            managed(const managed&) = delete;
            managed(managed &&) = delete;
            managed& operator=(const managed&) = delete;
            managed& operator=(managed&&) = delete;
            template<typename, typename>
            friend struct managed;
        };

        template<>
        struct managed<primary_key<realm::object_id>> final : managed_base {
            primary_key<realm::object_id> detach() const {
                return operator realm::object_id();
            }

            operator realm::object_id() const {
                return m_obj->template get<internal::bridge::object_id>(m_key).operator ::realm::object_id();
            }

            rbool operator==(const realm::object_id& rhs) const noexcept;
            rbool operator!=(const realm::object_id& rhs) const noexcept;

        private:
            managed() = default;
            managed(const managed&) = delete;
            managed(managed &&) = delete;
            managed& operator=(const managed&) = delete;
            managed& operator=(managed&&) = delete;
            template<typename, typename>
            friend struct managed;
        };

        template<typename T>
        struct managed<primary_key<T>, std::enable_if_t<std::is_enum_v<T>>> final : managed_base {
            primary_key<T> detach() const {
                return operator T();
            }

            operator T() const {
                return static_cast<T>(m_obj->template get<int64_t>(m_key));
            }

            rbool operator==(const T& rhs) const noexcept {
                if (this->m_rbool_query) {
                    return this->m_rbool_query->equal(m_key, serialize(rhs));
                }
                return serialize(detach().value) == serialize(rhs);
            }
            rbool operator!=(const T& rhs) const noexcept {
                if (this->m_rbool_query) {
                    return this->m_rbool_query->not_equal(m_key, serialize(rhs));
                }
                return serialize(detach().value) != serialize(rhs);
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

        template<>
        struct managed<primary_key<std::optional<int64_t>>> final : managed_base {
            primary_key<std::optional<int64_t>> detach() const {
                return operator std::optional<int64_t>();
            }

            operator std::optional<int64_t>() const {
                return m_obj->get_optional<int64_t>(m_key);
            }

            rbool operator==(const std::optional<int64_t>& rhs) const noexcept;
            rbool operator!=(const std::optional<int64_t>& rhs) const noexcept;
            rbool operator>(const int64_t& rhs) const noexcept;
            rbool operator>=(const int64_t& rhs) const noexcept;
            rbool operator<(const int64_t& rhs) const noexcept;
            rbool operator<=(const int64_t& rhs) const noexcept;
            rbool operator>(const int& rhs) const noexcept;
            rbool operator>=(const int& rhs) const noexcept;
            rbool operator<(const int& rhs) const noexcept;
            rbool operator<=(const int& rhs) const noexcept;

        private:
            managed() = default;
            managed(managed &&) = delete;
            managed& operator=(const managed&) = delete;
            managed& operator=(managed&&) = delete;
            template<typename, typename>
            friend struct managed;
        };

        template<typename T>
        struct managed<primary_key<T>, std::enable_if_t<std::conjunction_v<typename internal::type_info::is_optional<T>,
                                                                           std::is_enum<typename T::value_type> >>> final : managed_base {
            primary_key<T> detach() const {
                return operator T();
            }

            operator T() const {
                auto v = m_obj->get_optional<int64_t>(m_key);
                if (v) {
                    return static_cast<typename T::value_type>(*v);
                } else {
                    return std::nullopt;
                }
            }

            rbool operator==(const T& rhs) const noexcept {
                if (this->m_rbool_query) {
                    return this->m_rbool_query->equal(m_key, serialize(rhs));
                }
                return serialize(detach().value) == serialize(rhs);
            }
            rbool operator!=(const T& rhs) const noexcept {
                if (this->m_rbool_query) {
                    return this->m_rbool_query->not_equal(m_key, serialize(rhs));
                }
                return serialize(detach().value) != serialize(rhs);
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

        template<>
        struct managed<primary_key<std::optional<std::string>>> final : managed_base {
            primary_key<std::optional<std::string>> detach() const {
                return operator std::optional<std::string>();
            }

            operator std::optional<std::string>() const {
                return m_obj->get_optional<std::string>(m_key);
            }

            rbool operator==(const std::optional<std::string>& rhs) const noexcept;
            rbool operator!=(const std::optional<std::string>& rhs) const noexcept;
            rbool operator==(const char* rhs) const noexcept;
            rbool operator!=(const char* rhs) const noexcept;

        private:
            managed() = default;
            managed(const managed&) = delete;
            managed(managed &&) = delete;
            managed& operator=(const managed&) = delete;
            managed& operator=(managed&&) = delete;
            template<typename, typename>
            friend struct managed;
        };

        template<>
        struct managed<primary_key<std::optional<realm::uuid>>> final : managed_base {
            primary_key<std::optional<realm::uuid>> detach() const {
                return operator std::optional<realm::uuid>();
            }

            operator std::optional<realm::uuid>() const {
                auto v = m_obj->get_optional<internal::bridge::uuid>(m_key);
                if (v) {
                    return v->operator ::realm::uuid();
                }
                return std::nullopt;
            }

            rbool operator==(const std::optional<realm::uuid>& rhs) const noexcept;
            rbool operator!=(const std::optional<realm::uuid>& rhs) const noexcept;

        private:
            managed() = default;
            managed(const managed&) = delete;
            managed(managed &&) = delete;
            managed& operator=(const managed&) = delete;
            managed& operator=(managed&&) = delete;
            template<typename, typename>
            friend struct managed;
        };

        template<>
        struct managed<primary_key<std::optional<realm::object_id>>> final : managed_base {
            std::optional<realm::object_id> detach() const {
                return operator std::optional<realm::object_id>();
            }

            operator std::optional<realm::object_id>() const {
                auto v = m_obj->get_optional<internal::bridge::object_id>(m_key);
                if (v) {
                    return v->operator ::realm::object_id();
                }
                return std::nullopt;
            }

            rbool operator==(const std::optional<realm::object_id>& rhs) const noexcept;
            rbool operator!=(const std::optional<realm::object_id>& rhs) const noexcept;

        private:
            managed() = default;
            managed(const managed&) = delete;
            managed(managed &&) = delete;
            managed& operator=(const managed&) = delete;
            managed& operator=(managed&&) = delete;
            template<typename, typename>
            friend struct managed;
        };
}

#endif //CPPREALM_MANAGED_PRIMARY_KEY_HPP
