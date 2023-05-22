#ifndef REALM_MANAGED_LIST_HPP
#define REALM_MANAGED_LIST_HPP

#include <cpprealm/notifications.hpp>
#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/experimental/types.hpp>

//#include <vector>

namespace realm::experimental {

    template<typename T>
    struct managed<std::vector<T>, std::enable_if_t<internal::type_info::is_primitive<T>::value>> : managed_base {
        [[nodiscard]] std::vector<T> value() const {
            // unused
            return std::vector<T>();
        }

        realm::notification_token observe(std::function<void(realm::collection_change<T>)>&& fn) {
            auto list = realm::internal::bridge::list(*m_realm, *m_obj, m_key);
            return list.add_notification_callback(
                std::make_shared<collection_callback_wrapper<T>>(
                        std::move(fn),
                        *static_cast<persisted<T>*>(this),
                        false));
        }

        T operator[](size_t idx) const {
            auto list = realm::internal::bridge::list(*m_realm, *m_obj, m_key);
            using U = typename internal::type_info::type_info<T>::internal_type;
            if constexpr (internal::type_info::MixedPersistableConcept<T>::value) {
                return deserialize<T>(realm::internal::bridge::get<U>(list, idx));
            } else {
                return deserialize(realm::internal::bridge::get<U>(list, idx));
            }
        }

        void push_back(const T& value)
        {
            auto list = internal::bridge::list(*m_realm, *m_obj, m_key);
            list.add(value);
        }
    };

    template<typename T>
    struct managed<std::vector<link<T>>> : managed_base {
        [[nodiscard]] std::vector<link<T>> value() const {
            // unused
            abort();
        }

        managed<T> operator[](size_t idx) const {
            auto list = realm::internal::bridge::list(*m_realm, *m_obj, m_key);
            managed<T> m(realm::internal::bridge::get<realm::internal::bridge::obj>(list, idx), *m_realm);
            std::apply([&m](auto &&...ptr) {
                std::apply([&](auto &&...name) {
                    ((m.*ptr).assign(&m.m_obj, &m.m_realm, m.m_obj.get_table().get_column_key(name)), ...);
                }, managed<T>::managed_pointers_names);
            }, managed<T>::managed_pointers());
            return m;
        }
    };
} // namespace realm::experimental

#endif//REALM_MANAGED_LIST_HPP
