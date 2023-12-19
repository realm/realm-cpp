#ifndef CPP_REALM_BRIDGE_REALM_HPP
#define CPP_REALM_BRIDGE_REALM_HPP

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    class Realm;
    struct RealmConfig;
    struct SyncConfig;
    struct scheduler;
    class SyncUser;

    enum class client_reset_mode: uint8_t {
        // Fire a client reset error
        manual,
        // Discard unsynced local changes, without disrupting accessors or closing the Realm
        discard_unsynced,
        // Attempt to recover unsynchronized but committed changes.
        recover,
        // Attempt recovery and if that fails, discard local.
        recover_or_discard,
    };
}

namespace realm::internal::bridge {
    template<typename T> struct client_reset_mode_base;
    struct group;
    struct schema;
    struct object_schema;
    struct table;
    struct dictionary;
    struct thread_safe_reference;
    struct obj;
    struct object;
    struct async_open_task;
    struct sync_session;
    struct sync_error;

    struct realm {
        enum class sync_session_stop_policy: uint8_t {
            immediately,          // Immediately stop the session as soon as all Realms/Sessions go out of scope.
            live_indefinitely,     // Never stop the session.
            after_changes_uploaded, // Once all Realms/Sessions go out of scope, wait for uploads to complete and stop.
        };

        struct sync_config {

            struct proxy_config {
                using port_type = std::uint_fast16_t;
                std::string address;
                port_type port;
                // For basic authorization.
                std::optional<std::pair<std::string, std::string>> username_password;
            };

            struct flx_sync_enabled {};
            sync_config() {}
            sync_config(const std::shared_ptr<SyncUser> &user);
            sync_config(const std::shared_ptr<SyncConfig> &);//NOLINT(google-explicit-constructor)
            operator std::shared_ptr<SyncConfig>() const;    //NOLINT(google-explicit-constructor)
            void set_stop_policy(sync_session_stop_policy &&);
            void set_error_handler(std::function<void(const sync_session &, const sync_error &)> &&fn);

        private:
            std::shared_ptr<SyncConfig> m_config;
        };

        struct config {
            // How to handle update_schema() being called on a file which has
            // already been initialized with a different schema
            enum class schema_mode : uint8_t {
                // If the schema version has increased, automatically apply all
                // changes, then call the migration function.
                //
                // If the schema version has not changed, verify that the only
                // changes are to add new tables and add or remove indexes, and then
                // apply them if so. Does not call the migration function.
                //
                // This mode does not automatically remove tables which are not
                // present in the schema that must be manually done in the migration
                // function, to support sharing a Realm file between processes using
                // different class subsets.
                //
                // This mode allows using schemata with different subsets of tables
                // on different threads, but the tables which are shared must be
                // identical.
                automatic,

                // Open the file in immutable mode. Schema version must match the
                // version in the file, and all tables present in the file must
                // exactly match the specified schema, except for indexes. Tables
                // are allowed to be missing from the file.
                immutable,

                // Open the Realm in read-only mode, transactions are not allowed to
                // be performed on the Realm instance. The schema of the existing Realm
                // file won't be changed through this Realm instance. Extra tables and
                // extra properties are allowed in the existing Realm schema. The
                // difference of indexes is allowed as well. Other schema differences
                // than those will cause an exception. This is different from Immutable
                // mode, sync Realm can be opened with ReadOnly mode. Changes
                // can be made to the Realm file through another writable Realm instance.
                // Thus, notifications are also allowed in this mode.
                read_only,

                // If the schema version matches and the only schema changes are new
                // tables and indexes being added or removed, apply the changes to
                // the existing file.
                // Otherwise delete the file and recreate it from scratch.
                // The migration function is not used.
                //
                // This mode allows using schemata with different subsets of tables
                // on different threads, but the tables which are shared must be
                // identical.
                soft_reset_file,

                // Delete the file and recreate it from scratch.
                // The migration function is not used.
                hard_reset_file,

                // The only changes allowed are to add new tables, add columns to
                // existing tables, and to add or remove indexes from existing
                // columns. Extra tables not present in the schema are ignored.
                // Indexes are only added to or removed from existing columns if the
                // schema version is greater than the existing one (and unlike other
                // modes, the schema version is allowed to be less than the existing
                // one).
                // The migration function is not used.
                // This should be used when including discovered user classes.
                // Previously called Additive.
                //
                // This mode allows updating the schema with additive changes even
                // if the Realm is already open on another thread.
                additive_discovered,

