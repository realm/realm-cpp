#ifndef REALM_MANAGED_LIST_HPP
#define REALM_MANAGED_LIST_HPP

#include <cpprealm/notifications.hpp>
#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/experimental/types.hpp>
#include <cpprealm/experimental/observation.hpp>

namespace realm::experimental {

    template<typename T>
    struct managed<std::vector<T>, std::enable_if_t<internal::type_info::is_primitive<T>::value>> : managed_base {
        [[nodiscard]] std::vector<T> value() const {
            // unused
            return std::vector<T>();
        }

        realm::notification_token observe(std::function<void(realm::experimental::collection_change<T>)>&& fn) {
            auto list = std::make_shared<realm::internal::bridge::list>(*m_realm, *m_obj, m_key);
            realm::notification_token token = list->add_notification_callback(
                                                          std::make_shared<realm::experimental::collection_callback_wrapper<T>>(
                                                                  std::move(fn),
                                                                  *static_cast<managed<std::vector<T>>*>(this),
                                                                  false));
            token.m_realm = *m_realm;
            token.m_list = list;
            return token;
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

        void pop_back() {
            internal::bridge::list(*m_realm, *m_obj, m_key).remove(size() - 1);
        }
        void erase(size_t idx) {
            internal::bridge::list(*m_realm, *m_obj, m_key).remove(idx);
        }
        void clear() {
            internal::bridge::list(*m_realm, *m_obj, m_key).remove_all();
        }
        void push_back(const T& value)
        {
            auto list = internal::bridge::list(*m_realm, *m_obj, m_key);
            list.add(value);
        }
        size_t size()
        {
            return internal::bridge::list(*m_realm, *m_obj, m_key).size();
        }
        size_t find(const T& a) {
            return internal::bridge::list(*m_realm, *m_obj, m_key).find(a);
        }
    };

    template<typename T>
    struct managed<std::vector<link<T>>> : managed_base {
        [[nodiscard]] std::vector<link<T>> value() const {
            // unused
            abort();
        }

        void pop_back() {
            internal::bridge::list(*m_realm, *m_obj, m_key).remove(size() - 1);
        }
        void erase(size_t idx) {
            internal::bridge::list(*m_realm, *m_obj, m_key).remove(idx);
        }
        void clear() {
            internal::bridge::list(*m_realm, *m_obj, m_key).remove_all();
        }
        void push_back(const T& value)
        {
            auto list = internal::bridge::list(*m_realm, *m_obj, m_key);
            auto table = m_obj->get_target_table(m_key);
            internal::bridge::obj m_obj;
            if constexpr (managed<T>::schema.HasPrimaryKeyProperty) {
                auto pk = value.*(managed<T>::schema.primary_key().ptr);
                m_obj = table.create_object_with_primary_key(serialize(pk.value));
            } else if (managed<T>::schema.is_embedded_experimental()) {
                m_obj = list.add_embedded();
            } else {
                m_obj = table.create_object();
            }
            std::apply([&m_obj, &value](auto && ...p) {
                (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                         m_obj, m_obj.get_table().get_column_key(p.name),
                         value.*(std::decay_t<decltype(p)>::ptr)), ...);
            }, managed<T, void>::schema.ps);
            if (!managed<T>::schema.is_embedded_experimental()) {
                list.add(m_obj.get_key());
            }
        }
        size_t size()
        {
            return internal::bridge::list(*m_realm, *m_obj, m_key).size();
        }
        size_t find(const managed<T>& a) {
            return internal::bridge::list(*m_realm, *m_obj, m_key).find(a.m_obj.get_key());
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
