#ifndef CPPREALM_MANAGED_TIMESTAMP_HPP
#define CPPREALM_MANAGED_TIMESTAMP_HPP

#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/internal/bridge/timestamp.hpp>

namespace realm {
    class rbool;
};
namespace realm::experimental {

    template<>
    struct managed<std::chrono::time_point<std::chrono::system_clock>> : public managed_base {
        using managed<std::chrono::time_point<std::chrono::system_clock>>::managed_base::operator=;

        [[nodiscard]] std::chrono::time_point<std::chrono::system_clock> value() const {
            return m_obj->template get<realm::internal::bridge::timestamp>(m_key);
        }

        [[nodiscard]] operator std::chrono::time_point<std::chrono::system_clock>() const {
            return value();
        }

        auto time_since_epoch() const {
            auto ts = m_obj->template get<internal::bridge::timestamp>(m_key);
            return ts.get_time_point().time_since_epoch();
        }

        template <typename S>
        void operator+=(const std::chrono::duration<S>& rhs) {
            auto ts = m_obj->template get<internal::bridge::timestamp>(m_key);
            m_obj->set(m_key, internal::bridge::timestamp(ts.get_time_point() + rhs));
        }

        //MARK: -   comparison operators
        rbool operator==(const std::chrono::time_point<std::chrono::system_clock>& rhs) const noexcept;
        rbool operator!=(const std::chrono::time_point<std::chrono::system_clock>& rhs) const noexcept;
    };

    template<>
    struct managed<std::optional<std::chrono::time_point<std::chrono::system_clock>>> : managed_base {
        using managed<std::optional<std::chrono::time_point<std::chrono::system_clock>>>::managed_base::operator=;

        [[nodiscard]] std::optional<std::chrono::time_point<std::chrono::system_clock>> value() const {
            return m_obj->template get<realm::internal::bridge::timestamp>(m_key);
        }

        [[nodiscard]] operator std::optional<std::chrono::time_point<std::chrono::system_clock>>() const {
            return value();
        }

        struct box {
            template <typename S>
            void operator+=(const std::chrono::duration<S>& rhs) {
                auto ts = m_parent.get().m_obj->get_optional<internal::bridge::timestamp>(m_parent.get().m_key);
                m_parent.get().m_obj->set(m_parent.get().m_key, internal::bridge::timestamp(ts->get_time_point() + rhs));
            }
            auto time_since_epoch() const {
                auto ts = m_parent.get().m_obj->get_optional<internal::bridge::timestamp>(m_parent.get().m_key);
                return ts->get_time_point().time_since_epoch();
            }
        private:
            box(managed& parent) : m_parent(parent) { }
            std::reference_wrapper<managed<std::optional<std::chrono::time_point<std::chrono::system_clock>>>> m_parent;
            friend struct managed<std::optional<std::chrono::time_point<std::chrono::system_clock>>>;
        };

        std::unique_ptr<box> operator->()
        {
            return std::make_unique<box>(box(*this));
        }
        [[nodiscard]] box operator*() {
            return box(*this);
        }

        //MARK: -   comparison operators
        rbool operator==(const std::optional<std::chrono::time_point<std::chrono::system_clock>>& rhs) const noexcept;
        rbool operator!=(const std::optional<std::chrono::time_point<std::chrono::system_clock>>& rhs) const noexcept;
    };

} // namespace realm::experimental


#endif//CPPREALM_MANAGED_TIMESTAMP_HPP
