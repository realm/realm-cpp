#ifndef CPPREALM_MANAGED_PRIMARY_KEY_HPP
#define CPPREALM_MANAGED_PRIMARY_KEY_HPP

#include <cpprealm/experimental/macros.hpp>

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

//            using internal_type = typename ::realm::internal::type_info::type_info<T, void>::internal_type;

            operator T() {
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

            std::enable_if_t<std::is_integral_v<T>, rbool>
            operator==(const int& rhs) const noexcept;
            std::enable_if_t<std::is_integral_v<T>, rbool>
            operator!=(const int& rhs) const noexcept;
            std::enable_if_t<std::is_integral_v<T>, rbool>
            operator>(const int& rhs) const noexcept;
            std::enable_if_t<std::is_integral_v<T>, rbool>
            operator>=(const int& rhs) const noexcept;
            std::enable_if_t<std::is_integral_v<T>, rbool>
            operator<(const int& rhs) const noexcept;
            std::enable_if_t<std::is_integral_v<T>, rbool>
            operator<=(const int& rhs) const noexcept;
        };
}

#endif//CPPREALM_MANAGED_PRIMARY_KEY_HPP
