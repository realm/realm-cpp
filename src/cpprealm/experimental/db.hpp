#ifndef CPPREALM_EXPERIMENTAL_DB_HPP
#define CPPREALM_EXPERIMENTAL_DB_HPP

#include <cpprealm/experimental/accessors.hpp>

#include <cpprealm/schema.hpp>

#include <cpprealm/scheduler.hpp>
#include <cpprealm/thread_safe_reference.hpp>

#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/experimental/results.hpp>
#include <cpprealm/experimental/types.hpp>

#include <cpprealm/internal/bridge/sync_session.hpp>

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
        friend struct ::realm::thread_safe_reference<experimental::db>;
        template <typename, typename> friend struct managed;
    private:
        db(internal::bridge::realm&& r)
        {
            m_realm = std::move(r);
        }
        db(const internal::bridge::realm& r)
        {
            m_realm = r;
        }
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
