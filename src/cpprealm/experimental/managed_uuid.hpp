#ifndef REALM_MANAGED_UUID_HPP
#define REALM_MANAGED_UUID_HPP

#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/experimental/types.hpp>

namespace realm::experimental {

    template<>
    struct managed<realm::uuid> : managed_base {
        using managed<realm::uuid>::managed_base::operator=;

        [[nodiscard]] realm::uuid value() const {
            return m_obj->template get<realm::internal::bridge::uuid>(m_key).operator ::realm::uuid();
        }

        [[nodiscard]] realm::uuid operator *() const {
            return value();
        }

        //MARK: -   comparison operators
        inline bool operator==(const realm::uuid& rhs) const noexcept {
            return value().m_uuid == rhs.m_uuid;
        }
        inline bool operator!=(const realm::uuid& rhs) const noexcept {
            return value().m_uuid != rhs.m_uuid;
        }
    };

    template<>
    struct managed<std::optional<realm::uuid>> : managed_base {
        using managed<std::optional<realm::uuid>>::managed_base::operator=;

        [[nodiscard]] std::optional<realm::uuid> value() const {
            auto v = m_obj->template get_optional<realm::internal::bridge::uuid>(m_key);
            if (v) {
                return v.value().operator ::realm::uuid();
            } else {
                return std::nullopt;
            }
        }

        [[nodiscard]] std::optional<realm::uuid> operator *() const {
            return value();
        }

        //MARK: -   comparison operators
        inline bool operator==(const std::optional<realm::uuid>& rhs) const noexcept {
            auto lhs = value();
            if (lhs && rhs) {
                return value()->m_uuid == rhs->m_uuid;
            } else if (!lhs && !rhs) {
                return true;
            } else {
                return false;
            }
        }
        inline bool operator!=(const std::optional<realm::uuid>& rhs) const noexcept {
            auto lhs = value();
            if (lhs && rhs) {
                return value()->m_uuid != rhs->m_uuid;
            } else if (!lhs && !rhs) {
                return false;
            } else {
                return true;
            }
        }
    };

} // namespace realm::experimental


#endif//REALM_MANAGED_UUID_HPP
