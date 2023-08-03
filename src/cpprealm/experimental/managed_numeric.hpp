#ifndef CPPREALM_MANAGED_NUMERIC_HPP
#define CPPREALM_MANAGED_NUMERIC_HPP

#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/persisted.hpp>

namespace realm::experimental {
    template<>
    struct managed<int64_t> : managed_base {
        using managed<int64_t>::managed_base::operator=;

        managed_base& operator =(const int64_t& v) {
            this->m_obj->template set<int64_t>(m_key, v);
            return *this;
        }

        [[nodiscard]] int64_t value() const {
            return m_obj->template get<int64_t>(m_key);
        }

        [[nodiscard]] int64_t operator *() const {
            return value();
        }

        [[nodiscard]] operator int64_t() const {
            return value();
        }

        template<typename T>
        std::enable_if_t<std::is_integral_v<T>, rbool> operator==(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.equal(this->m_key, (int64_t)rhs);
                return rbool(std::move(query));
            }
            return serialize(value()) == rhs;
        }

        template<typename T>
        std::enable_if_t<std::is_integral_v<T>, rbool> operator!=(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.not_equal(this->m_key, (int64_t)rhs);
                return rbool(std::move(query));
            }
            return serialize(value()) != rhs;
        }

        template<typename T>
        std::enable_if_t<std::is_integral_v<T>, rbool> operator>(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.greater(this->m_key, (int64_t)rhs);
                return rbool(std::move(query));
            }
            return serialize(value()) > rhs;
        }

        template<typename T>
        std::enable_if_t<std::is_integral_v<T>, rbool> operator<(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.less(this->m_key, (int64_t)rhs);
                return rbool(std::move(query));
            }
            return serialize(value()) < rhs;
        }

        template<typename T>
        std::enable_if_t<std::is_integral_v<T>, rbool> operator>=(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.greater_equal(this->m_key, (int64_t)rhs);
                return rbool(std::move(query));
            }
            return serialize(value()) >= rhs;
        }

        template<typename T>
        std::enable_if_t<std::is_integral_v<T>, rbool> operator<=(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.less_equal(this->m_key, (int64_t)rhs);
                return rbool(std::move(query));
            }
            return serialize(value()) <= rhs;
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
        managed& operator-=(const int64_t& o) {
            auto old_val = m_obj->template get<int64_t>(m_key);
            m_obj->template set<int64_t>(this->m_key, old_val - o);
            return *this;
        }
        void operator--(int) {
            auto old_val = m_obj->template get<int64_t>(m_key);
            m_obj->template set<int64_t>(this->m_key, old_val - 1);
        }
        managed& operator*=(const int64_t& o) {
            auto old_val = m_obj->template get<int64_t>(m_key);
            m_obj->template set<int64_t>(this->m_key, old_val * o);
            return *this;
        }
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

        [[nodiscard]] double value() const {
            return m_obj->template get<double>(m_key);
        }

        double operator *() const {
            return value();
        }
        [[nodiscard]] operator double() const {
            return value();
        }

        template<typename T>
        std::enable_if_t< std::disjunction_v<std::is_integral<T>, std::is_floating_point<T>>, rbool> operator==(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.equal(this->m_key, (double)rhs);
                return rbool(std::move(query));
            }
            return serialize(value()) == rhs;
        }

        template<typename T>
        std::enable_if_t< std::disjunction_v<std::is_integral<T>, std::is_floating_point<T>>, rbool> operator!=(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.not_equal(this->m_key, (double)rhs);
                return rbool(std::move(query));
            }
            return serialize(value()) != rhs;
        }

        template<typename T>
        std::enable_if_t< std::disjunction_v<std::is_integral<T>, std::is_floating_point<T>>, rbool> operator>(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.greater(this->m_key, (double)rhs);
                return rbool(std::move(query));
            }
            return serialize(value()) > rhs;
        }

        template<typename T>
        std::enable_if_t< std::disjunction_v<std::is_integral<T>, std::is_floating_point<T>>, rbool> operator<(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.less(this->m_key, (double)rhs);
                return rbool(std::move(query));
            }
            return serialize(value()) < rhs;
        }

        template<typename T>
        std::enable_if_t< std::disjunction_v<std::is_integral<T>, std::is_floating_point<T>>, rbool> operator>=(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.greater_equal(this->m_key, (double)rhs);
                return rbool(std::move(query));
            }
            return serialize(value()) >= rhs;
        }

        template<typename T>
        std::enable_if_t< std::disjunction_v<std::is_integral<T>, std::is_floating_point<T>>, rbool> operator<=(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.less_equal(this->m_key, (double)rhs);
                return rbool(std::move(query));
            }
            return serialize(value()) <= rhs;
        }

        void operator+=(const double& o) {
            auto old_val = m_obj->template get<double>(m_key);
            m_obj->template set<double>(this->m_key, old_val + o);
        }
        void operator++(int) {
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
        void operator*=(const double& o) {
            auto old_val = m_obj->template get<double>(m_key);
            m_obj->template set<double>(this->m_key, old_val * o);
        }
    };

    template<>
    struct managed<bool> : managed_base {
        using managed<bool>::managed_base::operator=;

        [[nodiscard]] bool value() const {
            return m_obj->template get<bool>(m_key);
        }
        [[nodiscard]] operator bool() const {
            return value();
        }
        bool operator *() const {
            return value();
        }

        rbool operator==(const bool& rhs) const noexcept;
        rbool operator!=(const bool& rhs) const noexcept;
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
        [[nodiscard]] std::optional<type> value() const { \
            return m_obj->get_optional<type>(m_key); \
        } \
\
        [[nodiscard]] std::optional<type> operator *() const { \
            return value(); \
        } \
        [[nodiscard]] operator std::optional<type>() const { \
            return value(); \
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
        } \
    }; \

