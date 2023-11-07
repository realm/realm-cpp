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

#ifndef realm_persisted_hpp
#define realm_persisted_hpp

#include <cpprealm/internal/type_info.hpp>
#include <cpprealm/internal/bridge/object.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/rbool.hpp>

#include <optional>

namespace realm::experimental {
    template<typename>
    struct box_base;
}

#define __cpp_realm_generate_operator(type, op, name) \
    inline rbool operator op(const persisted<type>& a, const type& b) { \
        if (a.should_detect_usage_for_queries) { \
            auto query = internal::bridge::query(a.query->get_table()); \
            auto serialized = persisted<type>::serialize(b);     \
            query.name(a.managed, serialized);    \
            return query; \
        } \
        return *a op b; \
    } \
    inline rbool operator op(const persisted<type>& a, const persisted<type>& b) { \
        return a op *b; \
    } \

namespace realm {

struct FieldValue;
template <typename T, typename = void>
struct persisted;

template <typename T>
struct persisted_base {
    static_assert(std::is_destructible_v<T>);

    using Result = T;
    persisted_base() = default;
    persisted_base(const persisted_base& o) {
        *this = o;
    }
    persisted_base(persisted_base&& o) {
        *this = std::move(o);
    }
protected:
    [[nodiscard]] bool is_managed() const {
        return m_object || query;
    }

    virtual void manage(internal::bridge::object* object,
                        internal::bridge::col_key&& col_key) = 0;
    virtual void assign_accessor(internal::bridge::object* object,
                                 internal::bridge::col_key&& col_key) = 0;

    std::optional<internal::bridge::object> m_object;

    // MARK: Queries
    bool should_detect_usage_for_queries = false;
    internal::bridge::query* query = nullptr;
    bool is_and;
    bool is_or;

    void prepare_for_query(internal::bridge::query& query_builder) {
        should_detect_usage_for_queries = true;
        query = &query_builder;
    }

    __cpp_realm_friends
};

    template <typename T>
    struct persisted_primitive_base : public persisted_base<T> {
        using persisted_base<T>::persisted_base;
        persisted_primitive_base() {
            new (&unmanaged) T();
        }
        persisted_primitive_base(const persisted_primitive_base& v) {
            if (v.is_managed()) {
                this->m_object = v.m_object;
                new (&managed) internal::bridge::col_key(v.managed);
            } else {
                new (&unmanaged) T(v.unmanaged);
            }
        }
        persisted_primitive_base(const T& value) {
            new (&unmanaged) T(value);
        }
        persisted_primitive_base(T&& value) {
            new (&unmanaged) T(std::move(value));
        }
        ~persisted_primitive_base() {
            if (this->is_managed()) {
                managed.~col_key();
            } else {
                unmanaged.~T();
            }
        }

        persisted_primitive_base& operator=(const T& o) noexcept {
            if (this->is_managed()) {
                this->m_object->get_obj().template set(managed, persisted<T>::serialize(o));
            } else {
                new (&unmanaged) T(o);
            }
            return *this;
        }

        persisted_primitive_base& operator=(T&& o) noexcept {
            if (this->is_managed()) {
                this->m_object->get_obj().set(managed, experimental::serialize(o, this->m_object->get_realm()));
            } else {
                new (&unmanaged) T(std::move(o));
            }
            return *this;
        }

        persisted_primitive_base& operator=(const persisted_primitive_base& o) noexcept {
            if (o.m_object) {
                this->m_object = o.m_object;
                new (&managed) internal::bridge::col_key(o.managed);
            } else {
                if (this->is_managed()) {
                    this->m_object->get_obj().set(managed, o.unmanaged);
                } else {
                    new (&unmanaged) T(o.unmanaged);
                }
            }
            return *this;
        }


        persisted_primitive_base& operator=(persisted_primitive_base&& o) noexcept {
            if (o.m_object) {
                this->m_object = o.m_object;
                new (&managed) internal::bridge::col_key(std::move(o.managed));
            } else {
                if (this->is_managed()) {
                    this->m_object->get_obj().set(managed, std::move(o.unmanaged));
                } else {
                    new (&unmanaged) T(std::move(o.unmanaged));
                }
            }
            return *this;
        }

        T operator *() const {
            if (this->is_managed()) {
                return persisted<T>::deserialize(this->m_object->get_obj()
                        .template get<typename internal::type_info::type_info<T>::internal_type>(this->managed));
            } else {
                return this->unmanaged;
            }
        }
    protected:
        union {
            T unmanaged;
            internal::bridge::col_key managed;
        };

        void manage(internal::bridge::object* object,
                    internal::bridge::col_key&& col_key) final {
            object->get_obj().set(col_key, persisted<T>::serialize(unmanaged, object->get_realm()));
            assign_accessor(object, std::move(col_key));
        }
        void assign_accessor(internal::bridge::object* object,
                             internal::bridge::col_key&& col_key) final {
            this->m_object = *object;
            new (&this->managed) internal::bridge::col_key(col_key);
        }
    };

template <typename T>
inline std::ostream& operator<< (std::ostream& stream, const persisted_primitive_base<T>& value)
{
    return stream << value;
}

template <typename T>
inline typename std::enable_if<std::is_base_of<realm::object_base<T>, T>::value, std::ostream>::type&
operator<< (std::ostream& stream, const T& object)
{
    std::apply([&stream, &object](auto&&... props) {
        stream << std::string("{\n");
        ((stream << std::string("\t") << props.name << ": " << *(object.*props.ptr) << "\n"), ...);
        stream << std::string("}");
    }, T::schema.ps);

    return stream;
}

}

#endif /* realm_persisted_hpp */
