#ifndef CPP_REALM_EXPERIMENTAL_DB_HPP
#define CPP_REALM_EXPERIMENTAL_DB_HPP

#include <string>
#include <optional>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/schema.hpp>
#include <cpprealm/db.hpp>
#include "link.hpp"
#include <realm/object-store/property.hpp>
#include <cpprealm/persisted_uuid.hpp>
#include <utility>
#include <cpprealm/experimental/macros.hpp>

namespace realm::experimental {
    template<>
    struct managed<int> : managed_base {
        [[nodiscard]] int value() const {
            return m_obj->template get<int64_t>(m_key);
        }
    };
    template<>
    struct managed<uuid> : managed_base {
        [[nodiscard]] uuid value() const {
            return m_obj->template get<uuid>(m_key);
        }
    };
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
        ~link() {
            if (is_managed) managed.~managed();
            else unmanaged.~T();
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
    };

    template <typename T, typename = void>
    struct accessor {
        static inline void set(internal::bridge::obj& obj,
                               const internal::bridge::col_key& key,
                               const T& value) {
            obj.set(key, value);
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
                    }, managed<T>::schema.ps);
                    keys.push_back(m_obj.get_key());
                }
            }
            obj.set_list_values(key, keys);
        }
    };
    template <>
    struct accessor<std::string> {
        static inline void set(internal::bridge::obj& obj,
                               internal::bridge::col_key&& key,
                               const std::string& value) {
            obj.set(key, value);
        }
    };
    template <>
    struct accessor<int> {
        static inline void set(internal::bridge::obj& obj,
                               internal::bridge::col_key&& key,
                               int64_t value) {
            obj.set(key, value);
        }
    };
    template <typename T>
    struct accessor<std::map<std::string, T>> {
        static inline void set(internal::bridge::obj& obj,
                               internal::bridge::col_key&& key,
                               const std::map<std::string, T>& value) {
            internal::bridge::dict d = obj.get_dictionary(key);
            for (auto& [k, v] : value) {
                if constexpr (internal::type_info::MixedPersistableConcept<T>::value) {
                    d.insert(k, std::visit([](auto&& arg) {
                        using M = typename internal::type_info::type_info<std::decay_t<decltype(arg)>>::internal_type;
                        return internal::bridge::mixed(M(arg));
                    }, v));
                } else {
                    d.insert(k, v);
                }
            }
        }
    };
    template <typename T>
    struct accessor<std::map<std::string, link<T>>> {
        static inline void set(internal::bridge::obj& obj,
                               internal::bridge::col_key&& key,
                               const std::map<std::string, link<T>>& value) {
            internal::bridge::dict d = obj.get_dictionary(key);
            for (auto& [k, v] : value) {
                if (v.is_managed) {
                    d.insert(k, v.managed.m_obj);
                } else {
                    auto table = obj.get_target_table(key);
                    auto m_obj = table.create_object();
                    auto& lnk = v;
                    std::apply([&m_obj, &lnk](auto && ...p) {
                        (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                                m_obj, m_obj.get_table().get_column_key(p.name),
                                lnk.unmanaged.*(std::decay_t<decltype(p)>::ptr)), ...);
                    }, managed<T>::schema.ps);
                    d.insert(k, m_obj);
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
                }, managed<T>::schema.ps);
            }
        }
    };


    struct db {
        internal::bridge::realm m_realm;
        explicit db(realm::db_config config)
        : m_realm(config)
        {
        }

        void begin_write() const { m_realm.begin_transaction(); }
        void commit_write() const { m_realm.commit_transaction(); }

        template <typename Fn>
        std::invoke_result_t<Fn> write(Fn&& fn) const {
            begin_write();
            if constexpr (!std::is_void_v<std::invoke_result_t<Fn>>) {
                auto val = fn();
                commit_write();
                return val;
            } else {
                fn();
                commit_write();
            }
        }
        template <typename T>
        managed<T> add(T &&v) {
            static_assert(sizeof(managed<T>), "Must declare schema for T");
            auto table = m_realm.table_for_object_type(managed<T>::schema.name);
            auto m_obj = table.create_object();
            std::apply([&m_obj, &v](auto && ...p) {
                (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                        m_obj, m_obj.get_table().get_column_key(p.name), v.*(std::decay_t<decltype(p)>::ptr)
                ), ...);
            }, managed<T>::schema.ps);
            auto m = managed<T>(std::move(m_obj), m_realm);
            std::apply([&m](auto && ...ptr) {
                std::apply([&](auto&& ...name) {
                    ((m.*ptr).assign(&m.m_obj, &m.m_realm, m.m_obj.get_table().get_column_key(name)), ...);
                }, managed<T>::managed_pointers_names);
            }, managed<T>::managed_pointers);
            return m;
        }
        template <typename T>
        void insert(const std::vector<T> &v) {
            static_assert(sizeof(managed<T>), "Must declare schema for T");
            internal::bridge::table table = m_realm.table_for_object_type(managed<T>::schema.name);
            for (auto& obj : v) {
                auto m_obj = table.create_object();
                std::apply([&m_obj, &obj](auto && ...p) {
                    (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                            m_obj, m_obj.get_table().get_column_key(p.name), obj.*(std::decay_t<decltype(p)>::ptr)
                    ), ...);
                }, managed<T>::schema.ps);
            }
        }
    };

    template <typename ...Ts>
    db open(const std::string& path, const std::shared_ptr<scheduler>& scheduler = scheduler::make_default()) {
        db_config config;
        config.set_path(path);
        config.set_scheduler(scheduler);
        std::vector<internal::bridge::object_schema> schema;
        (schema.push_back(managed<Ts>::schema.to_core_schema()), ...);
        config.set_schema(schema);
        return db(config);
    }

    struct dump_db {
        realm::DBRef m_db;
        explicit dump_db(std::string&& path)
                : m_db(DB::create(std::move(path)))
        {
        }

        template <typename T>
        static TableRef add_table(const TransactionRef& transaction_ref) {
            auto tbl = transaction_ref->add_table(internal::bridge::table_name_for_object_type(managed<T>::schema.name));
            std::apply([&transaction_ref, &tbl](auto&& ...p) {
                (add_property_to_table(transaction_ref, tbl, p), ...);
            }, managed<T>::schema.ps);
            return tbl;
        }

        static constexpr DataType property_type_to_data_type(internal::bridge::property::type type) {
            if (is_mixed(static_cast<PropertyType>(type))) {
                return type_Mixed;
            }
            switch (static_cast<PropertyType>(type) & ~PropertyType::Flags) {
                case PropertyType::Int: return type_Int;
                case PropertyType::Bool: return type_Bool;
                case PropertyType::String: return type_String;
                case PropertyType::Data: return type_Binary;
                case PropertyType::Date: return type_Timestamp;
                case PropertyType::Float: return type_Float;
                case PropertyType::Double: return type_Double;
                case PropertyType::Object:
                case PropertyType::LinkingObjects: return type_Link;
                case PropertyType::ObjectId: return type_ObjectId;
                case PropertyType::Decimal: return type_Decimal;
                case PropertyType::UUID: return type_UUID;
                default: abort();
            }
        }
        template <auto Ptr, bool IsPrimaryKey>
        static void add_property_to_table(TransactionRef transaction_ref,
                                          TableRef tbl,
                                          const realm::schemagen::property<Ptr, IsPrimaryKey>& v) {
            ColKey col_key;
            if constexpr (internal::type_info::is_map<typename std::decay_t<decltype(v)>::Result>::value) {
                if constexpr (internal::type_info::is_link<typename std::decay_t<decltype(v)>::Result::mapped_type>::value) {
                    using obj_type = typename std::decay_t<decltype(v)>::Result::mapped_type::value_type;
                    auto table_key = transaction_ref->find_table(internal::bridge::table_name_for_object_type(managed<obj_type>::schema.name));
                    if (!table_key) {
                        col_key = tbl->add_column_dictionary(*add_table<obj_type>(transaction_ref), v.name);
                    } else {
                        col_key = tbl->add_column_dictionary(*transaction_ref->get_table(table_key), v.name);
                    }
                } else {
                    col_key = tbl->add_column_dictionary(property_type_to_data_type(v.type), v.name);
                }
            } else if constexpr (internal::type_info::is_vector<typename std::decay_t<decltype(v)>::Result>::value) {
                if constexpr (internal::type_info::is_link<typename std::decay_t<decltype(v)>::Result::value_type>::value) {
                    using obj_type = typename std::decay_t<decltype(v)>::Result::value_type::value_type;
                    auto table_key = transaction_ref->find_table(internal::bridge::table_name_for_object_type(managed<obj_type>::schema.name));
                    if (!table_key) {
                        col_key = tbl->add_column_list(*add_table<obj_type>(transaction_ref), v.name);
                    } else {
                        col_key = tbl->add_column_list(*transaction_ref->get_table(table_key), v.name);
                    }
                } else if constexpr (std::is_same_v<typename std::decay_t<decltype(v)>::Result::value_type, uint8_t>) {
                    col_key = tbl->add_column(property_type_to_data_type(v.type), v.name, is_nullable(static_cast<PropertyType>(v.type)));
                }
                else {
                    col_key = tbl->add_column_list(property_type_to_data_type(v.type), v.name);
                }
            } else {
                if constexpr (internal::type_info::is_link<typename std::decay_t<decltype(v)>::Result>::value) {
                    using obj_type = typename std::decay_t<decltype(v)>::Result::value_type;
                    auto table_key = transaction_ref->find_table(internal::bridge::table_name_for_object_type(managed<obj_type>::schema.name));
                    if (!table_key) {
                        col_key = tbl->add_column(*add_table<obj_type>(transaction_ref), v.name);
                    } else {
                        col_key = tbl->add_column(*transaction_ref->get_table(table_key), v.name);
                    }
                } else {
                    col_key = tbl->add_column(property_type_to_data_type(v.type), v.name, is_nullable(static_cast<PropertyType>(v.type)));
                }
            }
            if (v.is_primary_key) {
                tbl->set_primary_key_column(col_key);
            }
        }

        template <typename T>
        void insert(const std::vector<T> &v) {
            static_assert(sizeof(managed<T>), "Must declare schema for T");
            auto transaction_ref = m_db->start_write();
            auto table_name = internal::bridge::table_name_for_object_type(managed<T>::schema.name);
            if (!transaction_ref->find_table(table_name)) {
                add_table<T>(transaction_ref);
            }
            internal::bridge::table table = transaction_ref->get_table(internal::bridge::table_name_for_object_type(managed<T>::schema.name));

            for (auto& obj : v) {
                auto m_obj = table.create_object();
                std::apply([&m_obj, &obj](auto && ...p) {
                    (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                            m_obj, m_obj.get_table().get_column_key(p.name), obj.*(std::decay_t<decltype(p)>::ptr)
                    ), ...);
                }, managed<T>::schema.ps);
            }
        }

        template <typename ...Ts>
        db open(const std::string& path, const std::shared_ptr<scheduler>& scheduler = scheduler::make_default()) {
            db_config config;
            config.set_path(path);
            config.set_scheduler(scheduler);
            std::vector<internal::bridge::object_schema> schema;
            (schema.push_back(managed<Ts>::schema.to_core_schema()), ...);
            config.set_schema(schema);
            return db(config);
        }
    };

    template <typename T>
    inline std::ostream& operator<< (std::ostream& stream, const realm::experimental::link<T>& object)
    {
        stream << "link:" << object.is_managed << std::endl;
        return stream;
    }
}


#endif //CPP_REALM_EXPERIMENTAL_DB_HPP
