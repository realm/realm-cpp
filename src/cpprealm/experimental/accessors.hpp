#ifndef CPP_REALM_ACCESSORS_HPP
#define CPP_REALM_ACCESSORS_HPP

#include <cpprealm/experimental/types.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/table.hpp>

namespace realm::experimental {
    template<typename>
    struct link;
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
    struct accessor<std::vector<link<T>>> {
        static inline void set(internal::bridge::obj& obj,
                               internal::bridge::col_key&& key,
                               const std::vector<link<T>>& value) {
            std::vector<internal::bridge::obj_key> keys;
            for (const link<T>& lnk : value) {
                if (lnk.is_managed) {
                    keys.push_back(lnk.managed.m_obj.get_key());
                } else {
                    auto table = obj.get_target_table(key);
                    auto m_obj = table.create_object();
                    std::apply([&m_obj, &lnk](auto && ...p) {
                        (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                                 m_obj, m_obj.get_table().get_column_key(p.name),
                                 lnk.unmanaged.*(std::decay_t<decltype(p)>::ptr)), ...);
                    }, managed<T, void>::schema.ps);
                    keys.push_back(m_obj.get_key());
                }
            }
            obj.set_list_values(key, keys);
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
    struct accessor<std::map<std::string, std::optional<link<T>>>> {
        static void set(internal::bridge::obj& obj,
                        internal::bridge::col_key&& key,
                        const std::map<std::string, std::optional<link<T>>>& value) {
            auto d = obj.get_dictionary(key);
            for (auto& [k, v] : value) {
                if (v) {
                    if (v->is_managed) {
                        abort();
                        //d.insert(k, v.managed.m_obj);
                    } else {
                        auto m_obj = d.create_and_insert_linked_object(k);
                        std::apply([&m_obj, o = *v](auto && ...p) {
                            (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                                     m_obj, m_obj.get_table().get_column_key(p.name),
                                     o.unmanaged.*(std::decay_t<decltype(p)>::ptr)), ...);
                        }, managed<T, void>::schema.ps);
                    }
                }

            }
        }
    };
    // MARK: - accessor link
    template <typename T>
    struct accessor<link<T>> {
        static inline void set(internal::bridge::obj& obj,
                               internal::bridge::col_key&& key,
                               const link<T>& value) {
            if (value.is_managed) {
                obj.set(key, value.managed.m_obj.get_key());
            } else {
                auto table = obj.get_target_table(key);
                auto m_obj = table.create_object();
                obj.set(key, m_obj.get_key());
                std::apply([&m_obj, &value](auto && ...p) {
                    (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                             m_obj, m_obj.get_table().get_column_key(p.name),
                             value.unmanaged.*(std::decay_t<decltype(p)>::ptr)), ...);
                }, managed<T, void>::schema.ps);
            }
        }
    };
} // realm::experimental
#endif//CPP_REALM_ACCESSORS_HPP

