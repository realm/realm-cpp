#ifndef REALM_MANAGED_BINARY_HPP
#define REALM_MANAGED_BINARY_HPP

#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/experimental/types.hpp>

namespace realm::experimental {

    template<>
    struct managed<std::vector<uint8_t>> : managed_base {
        using managed<std::vector<uint8_t>>::managed_base::operator=;

        [[nodiscard]] std::vector<uint8_t> value() const {
            return m_obj->template get<realm::internal::bridge::binary>(m_key);
        }

        std::vector<uint8_t> operator*() const {
            return value();
        }

        //MARK: -   comparison operators
        inline bool operator==(const std::vector<uint8_t>& rhs) const noexcept {
            return value() == rhs;
        }
        inline bool operator!=(const std::vector<uint8_t>& rhs) const noexcept {
            return value() != rhs;
        }

        void push_back(uint8_t v) {
            auto v2 = m_obj->template get<internal::bridge::binary>(m_key).operator std::vector<uint8_t>();
            v2.push_back(v);
            m_obj->template set<internal::bridge::binary>(m_key, v2);
        }
    };

    template<>
    struct managed<std::optional<std::vector<uint8_t>>> : managed_base {
        using managed<std::optional<std::vector<uint8_t>>>::managed_base::operator=;

        [[nodiscard]] std::optional<std::vector<uint8_t>> value() const {
            return deserialize(m_obj->template get_optional<realm::internal::bridge::binary>(m_key));
        }

        std::optional<std::vector<uint8_t>> operator*() const {
            return value();
        }

        //MARK: -   comparison operators
        inline bool operator==(const std::optional<std::vector<uint8_t>>& rhs) const noexcept {
            return value() == rhs;
        }
        inline bool operator!=(const std::optional<std::vector<uint8_t>>& rhs) const noexcept {
            return value() != rhs;
        }
    };
}


#endif//REALM_MANAGED_BINARY_HPP