CPP_REALM_MANAGED_OPTIONAL_NUMERIC(int64_t);
CPP_REALM_MANAGED_OPTIONAL_NUMERIC(double);

    template<>
    struct managed<std::optional<bool>> : managed_base {
        using managed<std::optional<bool>>::managed_base::operator=;

        [[nodiscard]] std::optional<bool> value() const {
            return m_obj->template get_optional<bool>(m_key);
        }

        [[nodiscard]] operator std::optional<bool>() const {
            return m_obj->template get_optional<bool>(m_key);
        }

        std::optional<bool> operator *() const {
            return value();
        }

        rbool operator==(const std::optional<bool>& rhs) const noexcept;
        rbool operator!=(const std::optional<bool>& rhs) const noexcept;
    };

    template <typename T>
    struct managed<T, std::enable_if_t<std::is_enum_v<T>>> : public managed_base {
        managed<T>& operator =(const T& v) {
            m_obj->template set<int64_t>(m_key, static_cast<int64_t>(v));
            return *this;
        }

        [[nodiscard]] T value() const {
            return static_cast<T>(m_obj->get<int64_t>(m_key));
        }

        [[nodiscard]] T operator *() const {
            return value();
        }

        [[nodiscard]] operator T() const {
            return value();
        }

        //MARK: -   comparison operators
        rbool operator==(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.equal(this->m_key, serialize(rhs));
                return query;
            }
            return value() == rhs;
        }
        rbool operator!=(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.not_equal(this->m_key, serialize(rhs));
                return query;
            }
            return value() != rhs;
        }
        rbool operator>(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.greater(this->m_key, serialize(rhs));
                return query;
            }
            return value() > rhs;
        }
        rbool operator<(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.less(this->m_key, serialize(rhs));
                return query;
            }
            return value() < rhs;
        }
        rbool operator>=(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.greater_equal(this->m_key, serialize(rhs));
                return query;
            }
            return value() >= rhs;
        }
        rbool operator<=(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.less_equal(this->m_key, serialize(rhs));
                return query;
            }
            return value() <= rhs;
        }
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

        [[nodiscard]] std::optional<T> value() const {
            if (auto v = m_obj->get_optional<int64_t>(m_key)) {
                return static_cast<T>(*v);
            }
            return std::nullopt;
        }

        [[nodiscard]] std::optional<T> operator *() const {
            return value();
        }

        [[nodiscard]] operator std::optional<T>() const {
            return value();
        }

        //MARK: -   comparison operators
        rbool operator==(const std::optional<T>& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                if (auto r = rhs) {
                    query.equal(this->m_key, serialize(*r));
                } else {
                    query.equal(this->m_key, std::nullopt);
                }
                return query;
            }
            return value() == rhs;
        }
        rbool operator!=(const std::optional<T>& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                if (auto r = rhs) {
                    query.not_equal(this->m_key, serialize(*r));
                } else {
                    query.not_equal(this->m_key, std::nullopt);
                }
                return query;
            }
            return value() != rhs;
        }
        rbool operator>(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.greater(this->m_key, serialize(rhs));
                return query;
            }
            return value() > rhs;
        }
        rbool operator<(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.less(this->m_key, serialize(rhs));
                return query;
            }
            return value() < rhs;
        }
        rbool operator>=(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.greater_equal(this->m_key, serialize(rhs));
                return query;
            }
            return value() >= rhs;
        }
        rbool operator<=(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.less_equal(this->m_key, serialize(rhs));
                return query;
            }
            return value() <= rhs;
        }
    };
} // namespace realm::experimental

#endif//CPPREALM_MANAGED_NUMERIC_HPP
