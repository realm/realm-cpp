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

#ifndef CPPREALM_MANAGED_NUMERIC_HPP
#define CPPREALM_MANAGED_NUMERIC_HPP

#include <cpprealm/macros.hpp>
#include <cpprealm/rbool.hpp>

namespace realm {
    template<>
    struct managed<int64_t> : managed_base {
        using managed<int64_t>::managed_base::operator=;

        managed_base& operator =(const int64_t& v) {
            this->m_obj->template set<int64_t>(m_key, v);
            return *this;
        }

        [[nodiscard]] int64_t detach() const {
            return m_obj->template get<int64_t>(m_key);
        }

        [[nodiscard]] int64_t operator *() const {
            return detach();
        }

        [[nodiscard]] operator int64_t() const {
            return detach();
        }

        template<typename T>
        std::enable_if_t<std::is_integral_v<T>, rbool> operator==(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->equal(m_key, (int64_t)rhs);
            }
            return serialize(detach()) == rhs;
        }

        template<typename T>
        std::enable_if_t<std::is_integral_v<T>, rbool> operator!=(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->not_equal(m_key, (int64_t)rhs);
            }
            return serialize(detach()) != rhs;
        }

        template<typename T>
        std::enable_if_t<std::is_integral_v<T>, rbool> operator>(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->greater(m_key, (int64_t)rhs);
            }
            return serialize(detach()) > rhs;
        }

        template<typename T>
        std::enable_if_t<std::is_integral_v<T>, rbool> operator<(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->less(m_key, (int64_t)rhs);
            }
            return serialize(detach()) < rhs;
        }

        template<typename T>
        std::enable_if_t<std::is_integral_v<T>, rbool> operator>=(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->greater_equal(m_key, (int64_t)rhs);
            }
            return serialize(detach()) >= rhs;
        }

        template<typename T>
        std::enable_if_t<std::is_integral_v<T>, rbool> operator<=(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->less_equal(m_key, (int64_t)rhs);
            }
            return serialize(detach()) <= rhs;
        }

        managed& operator+=(const int64_t& o) {
            auto old_val = m_obj->template get<int64_t>(m_key);
            m_obj->template set<int64_t>(this->m_key, old_val + o);
            return *this;
        }
        void operator++(int) {
            auto old_val = m_obj->template get<int64_t>(m_key);
            m_obj->template set<int64_t>(this->m_key, old_val + 1);
        }
        void operator++() {
            auto old_val = m_obj->template get<int64_t>(m_key);
            m_obj->template set<int64_t>(this->m_key, old_val + 1);
        }
        managed& operator-=(const int64_t& o) {
            auto old_val = m_obj->template get<int64_t>(m_key);
            m_obj->template set<int64_t>(this->m_key, old_val - o);
            return *this;
        }
        void operator--(int) {
            auto old_val = m_obj->template get<int64_t>(m_key);
            m_obj->template set<int64_t>(this->m_key, old_val - 1);
        }
        void operator--() {
            auto old_val = m_obj->template get<int64_t>(m_key);
            m_obj->template set<int64_t>(this->m_key, old_val - 1);
        }
        managed& operator*=(const int64_t& o) {
            auto old_val = m_obj->template get<int64_t>(m_key);
            m_obj->template set<int64_t>(this->m_key, old_val * o);
            return *this;
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
    struct managed<double> : managed_base {
        using managed<double>::managed_base::operator=;

        managed_base& operator =(const double& v) {
            this->m_obj->template set<double>(m_key, v);
            return *this;
        }

        managed_base& operator =(const int& v) {
            this->m_obj->template set<double>(m_key, (double)v);
            return *this;
        }

        [[nodiscard]] double detach() const {
            return m_obj->template get<double>(m_key);
        }

        double operator *() const {
            return detach();
        }
        [[nodiscard]] operator double() const {
            return detach();
        }

        template<typename T>
        std::enable_if_t< std::disjunction_v<std::is_integral<T>, std::is_floating_point<T>>, rbool> operator==(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->equal(m_key, (double)rhs);
            }
            return serialize(detach()) == rhs;
        }

        template<typename T>
        std::enable_if_t< std::disjunction_v<std::is_integral<T>, std::is_floating_point<T>>, rbool> operator!=(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->not_equal(m_key, (double)rhs);
            }
            return serialize(detach()) != rhs;
        }

        template<typename T>
        std::enable_if_t< std::disjunction_v<std::is_integral<T>, std::is_floating_point<T>>, rbool> operator>(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->greater(m_key, (double)rhs);
            }
            return serialize(detach()) > rhs;
        }

        template<typename T>
        std::enable_if_t< std::disjunction_v<std::is_integral<T>, std::is_floating_point<T>>, rbool> operator<(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->less(m_key, (double)rhs);
            }
            return serialize(detach()) < rhs;
        }

        template<typename T>
        std::enable_if_t< std::disjunction_v<std::is_integral<T>, std::is_floating_point<T>>, rbool> operator>=(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->greater_equal(m_key, (double)rhs);
            }
            return serialize(detach()) >= rhs;
        }

        template<typename T>
        std::enable_if_t< std::disjunction_v<std::is_integral<T>, std::is_floating_point<T>>, rbool> operator<=(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->less_equal(m_key, (double)rhs);
            }
            return serialize(detach()) <= rhs;
        }

        void operator+=(const double& o) {
            auto old_val = m_obj->template get<double>(m_key);
            m_obj->template set<double>(this->m_key, old_val + o);
        }
        void operator++(int) {
            auto old_val = m_obj->template get<double>(m_key);
            m_obj->template set<double>(this->m_key, old_val + 1.0);
        }
        void operator++() {
            auto old_val = m_obj->template get<double>(m_key);
            m_obj->template set<double>(this->m_key, old_val + 1.0);
        }
        void operator-=(const double& o) {
            auto old_val = m_obj->template get<double>(m_key);
            m_obj->template set<double>(this->m_key, old_val - o);
        }
        void operator--(int) {
            auto old_val = m_obj->template get<double>(m_key);
            m_obj->template set<double>(this->m_key, old_val - 1.0);
        }
        void operator--() {
            auto old_val = m_obj->template get<double>(m_key);
            m_obj->template set<double>(this->m_key, old_val - 1.0);
        }
        void operator*=(const double& o) {
            auto old_val = m_obj->template get<double>(m_key);
            m_obj->template set<double>(this->m_key, old_val * o);
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
    struct managed<bool> : managed_base {
        using managed<bool>::managed_base::operator=;

        [[nodiscard]] bool detach() const {
            return m_obj->template get<bool>(m_key);
        }
        [[nodiscard]] operator bool() const {
            return detach();
        }
        bool operator *() const {
            return detach();
        }

        rbool operator==(const bool& rhs) const noexcept;
        rbool operator!=(const bool& rhs) const noexcept;

    private:
        managed() = default;
        managed(const managed&) = delete;
        managed(managed &&) = delete;
        managed& operator=(const managed&) = delete;
        managed& operator=(managed&&) = delete;
        template<typename, typename>
        friend struct managed;
    };

#define CPP_REALM_MANAGED_OPTIONAL_NUMERIC(type) \
    template<> \
    struct managed<std::optional<type>> : managed_base { \
        using managed<std::optional<type>>::managed_base::operator=; \
                                                 \
        managed<std::optional<type>>& operator =(const double& v) { \
            this->m_obj->template set<type>(m_key, v); \
            return *this; \
        } \
        \
        managed<std::optional<type>>& operator =(const int& v) { \
            this->m_obj->template set<type>(m_key, (double)v); \
            return *this; \
        } \
                                                 \
        [[nodiscard]] std::optional<type> detach() const { \
            return m_obj->get_optional<type>(m_key); \
        } \
\
        [[nodiscard]] std::optional<type> operator *() const { \
            return detach(); \
        } \
        [[nodiscard]] operator std::optional<type>() const { \
            return detach(); \
        } \
        rbool operator==(const std::optional<type>& rhs) const noexcept; \
        rbool operator!=(const std::optional<type>& rhs) const noexcept; \
        void operator+=(const type& o) { \
            auto old_val = m_obj->get_optional<type>(m_key);    \
            if (!old_val) { \
                throw std::runtime_error("Cannot perform arithmetic on null value."); \
            } \
            m_obj->template set<type>(this->m_key, (*old_val) + o); \
        } \
        void operator++(int) { \
            auto old_val = m_obj->get_optional<type>(m_key);    \
            if (!old_val) { \
                throw std::runtime_error("Cannot perform arithmetic on null value."); \
            } \
            m_obj->template set<type>(this->m_key, (*old_val) + 1); \
        } \
        void operator-=(const type& o) { \
            auto old_val = m_obj->get_optional<type>(m_key);    \
            if (!old_val) { \
                throw std::runtime_error("Cannot perform arithmetic on null value."); \
            } \
            m_obj->template set<type>(this->m_key, (*old_val) - o); \
        } \
        void operator--(int) { \
            auto old_val = m_obj->get_optional<type>(m_key);    \
            if (!old_val) { \
                throw std::runtime_error("Cannot perform arithmetic on null value."); \
            } \
            m_obj->template set<type>(this->m_key, (*old_val) - 1); \
        } \
        void operator*=(const type& o) { \
            auto old_val = m_obj->get_optional<type>(m_key);    \
            if (!old_val) { \
                throw std::runtime_error("Cannot perform arithmetic on null value."); \
            } \
            m_obj->template set<type>(this->m_key, (*old_val) * o); \
        } \
        void operator*(const type& o) { \
            auto old_val = m_obj->get_optional<type>(m_key);    \
            if (!old_val) { \
                throw std::runtime_error("Cannot perform arithmetic on null value."); \
            } \
            m_obj->template set<type>(this->m_key, (*old_val) * o); \
        } \
        void operator/(const type& o) { \
            auto old_val = m_obj->get_optional<type>(m_key);    \
            if (!old_val) { \
                throw std::runtime_error("Cannot perform arithmetic on null value."); \
            } \
            m_obj->template set<type>(this->m_key, (*old_val) / o); \
        }                                        \
    private:    \
        managed() = default; \
        managed(const managed&) = delete; \
        managed(managed &&) = delete; \
        managed& operator=(const managed&) = delete; \
        managed& operator=(managed&&) = delete; \
        template<typename, typename> \
        friend struct managed; \
    }; \

CPP_REALM_MANAGED_OPTIONAL_NUMERIC(int64_t)
CPP_REALM_MANAGED_OPTIONAL_NUMERIC(double)

    template<>
    struct managed<std::optional<bool>> : managed_base {
        using managed<std::optional<bool>>::managed_base::operator=;

        [[nodiscard]] std::optional<bool> detach() const {
            return m_obj->template get_optional<bool>(m_key);
        }

        [[nodiscard]] operator std::optional<bool>() const {
            return m_obj->template get_optional<bool>(m_key);
        }

        std::optional<bool> operator *() const {
            return detach();
        }

        rbool operator==(const std::optional<bool>& rhs) const noexcept;
        rbool operator!=(const std::optional<bool>& rhs) const noexcept;

    private:
        managed() = default;
        managed(const managed&) = delete;
        managed(managed &&) = delete;
        managed& operator=(const managed&) = delete;
        managed& operator=(managed&&) = delete;
        template<typename, typename>
        friend struct managed;
    };

    template <typename T>
    struct managed<T, std::enable_if_t<std::is_enum_v<T>>> : public managed_base {
        managed<T>& operator =(const T& v) {
            m_obj->template set<int64_t>(m_key, static_cast<int64_t>(v));
            return *this;
        }

        [[nodiscard]] T detach() const {
            return static_cast<T>(m_obj->get<int64_t>(m_key));
        }

        [[nodiscard]] T operator *() const {
            return detach();
        }

        [[nodiscard]] operator T() const {
            return detach();
        }

        //MARK: -   comparison operators
        rbool operator==(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->equal(m_key, serialize(rhs));
            }
            return detach() == rhs;
        }
        rbool operator!=(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->not_equal(m_key, serialize(rhs));
            }
            return detach() != rhs;
        }
        rbool operator>(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->greater(m_key, serialize(rhs));
            }
            return detach() > rhs;
        }
        rbool operator<(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->greater(m_key, serialize(rhs));
            }
            return detach() < rhs;
        }
        rbool operator>=(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->greater_equal(m_key, serialize(rhs));
            }
            return detach() >= rhs;
        }
        rbool operator<=(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->less_equal(m_key, serialize(rhs));
            }
            return detach() <= rhs;
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

    template <typename T>
    struct managed<std::optional<T>, std::enable_if_t<std::is_enum_v<T>>> : public managed_base {
        managed<std::optional<T>>& operator =(const std::optional<T>& v) {
            if (v) {
                m_obj->template set<std::optional<int64_t>>(m_key, static_cast<int64_t>(*v));
            } else {
                m_obj->set_null(m_key);
            }
            return *this;
        }

        [[nodiscard]] std::optional<T> detach() const {
            if (auto v = m_obj->get_optional<int64_t>(m_key)) {
                return static_cast<T>(*v);
            }
            return std::nullopt;
        }

        [[nodiscard]] std::optional<T> operator *() const {
            return detach();
        }

        [[nodiscard]] operator std::optional<T>() const {
            return detach();
        }

        //MARK: -   comparison operators
        rbool operator==(const std::optional<T>& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->equal(m_key, serialize(rhs));
            }
            return detach() == rhs;
        }
        rbool operator!=(const std::optional<T>& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->not_equal(m_key, serialize(rhs));
            }
            return detach() != rhs;
        }
        rbool operator>(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->greater(m_key, rhs);
            }
            return detach() > rhs;
        }
        rbool operator<(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->less(m_key, rhs);
            }
            return detach() < rhs;
        }
        rbool operator>=(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->greater_equal(m_key, rhs);
            }
            return detach() >= rhs;
        }
        rbool operator<=(const T& rhs) const noexcept {
            if (this->m_rbool_query) {
                return this->m_rbool_query->less_equal(m_key, rhs);
            }
            return detach() <= rhs;
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
} // namespace realm

#endif//CPPREALM_MANAGED_NUMERIC_HPP
