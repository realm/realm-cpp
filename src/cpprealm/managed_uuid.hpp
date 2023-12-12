#ifndef CPPREALM_MANAGED_UUID_HPP
#define CPPREALM_MANAGED_UUID_HPP

#include "macros.hpp"
#include "types.hpp"

namespace realm {
    class rbool;
}
namespace realm {

    template<>
    struct managed<realm::uuid> : managed_base {
        using managed<realm::uuid>::managed_base::operator=;

        [[nodiscard]] realm::uuid detach() const {
            return m_obj->template get<realm::internal::bridge::uuid>(m_key).operator ::realm::uuid();
        }

        [[nodiscard]] realm::uuid operator *() const {
            return detach();
        }

        [[nodiscard]] operator realm::uuid() const {
            return detach();
        }

        //MARK: -   comparison operators
        rbool operator==(const realm::uuid& rhs) const noexcept;
        rbool operator!=(const realm::uuid& rhs) const noexcept;
    };

    template<>
    struct managed<std::optional<realm::uuid>> : managed_base {
        using managed<std::optional<realm::uuid>>::managed_base::operator=;

        [[nodiscard]] std::optional<realm::uuid> detach() const {
            auto v = m_obj->template get_optional<realm::internal::bridge::uuid>(m_key);
            if (v) {
                return v.value().operator ::realm::uuid();
            } else {
                return std::nullopt;
            }
        }

        [[nodiscard]] std::optional<realm::uuid> operator *() const {
            return detach();
        }

        [[nodiscard]] operator std::optional<realm::uuid>() const {
            return detach();
        }

        //MARK: -   comparison operators
        rbool operator==(const std::optional<realm::uuid>& rhs) const noexcept;
        rbool operator!=(const std::optional<realm::uuid>& rhs) const noexcept;
    };

} // namespace realm


#endif//CPPREALM_MANAGED_UUID_HPP
