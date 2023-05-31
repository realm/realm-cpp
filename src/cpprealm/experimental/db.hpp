#ifndef CPP_REALM_EXPERIMENTAL_DB_HPP
#define CPP_REALM_EXPERIMENTAL_DB_HPP

#include <cpprealm/experimental/accessors.hpp>
#include <realm/db.hpp>
#include <realm/transaction.hpp>
#include <cpprealm/db.hpp>
#include <cpprealm/persisted.hpp>
#include <cpprealm/experimental/macros.hpp>

#include <cpprealm/schema.hpp>
#include <optional>
#include <realm/object-store/property.hpp>
#include <string>
#include <utility>

#include <cpprealm/experimental/managed_string.hpp>
#include <cpprealm/experimental/managed_numeric.hpp>
#include <cpprealm/experimental/managed_timestamp.hpp>
#include <cpprealm/experimental/managed_uuid.hpp>
#include <cpprealm/experimental/managed_objectid.hpp>
#include <cpprealm/experimental/managed_binary.hpp>
#include <cpprealm/experimental/managed_list.hpp>
#include <cpprealm/experimental/managed_dictionary.hpp>
#include <cpprealm/experimental/managed_mixed.hpp>

#include <cpprealm/experimental/results.hpp>
#include <cpprealm/experimental/types.hpp>

namespace realm::experimental {

    struct db {
        static inline std::vector<internal::bridge::object_schema> schemas;
        internal::bridge::realm m_realm;
        explicit db(realm::db_config config)
        {
            config.set_schema(db::schemas);
            m_realm = internal::bridge::realm(config);
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
            internal::bridge::obj m_obj;
            if constexpr (managed<T>::schema.HasPrimaryKeyProperty) {
                auto pk = v.*(managed<T>::schema.primary_key().ptr);
                m_obj = table.create_object_with_primary_key(pk.value);
            } else {
                m_obj = table.create_object();
            }

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
            }, managed<T>::managed_pointers());
            return m;
        }
        template <typename T>
        void remove(T& object)
        {
            auto table = m_realm.table_for_object_type(T::schema.name);
            table.remove_object(object.m_obj.get_key());
        }
        template <typename T>
        void insert(const std::vector<T> &v) {
            static_assert(sizeof(managed<T>), "Must declare schema for T");
            internal::bridge::table table = m_realm.table_for_object_type(managed<T>::schema.name);
            for (auto& obj : v) {
                internal::bridge::obj m_obj;
                if constexpr (managed<T>::schema.HasPrimaryKeyProperty) {
                    auto pk = obj.*(managed<T>::schema.primary_key().ptr);
                    m_obj = table.create_object_with_primary_key(pk.value);
                } else {
                    m_obj = table.create_object();
                }
                std::apply([&m_obj, &obj](auto && ...p) {
                    (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                            m_obj, m_obj.get_table().get_column_key(p.name), obj.*(std::decay_t<decltype(p)>::ptr)
                    ), ...);
                }, managed<T>::schema.ps);
            }
        }

        template <typename T>
        results<T> objects()
        {
            return results<T>(internal::bridge::results(m_realm, m_realm.table_for_object_type(managed<T>::schema.name)));
        }

        [[maybe_unused]] bool refresh()
        {
            return m_realm.refresh();
        }

        sync_subscription_set subscriptions() {
            return sync_subscription_set(m_realm);
        }
    };

    template <typename ...Ts>
    inline db open(const std::string& path, const std::shared_ptr<scheduler>& scheduler = scheduler::make_default()) {
        auto config = db_config(path, scheduler);
        if constexpr (sizeof...(Ts) == 0) {
            config.set_schema(db::schemas);
        } else {
            std::vector<internal::bridge::object_schema> schema;
            (schema.push_back(managed<Ts>::schema.to_core_schema()), ...);
            config.set_schema(schema);
        }
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
                internal::bridge::obj m_obj;
                if constexpr (managed<T>::schema.HasPrimaryKeyProperty) {
                    auto pk = obj.*(managed<T>::schema.primary_key().ptr);
                    m_obj = table.create_object_with_primary_key(pk.value);
                } else {
                    m_obj = table.create_object();
                }
                std::apply([&m_obj, &obj](auto && ...p) {
                    (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                            m_obj, m_obj.get_table().get_column_key(p.name), obj.*(std::decay_t<decltype(p)>::ptr)
                    ), ...);
                }, managed<T>::schema.ps);
            }
        }

        template <typename ...Ts>
        db open(const std::string& path, const std::shared_ptr<scheduler>& scheduler = scheduler::make_default()) {
            auto config = db_config(path, scheduler);
            if constexpr (sizeof...(Ts) == 0) {
                config.set_schema(db::schemas);
            } else {
                std::vector<internal::bridge::object_schema> schema;
                (schema.push_back(managed<Ts>::schema.to_core_schema()), ...);
                config.set_schema(schema);
            }
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