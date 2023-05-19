#ifndef CPP_REALM_BRIDGE_LINK_HPP
#define CPP_REALM_BRIDGE_LINK_HPP

#include <cpprealm/internal/type_info.hpp>
#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/experimental/accessors.hpp>

namespace realm {
    namespace experimental {
        template <typename, typename>
        struct managed;

        template <typename T>
        struct link {
            using value_type = T;

            link() {
                new (&unmanaged) T();
            }
            link(const link& v) {
                *this = v;
            }
            link& operator =(const link& v) {
                if (v.is_managed) {
                    is_managed = true;
                    new (&managed) experimental::managed<T>(v.managed);
                } else {
                    new (&unmanaged) T(v.unmanaged);
                }
                return *this;
            }
            link(managed<T>&& v) {
                is_managed = true;
                new (&managed) experimental::managed<T>(std::move(v));
            }
            link(const T& v) {
                is_managed = false;
                new (&unmanaged) T(v);
            }
            ~link() {
                if (is_managed) managed.~managed();
                else unmanaged.~T();
            }

            bool operator ==(const managed<T>& other) const {
                if (is_managed) {
                    return managed.m_obj.operator Obj() == other.m_obj.operator Obj();
                } else {
                    return false;
                }
            }

            bool operator ==(const link<T>& other) const {
                if (other.is_managed && is_managed) {
                    return managed.m_obj.operator Obj() == other.managed.m_obj.operator Obj();
                } else {
                    return false;
                }
            }
            struct box {
                box(T* v) {
                    is_unboxed = false;
                    new (&obj) T*(v);
                }
                box(T&& v) {
                    is_unboxed = true;
                    new (&unboxed) T(v);
                }
                ~box() {
                    if (!is_unboxed) obj = nullptr;
                    else unboxed.~T();
                }
                T* operator ->() && {
                    if (!is_unboxed)
                        return obj;
                    return &unboxed;
                }
                bool is_unboxed;
                union {
                    T *obj = nullptr;
                    T unboxed;
                };
            };

            box operator ->() {
                if (is_managed) {
                    T obj = std::apply([this](auto &&...p) {
                        auto pairs = std::apply([&](auto &&...v) {
                            return std::make_tuple(std::make_pair(p, v)...);
                        }, experimental::managed<T>::managed_pointers);
                        return std::apply([&](auto&& ...pairs) {
                            T obj;
                            auto fn = [&](auto& first, auto& second) {
                                using Result = typename std::decay_t<decltype(first)>::Result;
                                obj.*std::decay_t<decltype(first)>::ptr =
                                        managed.m_obj.template get<Result>((managed.*second).m_key);
                            };
                            (fn(pairs.first, pairs.second), ...);
                            return obj;
                        }, pairs);
                    }, experimental::managed<T>::schema.ps);
                    return box(std::move(obj));
                } else {
                    return box(&unmanaged);
                }
            }
            union {
                T unmanaged;
                managed<T> managed;
            };
            bool is_managed = false;
        };
    }

    namespace experimental {
        template<typename T>
        struct managed<link<T>> : managed_base {
            link<T> value() const { return this->operator->().m_managed; }
            struct ref_type {
                managed<T> m_managed;

                managed<T>* operator ->() {
                    return &m_managed;
                }
            };
            ref_type operator ->() const {
                managed<T> m(m_obj->get_linked_object(m_key), *m_realm);
                std::apply([&m](auto && ...ptr) {
                    std::apply([&](auto&&...name) {
                        ((m.*ptr).assign(&m.m_obj, &m.m_realm, m.m_obj.get_table().get_column_key(name)), ...);
                    }, managed<T>::managed_pointers_names);
                }, managed<T>::managed_pointers);
                return {std::move(m)};
            }

            managed &operator=(T&& o) {
                auto table = m_realm->table_for_object_type(managed<T>::schema.name);
                auto obj = table.create_object();
                m_obj->set(m_key, obj.get_key());

                std::apply([&obj, &o](auto && ...p) {
                    (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                             obj, obj.get_table().get_column_key(p.name), o.*(std::decay_t<decltype(p)>::ptr)
                                     ), ...);
                }, managed<T>::schema.ps);
                auto m = managed<T>(std::move(obj), *m_realm);
                std::apply([&m](auto && ...ptr) {
                    std::apply([&](auto&& ...name) {
                        ((m.*ptr).assign(&m.m_obj, &m.m_realm, m.m_obj.get_table().get_column_key(name)), ...);
                    }, managed<T>::managed_pointers_names);
                }, managed<T>::managed_pointers);
                return *this;
            }
        };
    }
}
#endif //CPP_REALM_BRIDGE_LINK_HPP
