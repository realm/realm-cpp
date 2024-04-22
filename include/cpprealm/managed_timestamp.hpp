////////////////////////////////////////////////////////////////////////////
//
// Copyright 2022 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef CPPREALM_MANAGED_TIMESTAMP_HPP
#define CPPREALM_MANAGED_TIMESTAMP_HPP

#include <cpprealm/internal/bridge/timestamp.hpp>
#include <cpprealm/macros.hpp>

namespace realm {
    class rbool;
}
namespace realm {

    template<>
    struct managed<std::chrono::time_point<std::chrono::system_clock>> : public managed_base {
        using value_type = std::chrono::time_point<std::chrono::system_clock>;
        using managed<std::chrono::time_point<std::chrono::system_clock>>::managed_base::operator=;

        [[nodiscard]] std::chrono::time_point<std::chrono::system_clock> detach() const {
            return m_obj->template get<realm::internal::bridge::timestamp>(m_key);
        }

        [[nodiscard]] operator std::chrono::time_point<std::chrono::system_clock>() const {
            return detach();
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
        rbool operator>(const std::chrono::time_point<std::chrono::system_clock>& rhs) const noexcept;
        rbool operator>=(const std::chrono::time_point<std::chrono::system_clock>& rhs) const noexcept;
        rbool operator<(const std::chrono::time_point<std::chrono::system_clock>& rhs) const noexcept;
        rbool operator<=(const std::chrono::time_point<std::chrono::system_clock>& rhs) const noexcept;

    private:
        managed() = default;
        managed(const managed&) = delete;
        managed(managed &&) = delete;
        managed& operator=(const managed&) = delete;
        managed& operator=(managed&&) = delete;
        template<typename, typename>
        friend struct managed;
    };

    template<>
    struct managed<std::optional<std::chrono::time_point<std::chrono::system_clock>>> : managed_base {
        using value_type = std::optional<std::chrono::time_point<std::chrono::system_clock>>;
        using managed<std::optional<std::chrono::time_point<std::chrono::system_clock>>>::managed_base::operator=;

        [[nodiscard]] std::optional<std::chrono::time_point<std::chrono::system_clock>> detach() const {
            return m_obj->get_optional<realm::internal::bridge::timestamp>(m_key);
        }

        [[nodiscard]] operator std::optional<std::chrono::time_point<std::chrono::system_clock>>() const {
            return detach();
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

    private:
        managed() = default;
        managed(const managed&) = delete;
        managed(managed &&) = delete;
        managed& operator=(const managed&) = delete;
        managed& operator=(managed&&) = delete;
        template<typename, typename>
        friend struct managed;
    };

} // namespace realm


#endif//CPPREALM_MANAGED_TIMESTAMP_HPP
