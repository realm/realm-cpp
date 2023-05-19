#ifndef REALM_MANAGED_MIXED_HPP
#define REALM_MANAGED_MIXED_HPP

#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/experimental/types.hpp>

namespace realm::experimental {
    template <typename T>
    struct managed<T, std::enable_if_t<realm::internal::type_info::MixedPersistableConcept<T>::value>> : public managed_base {
        managed& operator =(const T& v) {
            m_obj->set(m_key, std::visit([](auto&& arg) {
                           using M = typename internal::type_info::type_info<std::decay_t<decltype(arg)>>::internal_type;
                           return internal::bridge::mixed(M(arg));
                       }, v));
            return *this;
        }

        [[nodiscard]] T value() const {
            return deserialize<T>(m_obj->get<realm::internal::bridge::mixed>(m_key));
        }

        [[nodiscard]] T operator *() const {
            return value();
        }

        inline bool operator==(const T& rhs) const noexcept {
            return value() == rhs;
        }
        inline bool operator!=(const T& rhs) const noexcept {
            return value() != rhs;
        }
    };
}

#endif//REALM_MANAGED_MIXED_HPP
