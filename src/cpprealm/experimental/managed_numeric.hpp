#ifndef REALM_MANAGED_NUMERIC_HPP
#define REALM_MANAGED_NUMERIC_HPP

#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/persisted.hpp>

namespace realm::experimental {
#define CPP_REALM_MANAGED_NUMERIC(type) \
    template<> \
    struct managed<type> : managed_base { \
        managed_base& operator =(const type& v) { \
            this->m_obj->template set<type>(m_key, v); \
            return *this; \
        } \
                                        \
        [[nodiscard]] type value() const { \
            return m_obj->template get<type>(m_key); \
        } \
\
        type operator *() const { \
            return value(); \
        } \
        rbool operator==(const type& rhs) const noexcept; \
        rbool operator!=(const type& rhs) const noexcept; \
        rbool operator>(const type& rhs) const noexcept; \
        rbool operator<(const type& rhs) const noexcept;  \
        rbool operator>=(const type& rhs) const noexcept; \
        rbool operator<=(const type& rhs) const noexcept; \
        void operator+=(const type& o) { \
            auto old_val = m_obj->template get<type>(m_key); \
            m_obj->template set<type>(this->m_key, old_val + o); \
        } \
        void operator++() { \
            auto old_val = m_obj->template get<type>(m_key); \
            m_obj->template set<type>(this->m_key, old_val++); \
        } \
        void operator-=(const type& o) { \
            auto old_val = m_obj->template get<type>(m_key); \
            m_obj->template set<type>(this->m_key, old_val - o); \
        } \
        void operator--() { \
            auto old_val = m_obj->template get<type>(m_key); \
            m_obj->template set<type>(this->m_key, old_val--); \
        } \
        void operator*=(const type& o) { \
            auto old_val = m_obj->template get<type>(m_key); \
            m_obj->template set<type>(this->m_key, old_val * o); \
        } \
    }; \

CPP_REALM_MANAGED_NUMERIC(int64_t);
CPP_REALM_MANAGED_NUMERIC(double);

    template<>
    struct managed<bool> : managed_base {
        using managed<bool>::managed_base::operator=;

        [[nodiscard]] bool value() const {
            return m_obj->template get<bool>(m_key);
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
        [[nodiscard]] std::optional<type> value() const { \
            return m_obj->get_optional<type>(m_key); \
        } \
\
        [[nodiscard]] std::optional<type> operator *() const { \
            return value(); \
        } \
\
        rbool operator==(const std::optional<type>& rhs) const noexcept; \
        rbool operator!=(const std::optional<type>& rhs) const noexcept; \
        void operator+=(const type& o) { \
            auto old_val = m_obj->get_optional<type>(m_key);    \
            if (!old_val) { \
                throw std::runtime_error("Cannot perform arithmetic on null value."); \
            } \
            m_obj->template set<type>(this->m_key, (*old_val) + o); \
        } \
        void operator++() { \
            auto old_val = m_obj->get_optional<type>(m_key);    \
            if (!old_val) { \
                throw std::runtime_error("Cannot perform arithmetic on null value."); \
            } \
            m_obj->template set<type>(this->m_key, (*old_val)++); \
        } \
        void operator-=(const type& o) { \
            auto old_val = m_obj->get_optional<type>(m_key);    \
            if (!old_val) { \
                throw std::runtime_error("Cannot perform arithmetic on null value."); \
            } \
            m_obj->template set<type>(this->m_key, (*old_val) - o); \
        } \
        void operator--() { \
            auto old_val = m_obj->get_optional<type>(m_key);    \
            if (!old_val) { \
                throw std::runtime_error("Cannot perform arithmetic on null value."); \
            } \
            m_obj->template set<type>(this->m_key, (*old_val)--); \
        } \
        void operator*=(const type& o) { \
            auto old_val = m_obj->get_optional<type>(m_key);    \
            if (!old_val) { \
                throw std::runtime_error("Cannot perform arithmetic on null value."); \
            } \
            m_obj->template set<type>(this->m_key, (*old_val) * o); \
        } \
    };                                           \

CPP_REALM_MANAGED_OPTIONAL_NUMERIC(int64_t);
CPP_REALM_MANAGED_OPTIONAL_NUMERIC(double);

    template<>
    struct managed<std::optional<bool>> : managed_base {
        using managed<std::optional<bool>>::managed_base::operator=;

        [[nodiscard]] std::optional<bool> value() const {
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

#endif//REALM_MANAGED_NUMERIC_HPP
