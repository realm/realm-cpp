#ifndef CPPREALM_MANAGED_PRIMARY_KEY_HPP
#define CPPREALM_MANAGED_PRIMARY_KEY_HPP

#include "rbool.hpp"

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
        struct managed<primary_key<int64_t>> : managed_base {
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
        };

        template<>
        struct managed<primary_key<std::string>> : managed_base {
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
        };

        template<>
        struct managed<primary_key<realm::object_id>> : managed_base {
            primary_key<realm::object_id> detach() const {
                return operator realm::object_id();
            }

            operator realm::object_id() const {
                return m_obj->template get<internal::bridge::object_id>(m_key).operator ::realm::object_id();
            }

            rbool operator==(const realm::object_id& rhs) const noexcept;
            rbool operator!=(const realm::object_id& rhs) const noexcept;
        };

        template<typename T>
        struct managed<primary_key<T>, std::enable_if_t<std::is_enum_v<T>>> : managed_base {
            primary_key<T> detach() const {
                return operator T();
            }

            operator T() const {
                return static_cast<T>(m_obj->template get<int64_t>(m_key));
            }

            rbool operator==(const T& rhs) const noexcept {
                if (this->should_detect_usage_for_queries) {
                    auto query = internal::bridge::query(this->query->get_table());
                    query.equal(this->m_key, serialize(rhs));
                    return query;
                }
                return serialize(detach().value) == serialize(rhs);
            }
            rbool operator!=(const T& rhs) const noexcept {
                if (this->should_detect_usage_for_queries) {
                    auto query = internal::bridge::query(this->query->get_table());
                    query.not_equal(this->m_key, serialize(rhs));
                    return query;
                }
                return serialize(detach().value) != serialize(rhs);
            }
        };

        template<>
        struct managed<primary_key<std::optional<int64_t>>> : managed_base {
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
        };

        template<typename T>
        struct managed<primary_key<T>, std::enable_if_t<std::conjunction_v<typename internal::type_info::is_optional<T>,
                                                                           std::is_enum<typename T::value_type> >>> : managed_base {
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
                if (this->should_detect_usage_for_queries) {
                    auto query = internal::bridge::query(this->query->get_table());
                    if (auto r = serialize(rhs)) {
                        query.equal(this->m_key, *r);
                    } else {
                        query.equal(this->m_key, std::nullopt);
                    }
                    return query;
                }
                return serialize(detach().value) == serialize(rhs);
            }
            rbool operator!=(const T& rhs) const noexcept {
                if (this->should_detect_usage_for_queries) {
                    auto query = internal::bridge::query(this->query->get_table());
                    if (auto r = serialize(rhs)) {
                        query.not_equal(this->m_key, *r);
                    } else {
                        query.not_equal(this->m_key, std::nullopt);
                    }
                    return query;
                }
                return serialize(detach().value) != serialize(rhs);
            }
        };

        template<>
        struct managed<primary_key<std::optional<std::string>>> : managed_base {
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
        };

        template<>
        struct managed<primary_key<std::optional<realm::uuid>>> : managed_base {
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
        };

        template<>
        struct managed<primary_key<std::optional<realm::object_id>>> : managed_base {
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
        };
}

#endif //CPPREALM_MANAGED_PRIMARY_KEY_HPP
