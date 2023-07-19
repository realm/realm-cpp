#ifndef CPPREALM_MANAGED_OBJECTID_HPP
#define CPPREALM_MANAGED_OBJECTID_HPP

#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/experimental/types.hpp>

#include <cpprealm/internal/bridge/object_id.hpp>

namespace realm {
    class rbool;
}

namespace realm::experimental {
    template<>
    struct managed<realm::object_id> : managed_base {
        using managed<realm::object_id>::managed_base::operator=;
        [[nodiscard]] realm::object_id value() const {
            return m_obj->template get<realm::internal::bridge::object_id>(m_key).operator ::realm::object_id();
        }

        [[nodiscard]] realm::object_id operator *() const {
            return value();
        }

        [[nodiscard]] operator realm::object_id() const {
            return value();
        }

        //MARK: -   comparison operators
        rbool operator==(const realm::object_id& rhs) const noexcept;
        rbool operator!=(const realm::object_id& rhs) const noexcept;
    };

    template<>
    struct managed<std::optional<realm::object_id>> : managed_base {
        using managed<std::optional<realm::object_id>>::managed_base::operator=;

        [[nodiscard]] std::optional<realm::object_id> value() const {
            auto v = m_obj->template get_optional<realm::internal::bridge::object_id>(m_key);
            if (v) {
                return v.value().operator ::realm::object_id();
            } else {
                return std::nullopt;
            }
        }

        [[nodiscard]] std::optional<realm::object_id> operator *() const {
            return value();
        }

        [[nodiscard]] operator std::optional<realm::object_id>() const {
            return value();
        }

        //MARK: -   comparison operators
        rbool operator==(const std::optional<realm::object_id>& rhs) const noexcept;
        rbool operator!=(const std::optional<realm::object_id>& rhs) const noexcept;
    };

} // namespace realm::experimental

#endif//CPPREALM_MANAGED_OBJECTID_HPP
