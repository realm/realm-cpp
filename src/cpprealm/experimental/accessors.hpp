#ifndef CPP_REALM_ACCESSORS_HPP
#define CPP_REALM_ACCESSORS_HPP

#include <cpprealm/experimental/types.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/lnklst.hpp>

namespace realm::experimental {
    template<typename>
    struct primary_key;
    template<typename, typename>
    struct managed;

    template <typename T, typename = void>
    struct accessor {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const T& value);
    };

    template <>
    struct accessor<int64_t> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const int64_t& value) {
            obj.set(key, value);
        }
    };
    template <>
    struct accessor<std::optional<int64_t>> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const std::optional<int64_t>& value) {
            if (value) {
                obj.set(key, *value);
            } else {
                obj.set_null(key);
            }
        }
    };
    template <>
    struct accessor<double> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const double& value) {
            obj.set(key, value);
        }
    };
    template <>
    struct accessor<std::optional<double>> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const std::optional<double>& value) {
            if (value) {
                obj.set(key, *value);
            } else {
                obj.set_null(key);
            }
        }
    };
    template <>
    struct accessor<bool> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const bool& value) {
            obj.set(key, value);
        }
    };
    template <>
    struct accessor<std::optional<bool>> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const std::optional<bool>& value) {
            if (value) {
                obj.set(key, *value);
            } else {
                obj.set_null(key);
            }
        }
    };


    template <typename T>
    struct accessor<T, std::enable_if_t<std::is_enum_v<T>>> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const T& value) {
            obj.set(key, static_cast<int64_t>(value));
        }
    };
    template <typename T>
    struct accessor<T, std::enable_if_t<std::is_enum_v<typename T::value_type>>> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const T& value) {
            if (value) {
                obj.set(key, static_cast<int64_t>(*value));
            } else {
                obj.set_null(key);
            }
        }
    };

    template <typename T>
    struct accessor<T, std::enable_if_t<internal::type_info::MixedPersistableConcept<T>::value>> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const T& value) {
            obj.set(key, serialize(value));
        }
    };


    template <>
    struct accessor<uuid> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const uuid& value) {
            obj.set(key, value);
        }
    };
    template <>
    struct accessor<std::optional<uuid>> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const std::optional<uuid>& value) {
            if (value) {
                obj.set(key, *value);
            } else {
                obj.set_null(key);
            }
        }
    };
    template <>
    struct accessor<object_id> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const object_id& value) {
            obj.set(key, value);
        }
    };
    template <>
    struct accessor<std::optional<object_id>> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const std::optional<object_id>& value) {
            if (value) {
                obj.set(key, *value);
            } else {
                obj.set_null(key);
            }
        }
    };

    template <>
    struct accessor<std::chrono::time_point<std::chrono::system_clock>> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const std::chrono::time_point<std::chrono::system_clock>& value) {
            obj.set(key, value);
        }
    };
    template <>
    struct accessor<std::optional<std::chrono::time_point<std::chrono::system_clock>>> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const std::optional<std::chrono::time_point<std::chrono::system_clock>>& value) {
            if (value) {
                obj.set(key, *value);
            } else {
                obj.set_null(key);
            }
        }
    };

    template <>
    struct accessor<std::string> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const std::string& value) {
            obj.set(key, value);
        }
    };
    template <>
    struct accessor<std::optional<std::string>> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const std::optional<std::string>& value) {
            if (value) {
                obj.set(key, *value);
            } else {
                obj.set_null(key);
            }
        }
    };

    template <>
    struct accessor<std::vector<uint8_t>> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const std::vector<uint8_t>& value) {
            obj.set(key, value);
        }
    };
    template <>
    struct accessor<std::optional<std::vector<uint8_t>>> {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const std::optional<std::vector<uint8_t>>& value) {
            if (value) {
                obj.set(key, *value);
            } else {
                obj.set_null(key);
            }
        }
    };
    template <typename T>
    struct accessor<std::vector<T>> {
        static inline void set(internal::bridge::obj& obj,
                               internal::bridge::col_key&& key,
                               const std::vector<T>& value) {
            obj.set_list_values(key, value);
        }
    };
    template <typename T>
    struct accessor<std::vector<T*>> {
        static inline void set(internal::bridge::obj& obj,
                               internal::bridge::col_key&& key,
                               const std::vector<T*>& value) {
            auto list = obj.get_linklist(key);
            for (size_t i = 0; i < value.size(); i++) {
                auto& lnk = value[i];
                if (!lnk) {
                    continue;
                }
                auto table = obj.get_target_table(key);
                internal::bridge::obj m_obj;
                if constexpr (managed<T>::schema.HasPrimaryKeyProperty) {
                    auto pk = (*lnk).*(managed<T>::schema.primary_key().ptr);
                    m_obj = table.create_object_with_primary_key(serialize(pk.value));
                } else if (managed<T>::schema.is_embedded_experimental()) {
                    m_obj = list.create_and_insert_linked_object(i);
                } else {
                    m_obj = table.create_object();
                }
                std::apply([&m_obj, &lnk](auto && ...p) {
                    (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                             m_obj, m_obj.get_table().get_column_key(p.name),
                             (*lnk).*(std::decay_t<decltype(p)>::ptr)), ...);
                }, managed<T, void>::schema.ps);
                if (!managed<T>::schema.is_embedded_experimental()) {
                    list.add(m_obj.get_key());
                }
            }
        }
    };
    template <typename T>
    struct accessor<std::map<std::string, T>> {
        static void set(internal::bridge::obj& obj,
                        internal::bridge::col_key&& key,
                        const std::map<std::string, T>& value) {
            auto d = obj.get_dictionary(key);
            for (auto& [k, v] : value) {
                if constexpr (internal::type_info::MixedPersistableConcept<T>::value) {
                    d.insert(k, std::visit([](auto&& arg) {
                                 using M = typename internal::type_info::type_info<std::decay_t<decltype(arg)>>::internal_type;
                                 return internal::bridge::mixed(M(arg));
                             }, v));
                } else {
                    if constexpr (internal::type_info::is_optional<T>::value) {
                        if constexpr (std::is_enum_v<typename T::value_type>) {
                            using U = typename internal::type_info::type_info<T, void>::internal_type;
                            if (v) {
                                d.insert(k, static_cast<typename std::underlying_type<typename T::value_type>::type>(*v));
                            } else {
                                d.insert(k, internal::bridge::mixed());
                            }
                        } else {
                            using U = typename internal::type_info::type_info<T, void>::internal_type;
                            d.insert(k, U(v));
                        }
                    } else {
                        using U = typename internal::type_info::type_info<T, void>::internal_type;
                        d.insert(k, U(v));
                    }
                }
            }
        }
    };
    template <typename T>
    struct accessor<std::map<std::string, T*>> {
        static void set(internal::bridge::obj& obj,
                        internal::bridge::col_key&& key,
                        const std::map<std::string, T*>& value) {
            auto d = obj.get_dictionary(key);
            for (auto& [k, v] : value) {
                if (v) {
//                    if (v->is_managed) {
//                    d.insert(k, v->m_obj.get_key());
//                    } if (!v->unmanaged) {
//                        d.insert(k, internal::bridge::mixed());
//                    } else {
                    internal::bridge::obj m_obj;
                    if constexpr (managed<T>::schema.HasPrimaryKeyProperty) {
                        auto pk = (*v).*(managed<T>::schema.primary_key().ptr);
                        m_obj = d.create_and_insert_linked_object(k, pk.value);
                    } else {
                        m_obj = d.create_and_insert_linked_object(k);
                    }
                    std::apply([&m_obj, o = *v](auto && ...p) {
                        (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                                 m_obj, m_obj.get_table().get_column_key(p.name),
                                 o.*(std::decay_t<decltype(p)>::ptr)), ...);
                    }, managed<T, void>::schema.ps);
                    d.insert(k, m_obj.get_key());
//                    }
                }
            }
        }
    };
    // MARK: - accessor link

    template <typename T>
    struct accessor<T*> {
        static inline void set(internal::bridge::obj& obj,
                               internal::bridge::col_key&& key,
                               T* value) {
            if (!value) {
                return;
            }
            auto table = obj.get_target_table(key);
            internal::bridge::obj m_obj;
            if constexpr (managed<T>::schema.HasPrimaryKeyProperty) {
                auto pk = (*value).*(managed<T>::schema.primary_key().ptr);
                m_obj = table.create_object_with_primary_key(pk.value);
                obj.set(key, m_obj.get_key());
            } else if (managed<T>::schema.is_embedded_experimental()) {
                m_obj = obj.create_and_set_linked_object(key);
            } else {
                m_obj = table.create_object();
                obj.set(key, m_obj.get_key());
            }
            std::apply([&m_obj, &value](auto && ...p) {
                (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                         m_obj, m_obj.get_table().get_column_key(p.name),
                         (*value).*(std::decay_t<decltype(p)>::ptr)), ...);
            }, managed<T, void>::schema.ps);
        }
    };

    template <auto T>
    struct accessor<linking_objects<T>> {
        static inline void set(internal::bridge::obj& obj,
                               internal::bridge::col_key&& key,
                               linking_objects<T> value) {
        }
    };
    // MARK: - accessor primary key
    template <typename T>
    struct accessor<primary_key<T>> {
        static inline void set(internal::bridge::obj& obj,
                               internal::bridge::col_key&& key,
                               const primary_key<T>& value) {
            if constexpr (std::is_enum_v<T>) {
                obj.set(key, static_cast<int64_t>(value.value));
            } else {
                obj.set(key, value.value);
            }
        }
    };
} // realm::experimental
#endif//CPP_REALM_ACCESSORS_HPP

