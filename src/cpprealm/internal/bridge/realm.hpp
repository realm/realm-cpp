#ifndef CPP_REALM_BRIDGE_REALM_HPP
#define CPP_REALM_BRIDGE_REALM_HPP

#include <string>
#include <optional>

namespace realm {
    class Realm;
    class RealmConfig;
    class SyncConfig;
    struct scheduler;
    struct SyncUser;
}

namespace realm::internal::bridge {
    struct group;
    struct schema;
    struct object_schema;
    struct table;
    struct dictionary;
    struct thread_safe_reference;
    struct object;
    struct async_open_task;
    struct sync_session;
    struct sync_error;

    struct realm {
        enum class sync_session_stop_policy {
            Immediately,          // Immediately stop the session as soon as all Realms/Sessions go out of scope.
            LiveIndefinitely,     // Never stop the session.
            AfterChangesUploaded, // Once all Realms/Sessions go out of scope, wait for uploads to complete and stop.
        };

        enum class client_resync_mode : unsigned char {
            // Fire a client reset error
            Manual,
            // Discard local changes, without disrupting accessors or closing the Realm
            DiscardLocal,
            // Attempt to recover unsynchronized but committed changes.
            Recover,
            // Attempt recovery and if that fails, discard local.
            RecoverOrDiscard,
        };

        struct sync_config {
            struct flx_sync_enabled {};

            sync_config(const std::shared_ptr<SyncUser>& user);
            sync_config(const std::shared_ptr<SyncConfig> &); //NOLINT(google-explicit-constructor)
            operator std::shared_ptr<SyncConfig>() const; //NOLINT(google-explicit-constructor)
            void set_client_resync_mode(client_resync_mode&&);
            void set_stop_policy(sync_session_stop_policy&&);
            void set_error_handler(std::function<void(const sync_session&, const sync_error&)>&& fn);
        private:
            std::shared_ptr<SyncConfig> m_config;
        };

        struct config {
            config();
            config(const RealmConfig&); //NOLINT(google-explicit-constructor)
            config(const std::string& path,
                   const std::shared_ptr<scheduler>& scheduler,
                   const std::optional<sync_config>& sync_config);
            [[nodiscard]] std::string path() const;
            [[nodiscard]] sync_config sync_config() const;
            [[nodiscard]] std::shared_ptr<scheduler> scheduler() const;
            operator RealmConfig() const; //NOLINT(google-explicit-constructor)
            void set_path(const std::string&);
            void set_schema(const std::vector<object_schema>&);
            void set_scheduler(const std::shared_ptr<struct scheduler>&);
            void set_sync_config(const std::optional<struct sync_config>&);
        private:
            unsigned char m_config[312]{};
        };
        realm();
        realm(const config&); //NOLINT(google-explicit-constructor)
        realm(std::shared_ptr<Realm>); //NOLINT(google-explicit-constructor)
        operator std::shared_ptr<Realm>() const; //NOLINT(google-explicit-constructor)
        group read_group();
        [[nodiscard]] config get_config() const;
        [[nodiscard]] schema schema() const;
        void begin_transaction() const;
        void commit_transaction() const;
        table table_for_object_type(const std::string& object_type);
        template <typename T>
        T resolve(thread_safe_reference&& tsr);
        template <>
        dictionary resolve(thread_safe_reference&& tsr);
        template <>
        object resolve(thread_safe_reference&& tsr);
        [[nodiscard]] std::shared_ptr<scheduler> scheduler() const;
        static async_open_task get_synchronized_realm(const config&);
    private:
        std::shared_ptr<Realm> m_realm;
        friend struct group;
    };

    bool operator ==(const realm&, const realm&);
    bool operator !=(const realm&, const realm&);
}

#endif //CPP_REALM_BRIDGE_REALM_HPP
