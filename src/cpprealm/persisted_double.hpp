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


#ifndef REALM_PERSISTED_DOUBLE_HPP
#define REALM_PERSISTED_DOUBLE_HPP

#include <cpprealm/persisted.hpp>
#include <cpprealm/persisted_custom.hpp>

namespace realm {

    template<>
    struct persisted<double> : persisted_primitive_base<double> {
        using persisted_primitive_base<double>::persisted_primitive_base;

        template<typename T>
        std::enable_if_t<std::is_arithmetic_v<T>, persisted &> // NOLINT(misc-unconventional-assign-operator)
        operator=(const T &o) {
            if (this->is_managed()) {
                this->m_object->get_obj().template set<double>(
                        this->managed,
                        static_cast<double>(o));
            } else {
                new(&this->unmanaged) T(o);
            }
            return *this;
        }

        template<typename T>
        std::enable_if_t<std::is_floating_point_v<T>>
        operator+=(const T &o) {
            if (this->is_managed()) {
                auto old_val = deserialize(this->m_object->get_obj().get<double>(this->managed));
                this->m_object->get_obj().template set<double>(
                        this->managed,
                        static_cast<double>(old_val + o));
            } else {
                this->unmanaged += T(o);
            }
        }
        void operator++() {
            if (this->is_managed()) {
                auto old_val = deserialize(this->m_object->get_obj().get<double>(this->managed));
                this->m_object->get_obj().template set<double>(
                        this->managed,
                        static_cast<double>(++old_val));
            } else {
                ++this->unmanaged;
            }
        }
        template<typename T>
        std::enable_if_t<std::is_floating_point_v<T>>
        operator-=(const T &o) {
            if (this->is_managed()) {
                auto old_val = deserialize(this->m_object->get_obj().get<double>(this->managed));
                this->m_object->get_obj().template set<double>(
                        this->managed,
                        static_cast<double>(old_val - o));
            } else {
                this->unmanaged -= T(o);
            }
        }
        void operator--() {
            if (this->is_managed()) {
                auto old_val = deserialize(this->m_object->get_obj().get<double>(this->managed));
                this->m_object->get_obj().template set<double>(
                        this->managed,
                        static_cast<double>(--old_val));
            } else {
                --this->unmanaged;
            }
        }
        template<typename T>
        std::enable_if_t<std::is_floating_point_v<T>>
        operator*=(const T &o) {
            if (this->is_managed()) {
                auto old_val = deserialize(this->m_object->get_obj().get<double>(this->managed));
                this->m_object->get_obj().template set<int64_t>(
                        this->managed,
                        static_cast<double>(old_val * o));
            } else {
                this->unmanaged *= T(o);
            }
        }

    protected:
        static double serialize(double, const std::optional<internal::bridge::realm>& = std::nullopt);

        static double deserialize(double);

        __cpp_realm_friends
    };

    __cpp_realm_generate_operator(double, >, greater)
    __cpp_realm_generate_operator(double, <, less)
    __cpp_realm_generate_operator(double, <=, less_equal)
    __cpp_realm_generate_operator(double, >=, greater_equal)
    __cpp_realm_generate_operator(double, ==, equal)
    __cpp_realm_generate_operator(double, !=, not_equal)

    namespace {
        static_assert(sizeof(persisted<double>{} == 1));
        static_assert(sizeof(persisted<double>{} == 1.0));
        static_assert(sizeof(persisted<double>{}.operator=(1)));
        static_assert(sizeof(persisted<double>{}.operator=(1.0)));
    }
} // namespace realm

#endif //REALM_PERSISTED_DOUBLE_HPP
