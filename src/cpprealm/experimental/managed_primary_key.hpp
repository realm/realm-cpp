#ifndef CPPREALM_MANAGED_PRIMARY_KEY_HPP
#define CPPREALM_MANAGED_PRIMARY_KEY_HPP

#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/experimental/types.hpp>

namespace realm {
class rbool;
};
namespace realm::experimental {
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
            operator T() {
                return value;
            }
        };

        template<>
        struct primary_key<int> {
            using internal_type = int64_t;
            primary_key() = default;
            primary_key(const int& v) {
                this->value = v;
            }
            int64_t value;
            operator int64_t() {
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
            int64_t value;
            operator int64_t() {
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
            operator object_id() {
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
            std::string value;
            operator std::string() {
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
            operator uuid() {
                return value;
            }
        };

        template<typename T>
        struct managed<primary_key<T>> : managed_base {
            using managed<primary_key<T>>::managed_base::operator=;

            primary_key<T> value() const {
                return m_obj->template get<T>(m_key);
            }

            operator T() {
                return m_obj->template get<T>(m_key);
            }

            rbool operator==(const T& rhs) const noexcept;
            rbool operator!=(const T& rhs) const noexcept;
            rbool operator>(const T& rhs) const noexcept;
            rbool operator>=(const T& rhs) const noexcept;
            rbool operator<(const T& rhs) const noexcept;
            rbool operator<=(const T& rhs) const noexcept;
        };
}

#endif//CPPREALM_MANAGED_PRIMARY_KEY_HPP
