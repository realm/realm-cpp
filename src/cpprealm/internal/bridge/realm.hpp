#ifndef CPP_REALM_BRIDGE_REALM_HPP
#define CPP_REALM_BRIDGE_REALM_HPP

#include <string>

namespace realm {
    class Realm;
    class RealmConfig;
    class SyncConfig;
    struct scheduler;
}

namespace realm::internal::bridge {
    struct group;
    struct schema;
    struct object_schema;
    struct table;

    struct realm {
        struct sync_config {
            sync_config();
            sync_config(const SyncConfig&); //NOLINT(google-explicit-constructor)
            operator SyncConfig() const; //NOLINT(google-explicit-constructor)
            unsigned char m_config[432]{};
        };
        struct config {
            config();
            config(const RealmConfig&); //NOLINT(google-explicit-constructor)
            config(const std::string& path,
                   const std::shared_ptr<scheduler>& scheduler,
                   const sync_config& sync_config);
            [[nodiscard]] std::string path() const;
            [[nodiscard]] sync_config sync_config() const;
            [[nodiscard]] std::shared_ptr<scheduler> scheduler() const;
            operator RealmConfig() const; //NOLINT(google-explicit-constructor)
            void set_schema(const std::vector<object_schema>&);
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
    private:
        std::shared_ptr<Realm> m_realm;
        friend struct group;
    };

    bool operator ==(const realm&, const realm&);
    bool operator !=(const realm&, const realm&);
}

#endif //CPP_REALM_BRIDGE_REALM_HPP