                // The same additive properties as AdditiveDiscovered, except
                // in this mode, all classes in the schema have been explicitly
                // included by the user. This means that stricter schema checks are
                // run such as throwing an error when an embedded object type which
                // is not linked from any top level object types is included.
                additive_explicit,

                // Verify that the schema version has increased, call the migration
                // function, and then verify that the schema now matches.
                // The migration function is mandatory for this mode.
                //
                // This mode requires that all threads and processes which open a
                // file use identical schemata.
                manual
            };
            config();
            config(const config& other) ;
            config& operator=(const config& other) ;
            config(config&& other);
            config& operator=(config&& other);
            ~config();
            config(const RealmConfig&); //NOLINT(google-explicit-constructor)
            config(const std::string& path,
                   const std::shared_ptr<struct scheduler>& scheduler);
            [[nodiscard]] std::string path() const;
            [[nodiscard]] struct sync_config sync_config() const;
            [[nodiscard]] std::shared_ptr<struct scheduler> scheduler();
            operator RealmConfig() const; //NOLINT(google-explicit-constructor)
            void set_path(const std::string&);
            void set_schema(const std::vector<object_schema>&);
            void set_schema_mode(schema_mode);
            void set_scheduler(const std::shared_ptr<struct scheduler>&);
            void set_sync_config(const std::optional<struct sync_config>&);
            void set_custom_http_headers(const std::map<std::string, std::string>& headers);
            void set_proxy_config(const sync_config::proxy_config&);
            void set_schema_version(uint64_t version);
            void set_encryption_key(const std::array<char, 64>&);
            std::optional<schema> get_schema();

            template<typename T>
            void set_client_reset_handler(const client_reset_mode_base<T>& handler) {
                before_client_reset([fn = std::move(handler.m_before)](realm local_realm) {
                    fn(local_realm.freeze());
                });
                after_client_reset([fn = std::move(handler.m_after)](realm local_realm, realm remote_realm) {
                    fn(local_realm.freeze(), remote_realm);
                });
                set_client_reset_mode(handler.m_mode);
            }
            enum client_reset_mode get_client_reset_mode() const;
        private:
            void set_client_reset_mode(enum client_reset_mode mode);
            void before_client_reset(std::function<void(realm old_realm)> callback);
            void after_client_reset(std::function<void(realm local_realm, realm remote_realm)> callback);
            inline RealmConfig* get_config();
            inline const RealmConfig* get_config() const;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
            storage::Realm_Config m_config[1];
#else
            std::shared_ptr<RealmConfig> m_config;
#endif
        };

        realm();
        realm(const config&); //NOLINT(google-explicit-constructor)
        realm(std::shared_ptr<Realm>); //NOLINT(google-explicit-constructor)
        realm(thread_safe_reference&& tsr, const std::optional<std::shared_ptr<scheduler>>&); //NOLINT(google-explicit-constructor)
        operator std::shared_ptr<Realm>() const; //NOLINT(google-explicit-constructor)
        group read_group();
        [[nodiscard]] config get_config() const;
        [[nodiscard]] struct schema schema() const;
        void begin_transaction() const;
        void commit_transaction() const;
        table table_for_object_type(const std::string& object_type);
        table get_table(const uint32_t &);
        [[nodiscard]] std::shared_ptr<struct scheduler> scheduler() const;
        static async_open_task get_synchronized_realm(const config&);
        bool refresh();
        bool is_frozen() const;
        realm freeze(); // throws
        realm thaw(); // throws
        void close();
        bool is_closed();
        void invalidate();
        obj import_copy_of(const obj&) const;
        [[nodiscard]] std::optional<sync_session> get_sync_session() const;
    private:
        std::shared_ptr<Realm> m_realm;
        friend struct group;
    };

    template<typename T>
    struct client_reset_mode_base {
    protected:
        std::function<void(T local)> m_before;
        std::function<void(T local, T remote)> m_after;
        ::realm::client_reset_mode m_mode;
        friend struct internal::bridge::realm::config;
    };

    template <typename T>
    T resolve(const realm&, thread_safe_reference&& tsr);
    template <>
    dictionary resolve(const realm&, thread_safe_reference&& tsr);
    template <>
    object resolve(const realm&, thread_safe_reference&& tsr);

    bool operator ==(const realm&, const realm&);
    bool operator !=(const realm&, const realm&);
}

#endif //CPP_REALM_BRIDGE_REALM_HPP
