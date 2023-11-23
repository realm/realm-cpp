#ifndef CPPREALM_EXPERIMENTAL_DB_HPP
#define CPPREALM_EXPERIMENTAL_DB_HPP

#include <cpprealm/experimental/accessors.hpp>

#include <realm/transaction.hpp>
#include <realm/object-store/property.hpp>

#include <cpprealm/schema.hpp>

#include <cpprealm/internal/bridge/sync_session.hpp>
#include <cpprealm/scheduler.hpp>
#include <cpprealm/thread_safe_reference.hpp>

#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/experimental/results.hpp>
#include <cpprealm/experimental/types.hpp>

#include <filesystem>
#include <optional>
#include <string>
#include <utility>

namespace realm {
    namespace {
        template<typename T>
        using is_optional = internal::type_info::is_optional<T>;
    }
    namespace schemagen {
        template <typename Class, typename ...Properties>
        struct schema;
        template <auto Ptr, bool IsPrimaryKey>
        struct property;
    }

    using sync_config = internal::bridge::realm::sync_config;
    using db_config = internal::bridge::realm::config;
    using sync_session = internal::bridge::sync_session;

    struct sync_subscription_set;
}

namespace realm::experimental {

    struct db {
        static inline std::vector<internal::bridge::object_schema> schemas;
        internal::bridge::realm m_realm;
        explicit db(realm::db_config config)
        {
            if (!config.get_schema())
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
        template <typename U>
        managed<std::remove_const_t<U>> add(U &&v) {
            using T = std::remove_const_t<U>;
            static_assert(sizeof(managed<T>), "Must declare schema for T");
            auto table = m_realm.table_for_object_type(managed<std::remove_const_t<T>>::schema.name);
            internal::bridge::obj m_obj;
            if constexpr (managed<std::remove_const_t<T>>::schema.HasPrimaryKeyProperty) {
                auto pk = v.*(managed<std::remove_const_t<T>>::schema.primary_key().ptr);
                m_obj = table.create_object_with_primary_key(realm::internal::bridge::mixed(serialize(pk.value)));
            } else {
                m_obj = table.create_object();
            }

            std::apply([&m_obj, &v, this](auto && ...p) {
                (accessor<typename std::decay_t<decltype(p)>::Result>::set(
                        m_obj, m_obj.get_table().get_column_key(p.name), m_realm, v.*(std::decay_t<decltype(p)>::ptr)
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
                    m_obj = table.create_object_with_primary_key(realm::internal::bridge::mixed(serialize(pk.value)));
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

    private:
        template <size_t N, typename Tpl, typename ...Ts> auto v_add(const Tpl& tpl, const std::tuple<Ts...>& vs) {
            if constexpr (N + 1 == sizeof...(Ts)) {
                auto managed = add(std::move(std::get<N>(vs)));
                return std::tuple_cat(tpl, std::make_tuple(std::move(managed)));
            } else {
                auto managed = add(std::move(std::get<N>(vs)));
                return v_add<N + 1>(std::tuple_cat(tpl, std::make_tuple(std::move(managed))), vs);
            }
        }
    public:
        template <typename ...Ts>
        std::tuple<managed<Ts>...> insert(Ts&&... v) {
            std::tuple<> tpl;
            return v_add<0>(tpl, std::make_tuple(v...));
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

        ::realm::sync_subscription_set subscriptions();

        /**
        An object encapsulating an Atlas App Services "session". Sessions represent the
        communication between the client (and a local Realm file on disk), and the server

        Sessions are always created by the SDK and vended out through various APIs. The
        lifespans of sessions associated with Realms are managed automatically.
        */
        std::optional<sync_session> get_sync_session() const {
            return m_realm.get_sync_session();
        }

        template <typename T>
        managed<T> resolve(thread_safe_reference<T>&& tsr)
        {
            auto object = internal::bridge::resolve<internal::bridge::object>(m_realm, std::move(tsr.m_tsr));
            internal::bridge::obj m_obj = object.get_obj();
            auto m = managed<T>(std::move(m_obj), m_realm);
            std::apply([&m](auto && ...ptr) {
                std::apply([&](auto&& ...name) {
                    ((m.*ptr).assign(&m.m_obj, &m.m_realm, m.m_obj.get_table().get_column_key(name)), ...);
                }, managed<T>::managed_pointers_names);
            }, managed<T>::managed_pointers());
            return m;
        }

        bool is_frozen() const;
        db freeze();
        db thaw();
        void invalidate();
    private:
        friend struct ::realm::thread_safe_reference<experimental::db>;
        db(internal::bridge::realm&& r)
        {
            m_realm = std::move(r);
        }

        std::unordered_map<std::string, internal::bridge::table> m_object_tables;
    };

    bool operator==(const db&, const db&);
    bool operator!=(const db&, const db&);

    template <typename ...Ts>
    inline db open(const db_config& config) {
        auto config_copy = config;
        if constexpr (sizeof...(Ts) == 0) {
            config_copy.set_schema(db::schemas);
        } else {
            std::vector<internal::bridge::object_schema> schema;
            (schema.push_back(managed<Ts>::schema.to_core_schema()), ...);
            config_copy.set_schema(schema);
        }
        return db(config_copy);
    }
    template <typename ...Ts>
    inline db open(const std::string& path, const std::shared_ptr<scheduler>& scheduler = scheduler::make_default()) {
        return open<Ts...>(db_config(path, scheduler));
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
                    m_obj = table.create_object_with_primary_key(realm::internal::bridge::mixed(serialize(pk.value)));
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
    inline std::ostream& operator<< (std::ostream& stream, const T*& object)
    {
        stream << "link:" << object << std::endl;
        return stream;
    }
}

namespace realm {

    template <typename T>
    struct thread_safe_reference<T, std::enable_if_t<sizeof(experimental::managed<T>) != 0>> {
        explicit thread_safe_reference(const experimental::managed<T>& object)
            : m_tsr(internal::bridge::thread_safe_reference(internal::bridge::object(object.m_realm, object.m_obj)))
        {
        }
        thread_safe_reference(internal::bridge::thread_safe_reference&& tsr)
            : m_tsr(std::move(tsr))
        {
        }
        experimental::managed<T> resolve(std::shared_ptr<Realm> const&);
    private:
        internal::bridge::thread_safe_reference m_tsr;
        friend struct experimental::db;
    };

    template<>
    struct thread_safe_reference<experimental::db> {
        thread_safe_reference(internal::bridge::thread_safe_reference&& tsr)
            : m_tsr(std::move(tsr))
        {
        }
        thread_safe_reference() = default;
        thread_safe_reference(thread_safe_reference&&) = delete;
        thread_safe_reference& operator=(thread_safe_reference&&) noexcept = default;
        ~thread_safe_reference() = default;

        experimental::db resolve(const std::optional<std::shared_ptr<scheduler>>& s = std::nullopt)
        {
            return experimental::db(internal::bridge::realm(std::move(m_tsr), s));
        }

        explicit operator bool() const noexcept
        {
            return m_tsr.operator bool();
        }
    private:
        internal::bridge::thread_safe_reference m_tsr;
        friend struct experimental::db;
    };
}


#endif //CPPREALM_EXPERIMENTAL_DB_HPP
