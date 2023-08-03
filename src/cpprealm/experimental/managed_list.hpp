#ifndef CPPREALM_MANAGED_LIST_HPP
#define CPPREALM_MANAGED_LIST_HPP

#include <cpprealm/notifications.hpp>
#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/experimental/types.hpp>
#include <cpprealm/experimental/observation.hpp>

namespace realm::experimental {

    template<typename T>
    struct managed<std::vector<T>, std::enable_if_t<internal::type_info::is_primitive<T>::value>> : managed_base {
        using managed<std::vector<T>>::managed_base::operator=;

        class iterator {
        public:
            using iterator_category = std::input_iterator_tag;

            bool operator!=(const iterator& other) const
            {
                return !(*this == other);
            }

            bool operator==(const iterator& other) const
            {
                return (m_parent == other.m_parent) && (m_i == other.m_i);
            }

            T operator*() const noexcept
            {
                return (*m_parent)[m_i];
            }

            iterator& operator++()
            {
                this->m_i++;
                return *this;
            }

            const iterator& operator++(int i)
            {
                this->m_i += i;
                return *this;
            }
        private:
            template<typename, typename>
            friend struct managed;

            iterator(size_t i, managed<std::vector<T>>* parent)
                : m_i(i), m_parent(parent)
            {
            }
            size_t m_i;
            managed<std::vector<T>>* m_parent;
        };
        iterator begin()
        {
            return iterator(0, this);
        }

        iterator end()
        {
            return iterator(size(), this);
        }
        [[nodiscard]] std::vector<T> value() const {
            auto list = realm::internal::bridge::list(*m_realm, *m_obj, m_key);
            using U = typename internal::type_info::type_info<T>::internal_type;

            size_t count = list.size();
            if (count == 0)
                return std::vector<T>();

            auto ret = std::vector<T>();
            ret.reserve(count);
            for(size_t i = 0; i < count; i++) {
                if constexpr (internal::type_info::MixedPersistableConcept<T>::value) {
                    ret.push_back(deserialize<T>(realm::internal::bridge::get<U>(list, i)));
                } else {
                    ret.push_back(deserialize(realm::internal::bridge::get<U>(list, i)));
                }
            }

            return ret;
        }

        realm::notification_token observe(std::function<void(realm::experimental::collection_change)>&& fn) {
            auto list = std::make_shared<realm::internal::bridge::list>(*m_realm, *m_obj, m_key);
            realm::notification_token token = list->add_notification_callback(
                                                          std::make_shared<realm::experimental::collection_callback_wrapper>(
                                                                  std::move(fn),
                                                                  false));
            token.m_realm = *m_realm;
            token.m_list = list;
            return token;
        }

        // TODO: emulate a reference to the value.
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
        void set(size_t pos, const T& a) {
            internal::bridge::list(*m_realm, *m_obj, m_key).set(pos, a);
        }
    };

    template<typename T>
    struct managed<std::vector<T*>> : managed_base {
        [[nodiscard]] std::vector<T*> value() const {
            throw std::runtime_error("value() is not available on collections of managed objects. Access each object via subscript instead.");
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
        void push_back(T* value)
        {
            auto list = internal::bridge::list(*m_realm, *m_obj, m_key);
            auto table = m_obj->get_target_table(m_key);
            internal::bridge::obj m_obj;
            if constexpr (managed<T>::schema.HasPrimaryKeyProperty) {
                auto pk = (*value).*(managed<T>::schema.primary_key().ptr);
                m_obj = table.create_object_with_primary_key(realm::internal::bridge::mixed(serialize(pk.value)));
            } else if (managed<T>::schema.is_embedded_experimental()) {
                m_obj = list.add_embedded();
            } else {
                m_obj = table.create_object();
            }
            std::apply([&m_obj, &value](auto && ...p) {
                (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                         m_obj, m_obj.get_table().get_column_key(p.name),
                         (*value).*(std::decay_t<decltype(p)>::ptr)), ...);
            }, managed<T, void>::schema.ps);
            if (!managed<T>::schema.is_embedded_experimental()) {
                list.add(m_obj.get_key());
            }
        }
        void push_back(const managed<T>& value)
        {
            auto list = internal::bridge::list(*m_realm, *m_obj, m_key);
            if (!managed<T>::schema.is_embedded_experimental()) {
                list.add(value.m_obj.get_key());
            } else {
                throw std::logic_error("Cannot add existing embedded object to managed list.");
            }
        }
        void push_back(const managed<T*>& value)
        {
            if (!managed<T>::schema.is_embedded_experimental()) {
                auto list = internal::bridge::list(*m_realm, *m_obj, m_key);
                list.add(value.m_obj->get_key());
            } else {
                throw std::logic_error("Cannot add existing embedded object to managed list.");
            }
        }

        size_t size()
        {
            return internal::bridge::list(*m_realm, *m_obj, m_key).size();
        }
        size_t find(const managed<T>& a) {
            return internal::bridge::list(*m_realm, *m_obj, m_key).find(a.m_obj.get_key());
        }
        size_t find(const typename managed<T*>::ref_type& a) const {
            return internal::bridge::list(*m_realm, *m_obj, m_key).find(a->m_obj.get_key());
        }
        typename managed<T*>::ref_type operator[](size_t idx) const {
            auto list = realm::internal::bridge::list(*m_realm, *m_obj, m_key);
            managed<T> m(realm::internal::bridge::get<realm::internal::bridge::obj>(list, idx), *m_realm);
            std::apply([&m](auto &&...ptr) {
                std::apply([&](auto &&...name) {
                    ((m.*ptr).assign(&m.m_obj, &m.m_realm, m.m_obj.get_table().get_column_key(name)), ...);
                }, managed<T>::managed_pointers_names);
            }, managed<T>::managed_pointers());
            return {std::move(m)};
        }

        realm::notification_token observe(std::function<void(realm::experimental::collection_change)>&& fn) {
            auto list = std::make_shared<realm::internal::bridge::list>(*m_realm, *m_obj, m_key);
            realm::notification_token token = list->add_notification_callback(
                    std::make_shared<realm::experimental::collection_callback_wrapper>(
                            std::move(fn),
                            false));
            token.m_realm = *m_realm;
            token.m_list = list;
            return token;
        }
    };
} // namespace realm::experimental

#endif//CPPREALM_MANAGED_LIST_HPP
