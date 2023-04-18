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


#ifndef REALM_PERSISTED_INT_HPP
#define REALM_PERSISTED_INT_HPP

#include <cpprealm/persisted.hpp>
#include <cpprealm/persisted_custom.hpp>

namespace realm {
    template <>
    struct persisted<int64_t> : public persisted_primitive_base<int64_t> {
        using persisted_primitive_base<int64_t>::persisted_primitive_base;

        template <typename T>
        std::enable_if_t<std::is_arithmetic_v<T>, persisted&> // NOLINT(misc-unconventional-assign-operator)
        operator=(const T& o) {
            if (this->is_managed()) {
                this->m_object->get_obj().template set<int64_t>(
                        this->managed.value(),
                        static_cast<int64_t>(o));
            } else {
                new (&this->unmanaged) T(o);
            }
            return *this;
        }
        template<typename T>
        std::enable_if_t<std::is_integral_v<T>>
        operator+=(const T& o) {
            if (this->is_managed()) {
                auto old_val = deserialize(this->m_object->get_obj().get<int64_t>(this->managed.value()));
                this->m_object->get_obj().template set<int64_t>(
                        this->managed.value(),
                        static_cast<int64_t>(old_val + o));
            } else {
                this->unmanaged += T(o);
            }
        }
        void operator++() {
            if (this->is_managed()) {
                auto old_val = deserialize(this->m_object->get_obj().get<int64_t>(this->managed.value()));
                this->m_object->get_obj().template set<int64_t>(
                        this->managed.value(),
                        static_cast<int64_t>(++old_val));
            } else {
                ++this->unmanaged.value();
            }
        }
        template<typename T>
        std::enable_if_t<std::is_integral_v<T>>
        operator-=(const T& o) {
            if (this->is_managed()) {
                auto old_val = deserialize(this->m_object->get_obj().get<int64_t>(this->managed.value()));
                this->m_object->get_obj().template set<int64_t>(
                        this->managed.value(),
                        static_cast<int64_t>(old_val - o));
            } else {
                this->unmanaged.value() -= T(o);
            }
        }
        void operator--() {
            if (this->is_managed()) {
                auto old_val = deserialize(this->m_object->get_obj().get<int64_t>(this->managed.value()));
                this->m_object->get_obj().template set<int64_t>(
                        this->managed.value(),
                        static_cast<int64_t>(--old_val));
            } else {
                --this->unmanaged.value();
            }
        }
        template<typename T>
        std::enable_if_t<std::is_integral_v<T>>
        operator*=(const T& o) {
            if (this->is_managed()) {
                auto old_val = deserialize(this->m_object->get_obj().get<int64_t>(this->managed.value()));
                this->m_object->get_obj().template set<int64_t>(
                        this->managed.value(),
                        static_cast<int64_t>(old_val * o));
            } else {
                this->unmanaged.value() *= T(o);
            }
        }
            protected:
        static int64_t serialize(int64_t, const std::optional<internal::bridge::realm>& = std::nullopt);
        static int64_t deserialize(int64_t);

        __cpp_realm_friends
    };

    __cpp_realm_generate_operator(int64_t, >, greater)
    __cpp_realm_generate_operator(int64_t, <, less)
    __cpp_realm_generate_operator(int64_t, <=, less_equal)
    __cpp_realm_generate_operator(int64_t, >=, greater_equal)
    __cpp_realm_generate_operator(int64_t, ==, equal)
    __cpp_realm_generate_operator(int64_t, !=, not_equal)

    namespace {
        static_assert(sizeof(persisted<int64_t>{}.operator=(1)));
        static_assert(sizeof(persisted<int64_t>{} == 1));
        static_assert(sizeof(persisted<int64_t>{}.operator=(1.0)));
    }
}



#endif //REALM_PERSISTED_INT_HPP
