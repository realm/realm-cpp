#ifndef CPPREALM_MANAGED_UUID_HPP
#define CPPREALM_MANAGED_UUID_HPP

#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/experimental/types.hpp>

namespace realm {
    class rbool;
}
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

        [[nodiscard]] operator realm::uuid() const {
            return value();
        }

        //MARK: -   comparison operators
        rbool operator==(const realm::uuid& rhs) const noexcept;
        rbool operator!=(const realm::uuid& rhs) const noexcept;
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

        [[nodiscard]] operator std::optional<realm::uuid>() const {
            return value();
        }

        //MARK: -   comparison operators
        rbool operator==(const std::optional<realm::uuid>& rhs) const noexcept;
        rbool operator!=(const std::optional<realm::uuid>& rhs) const noexcept;
    };

} // namespace realm::experimental


#endif//CPPREALM_MANAGED_UUID_HPP
