#ifndef CPP_REALM_BRIDGE_REALM_HPP
#define CPP_REALM_BRIDGE_REALM_HPP

#include <string>

namespace realm {
    class Realm;
    class RealmConfig;
    struct scheduler;
}

namespace realm::internal::bridge {
    struct group;
    struct schema;
    struct realm {
        struct sync_config {

        };
        struct config {
            config(const RealmConfig&);
            config(const std::string& path,
                   const std::shared_ptr<scheduler>& scheduler,
                   const schema& schema);
            std::string path() const;
            sync_config sync_config() const;

        private:
            unsigned char m_config[312]{};
        };
        realm();
        realm(const config&);
        realm(std::shared_ptr<Realm> );
        group read_group();
        config get_config() const;
        schema schema() const;
        void begin_transaction() const;
        void commit_transaction() const;
    private:
        std::shared_ptr<Realm> m_realm;
        friend struct group;
    };

    bool operator ==(const realm&, const realm&);
    bool operator !=(const realm&, const realm&);
}

#endif //CPP_REALM_BRIDGE_REALM_HPP
