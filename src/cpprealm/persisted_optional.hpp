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

#ifndef REALM_PERSISTED_OPTIONAL_HPP
#define REALM_PERSISTED_OPTIONAL_HPP

#include "cpprealm/internal/bridge/obj.hpp"
#include <cpprealm/persisted.hpp>

namespace realm {
    template <typename T>
    struct persisted<std::optional<T>, std::enable_if_t<internal::type_info::is_primitive<T>::value>> :
            public persisted_base<std::optional<T>> {
        persisted() {
            new (&unmanaged) T();
        }
        persisted(const persisted& v) {
            *this = v;
        }
        persisted(persisted&& v) {
            *this = std::move(v);
        }
        persisted(const T& value) {
            new (&unmanaged) std::optional<T>(value);
        }
        persisted(T&& value) {
            new (&unmanaged) std::optional<T>(std::move(value));
        }
        ~persisted() {
            if (this->is_managed()) {
                managed.~col_key();
            } else {
                unmanaged.~optional<T>();
            }
        }
        persisted& operator=(const persisted& v) {
            if (v.is_managed()) {
                this->m_object = v.m_object;
                new (&managed) internal::bridge::col_key(v.managed);
            } else {
                new (&unmanaged) std::optional<T>(v.unmanaged);
            }
            return *this;
        }
        persisted& operator=(persisted&& v) {
            if (v.is_managed()) {
                this->m_object = v.m_object;
                new (&managed) internal::bridge::col_key(std::move(v.managed));
            } else {
                new (&unmanaged) std::optional<T>(std::move(v.unmanaged));
            }
            return *this;
        }
        persisted& operator=(const char* o) {
            static_assert(std::is_same_v<std::string, T>);
            *this = std::move(std::optional<T>(o));
            return *this;
        }
        persisted& operator=(T&& o) {
            *this = std::move(std::optional<T>(o));
            return *this;
        }
        persisted& operator=(const T& o) {
            *this = std::optional<T>(o);
            return *this;
        }
        persisted& operator=(std::optional<T>&& o) {
            if (auto& obj = this->m_object) {
                if (o) {
                    obj->obj().set(this->managed,
                                   persisted<T>::serialize(*o));
                } else {
                    obj->obj().set_null(this->managed);
                }
            } else {
                if (o)
                    new (&this->unmanaged) std::optional<T>(std::move(*o));
                else
                    new (&this->unmanaged) std::optional<T>();
            }
            return *this;
        }

        persisted& operator=(const std::optional<T>& o) {
            if (auto& obj = this->m_object) {
                if (o) {
                    obj->obj().set(this->managed,
                                   internal::type_info::serialize(*o));
                } else {
                    obj->obj().set_null(this->managed);
                }
            } else {
                new (&this->unmanaged) T(o);
            }
            return *this;
        }

        operator bool() const {
            if (this->is_managed()) {
                return !this->m_object->obj().is_null(managed);
            } else {
                return unmanaged.operator bool();
            }
        }

        T operator*() const {
            if (this->is_managed()) {
                return *this->m_object->obj().template get<std::optional<T>>(managed);
            } else {
                return *unmanaged;
            }
        }
    protected:
        union {
            std::optional<T> unmanaged;
            internal::bridge::col_key managed;
        };

        void manage(internal::bridge::object* object,
                    internal::bridge::col_key&& col_key) override {
            if (this->unmanaged) {
                object->obj().set(col_key, persisted<T>::serialize(*unmanaged));
            } else {
                object->obj().set_null(col_key);
            }
            unmanaged.~optional<T>();
            assign_accessor(object, std::move(col_key));
        }
        void assign_accessor(internal::bridge::object* object,
                             internal::bridge::col_key&& col_key) override {
            this->m_object = object;
            new (&managed) internal::bridge::col_key(std::move(col_key));
        }

        inline static std::optional<typename internal::type_info::type_info<T>::internal_type>
        serialize(const std::optional<T>& value) {
            if (!value) {
                return std::nullopt;
            }
            return persisted<T>::serialize(*value);
        }
        static std::optional<T> deserialize(const std::optional<typename internal::type_info::type_info<T>::internal_type>& value) {
            if (!value) {
                return std::nullopt;
            }
            return persisted<T>::deserialize(*value);
        }

        __cpp_realm_friends
    };

    template <typename T>
    rbool inline operator==(const persisted<std::optional<T>>& a, const std::optional<T>& b)
    {
        if (a.should_detect_usage_for_queries) {
            auto query = internal::bridge::query(a.query->get_table());
            if (b) {
                query.equal(a.managed, internal::type_info::serialize(*b));
            } else {
                query.equal(a.managed, realm::null{});
            }
            return query;
        }

        if (a && b) {
            return *a == *b;
        } else if (!a && !b) {
            return true;
        } else {
            return false;
        }
    }

    template <typename T>
    rbool inline operator==(const persisted<std::optional<T>>& a, const char* b)
    {
        if (a.should_detect_usage_for_queries) {
            auto query = internal::bridge::query(a.query->get_table());
            if (b) {
                query.equal(a.managed, b);
            } else {
                query.equal(a.managed, std::nullopt);
            }
            return query;
        }

        if (a) {
            return *a == b;
        } else {
            return false;
        }
    }

    template <typename T, typename V>
    rbool inline operator==(const persisted<std::optional<T>>& a, const V& b)
    {
        if (a.should_detect_usage_for_queries) {
            auto query = internal::bridge::query(a.query->get_table());
//            if (b) {
                query.equal(a.managed, persisted<V>::serialize(b));
//            } else {
//                query.equal(a.managed, std::nullopt);
//            }
            return query;
        }

        if (a) {
            return *a == b;
        } else {
            return false;
        }
    }

    namespace {
        static_assert(sizeof(persisted<std::optional<int64_t>>{}));
        static_assert(sizeof(persisted<std::optional<std::string>>{}));
    }
}

#endif //REALM_PERSISTED_OPTIONAL_HPP
