#ifndef CPPREALM_MANAGED_MIXED_HPP
#define CPPREALM_MANAGED_MIXED_HPP

#include <cpprealm/macros.hpp>
#include <cpprealm/rbool.hpp>
#include <cpprealm/types.hpp>

namespace realm {
    template <typename T>
    struct managed<T, std::enable_if_t<realm::internal::type_info::MixedPersistableConcept<T>::value>> : public managed_base {
        using managed<T>::managed_base::operator=;

        managed& operator =(const T& v) {
            m_obj->set(m_key, std::visit([](auto&& arg) {
                           using M = typename internal::type_info::type_info<std::decay_t<decltype(arg)>>::internal_type;
                           return internal::bridge::mixed(M(arg));
                       }, v));
            return *this;
        }

        template<typename U>
        managed& operator =(const U& v) {
            m_obj->set(m_key, internal::bridge::mixed(v));
            return *this;
        }

        [[nodiscard]] T detach() const {
            return deserialize<T>(m_obj->get<realm::internal::bridge::mixed>(m_key));
        }

        [[nodiscard]] T operator *() const {
            return detach();
        }

        //MARK: -   comparison operators
        rbool operator==(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.equal(this->m_key, serialize(rhs));
                return query;
            }
            return detach() == rhs;
        }

        rbool operator!=(const T& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.not_equal(this->m_key, serialize(rhs));
                return query;
            }
            return detach() != rhs;
        }

        rbool operator==(const std::nullopt_t& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.equal(this->m_key, rhs);
                return query;
            }
            return detach() == T(std::monostate());
        }

        rbool operator!=(const std::nullopt_t& rhs) const noexcept {
            if (this->should_detect_usage_for_queries) {
                auto query = internal::bridge::query(this->query->get_table());
                query.not_equal(this->m_key, rhs);
                return query;
            }
            return detach() != T(std::monostate());
        }
    };
}

#endif//CPPREALM_MANAGED_MIXED_HPP
