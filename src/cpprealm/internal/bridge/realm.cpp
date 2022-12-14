#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/scheduler.hpp>
#include <cpprealm/internal/bridge/object_schema.hpp>
#include <cpprealm/internal/bridge/schema.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/obj.hpp>

#include <realm/object-store/schema.hpp>
#include <realm/object-store/property.hpp>
#include <realm/object-store/shared_realm.hpp>
#include <realm/sync/config.hpp>
#include <realm/object-store/util/scheduler.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<312, sizeof(Realm::Config)>{});

    realm::realm(std::shared_ptr<Realm> v)
    : m_realm(std::move(v)){}

    realm::operator std::shared_ptr<Realm>() const {
        return m_realm;
    }
    void realm::begin_transaction() const {
        m_realm->begin_transaction();
    }

    void realm::commit_transaction() const {
        m_realm->commit_transaction();
    }

    struct internal_scheduler : util::Scheduler {
        internal_scheduler(const std::shared_ptr<scheduler>& s)
        : m_scheduler(s)
        {
        }

        ~internal_scheduler() override = default;
        void invoke(util::UniqueFunction<void ()> &&fn) override {
            m_scheduler->invoke([&fn]() {
                fn();
            });
        }

        bool is_on_thread() const noexcept override {
            return m_scheduler->is_on_thread();
        }
        bool is_same_as(const util::Scheduler *other) const noexcept override {
            return this == other;
        }

        bool can_invoke() const noexcept override {
            return m_scheduler->can_invoke();
        }
        std::shared_ptr<scheduler> m_scheduler;
    };

    realm::config::config(const RealmConfig &v) {
        new (&m_config) RealmConfig(v);
    }
    realm::config::config(const std::string &path,
                          const std::shared_ptr<struct scheduler>& scheduler,
                          const struct sync_config &s) {
        RealmConfig config;
        config.path = path;
        config.scheduler = std::make_shared<internal_scheduler>(scheduler);
        config.sync_config = std::make_shared<SyncConfig>(s);
        new (&m_config) RealmConfig(config);
    }

    static_assert(SizeCheck<432, sizeof(SyncConfig)>{});

    realm::sync_config::sync_config() {
        new (&m_config) SyncConfig();
    }
    realm::sync_config::sync_config(const SyncConfig &v) {
        new (&m_config) SyncConfig(v);
    }
    realm::sync_config::operator SyncConfig() const {
        return *reinterpret_cast<const SyncConfig*>(m_config);
    }
    std::string realm::config::path() const {
        return reinterpret_cast<const RealmConfig*>(m_config)->path;
    }

    void realm::config::set_schema(const std::vector<object_schema> &v) {
        std::vector<ObjectSchema> v2;
        for (auto& os : v) {
            v2.push_back(os);
        }
        reinterpret_cast<RealmConfig*>(m_config)->schema = v2;
    }
    schema realm::schema() const {
        return m_realm->schema();
    }

    table realm::table_for_object_type(const std::string &object_type) {
        auto name = table_name_for_object_type(object_type);
        return read_group().get_table(name);
    }
    realm::config::config() {
        new (m_config) RealmConfig();
    }
    realm::realm() {

    }
    realm::config::operator RealmConfig() const {
        return *reinterpret_cast<const RealmConfig*>(m_config);
    }
    realm::realm(const config &v) {
        m_realm = Realm::get_shared_realm(v);
    }
}
