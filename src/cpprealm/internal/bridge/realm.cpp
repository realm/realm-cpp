#include <cpprealm/internal/bridge/realm.hpp>

#include <cpprealm/analytics.hpp>
#include <cpprealm/app.hpp>
#include <cpprealm/internal/bridge/async_open_task.hpp>
#include <cpprealm/internal/bridge/dictionary.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/object.hpp>
#include <cpprealm/internal/bridge/object_schema.hpp>
#include <cpprealm/internal/bridge/schema.hpp>
#include <cpprealm/internal/bridge/sync_error.hpp>
#include <cpprealm/internal/bridge/sync_session.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/thread_safe_reference.hpp>
#include <cpprealm/logger.hpp>
#include <cpprealm/scheduler.hpp>

#include <realm/object-store/dictionary.hpp>
#include <realm/object-store/object_store.hpp>

#include <realm/object-store/schema.hpp>
#include <realm/object-store/shared_realm.hpp>
#include <realm/object-store/sync/sync_session.hpp>
#include <realm/object-store/sync/sync_user.hpp>
#include <realm/object-store/thread_safe_reference.hpp>
#include <realm/object-store/util/scheduler.hpp>
#include <realm/sync/config.hpp>

#include <filesystem>

namespace realm::internal::bridge {
    static_assert((uint8_t)realm::config::schema_mode::Automatic == (uint8_t)::realm::SchemaMode::Automatic);
    static_assert((uint8_t)realm::config::schema_mode::Immutable == (uint8_t)::realm::SchemaMode::Immutable);
    static_assert((uint8_t)realm::config::schema_mode::ReadOnly == (uint8_t)::realm::SchemaMode::ReadOnly);
    static_assert((uint8_t)realm::config::schema_mode::SoftResetFile == (uint8_t)::realm::SchemaMode::SoftResetFile);
    static_assert((uint8_t)realm::config::schema_mode::HardResetFile == (uint8_t)::realm::SchemaMode::HardResetFile);
    static_assert((uint8_t)realm::config::schema_mode::AdditiveDiscovered == (uint8_t)::realm::SchemaMode::AdditiveDiscovered);
    static_assert((uint8_t)realm::config::schema_mode::AdditiveExplicit == (uint8_t)::realm::SchemaMode::AdditiveExplicit);
    static_assert((uint8_t)realm::config::schema_mode::Manual == (uint8_t)::realm::SchemaMode::Manual);

    class null_logger : public ::realm::logger {
    public:
        null_logger() = default;
        void do_log(logger::level, const std::string&) override {}
    };

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
            m_scheduler->invoke(std::move(fn));
        }

        bool is_on_thread() const noexcept override {
            return m_scheduler->is_on_thread();
        }
        bool is_same_as(const util::Scheduler *other) const noexcept override {
            if (auto o = dynamic_cast<const internal_scheduler *>(other)) {
                return m_scheduler->is_same_as(o->m_scheduler.get());
            }
            return false;
        }

        bool can_invoke() const noexcept override {
            return m_scheduler->can_invoke();
        }
    private:
        std::shared_ptr<scheduler> m_scheduler;
    };

    realm::realm(thread_safe_reference&& tsr, const std::optional<std::shared_ptr<struct scheduler>>& s) {
        if (s) {
            m_realm = Realm::get_shared_realm(std::move(tsr), std::make_shared<internal_scheduler>(*s));
        } else {
            m_realm = Realm::get_shared_realm(std::move(tsr));
        }
    }

    realm::config::config() {
        RealmConfig config;
        config.cache = true;
        config.path = std::filesystem::current_path().append("default.realm").generic_string();
        config.scheduler = std::make_shared<internal_scheduler>(scheduler::make_default());
        config.schema_version = 0;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_config) RealmConfig(config);
#else
        m_config = std::make_shared<RealmConfig>(config);
#endif
    }

    realm::config::config(const config& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_config) RealmConfig(*reinterpret_cast<const RealmConfig*>(&other.m_config));
#else
        m_config = other.m_config;
#endif
    }

    realm::config& realm::config::operator=(const config& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<RealmConfig*>(&m_config) = *reinterpret_cast<const RealmConfig*>(&other.m_config);
        }
#else
        m_config = other.m_config;
#endif
        return *this;
    }

    realm::config::config(config&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_config) RealmConfig(std::move(*reinterpret_cast<RealmConfig*>(&other.m_config)));
#else
        m_config = std::move(other.m_config);
#endif
    }

    realm::config& realm::config::operator=(config&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<RealmConfig*>(&m_config) = std::move(*reinterpret_cast<RealmConfig*>(&other.m_config));
        }
#else
        m_config = std::move(other.m_config);
#endif
        return *this;
    }

    realm::config::~config() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<RealmConfig*>(&m_config)->~RealmConfig();
#endif
    }

    realm::config::config(const RealmConfig &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_config) RealmConfig(v);
#else
        m_config = std::make_shared<RealmConfig>(v);
#endif
    }
    realm::config::config(const std::string& path,
                          const std::shared_ptr<struct scheduler>& scheduler) {
        RealmConfig config;
        config.cache = true;
        config.path = path;
        config.scheduler = std::make_shared<internal_scheduler>(scheduler);
        config.schema_version = 0;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_config) RealmConfig(config);
#else
        m_config = std::make_shared<RealmConfig>(config);
#endif
    }

    realm::sync_config::sync_config(const std::shared_ptr<SyncConfig> &v) {
        m_config = v;
    }
    realm::sync_config::operator std::shared_ptr<SyncConfig>() const {
        return m_config;
    }

    inline RealmConfig* realm::config::get_config() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<RealmConfig*>(&m_config);
#else
        return m_config.get();
#endif
    }

    inline const RealmConfig* realm::config::get_config() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const RealmConfig*>(&m_config);
#else
        return m_config.get();
#endif
    }

    std::string realm::config::path() const {
        return get_config()->path;
    }
    realm::config realm::get_config() const {
        return m_realm->config();
    }
    void realm::config::set_schema(const std::vector<object_schema> &v) {
        std::vector<ObjectSchema> v2;
        for (auto& os : v) {
            v2.push_back(os);
        }
        get_config()->schema = v2;
    }
    void realm::config::set_schema_mode(schema_mode mode) {
        get_config()->schema_mode = static_cast<::realm::SchemaMode>(mode);
    }

    std::optional<schema> realm::config::get_schema() {
        if (auto s = get_config()->schema) {
            return *s;
        }
        return std::nullopt;
    }

    schema realm::schema() const {
        return m_realm->schema();
    }

    table realm::table_for_object_type(const std::string &object_type) {
        return read_group().get_table(object_type);
    }

    realm::realm() { }
    realm::config::operator RealmConfig() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const RealmConfig*>(&m_config);
#else
        return *m_config;
#endif
    }
    realm::realm(const config &v) {
        static bool initialized;
        if (!initialized) {
            set_default_level_threshold(logger::level::off);
            set_default_logger(std::make_shared<struct null_logger>());
            realm_analytics::send();
            initialized = true;
        }
        m_realm = Realm::get_shared_realm(static_cast<RealmConfig>(v));
    }
    bool operator==(realm const &lhs, realm const &rhs) {
        return static_cast<SharedRealm>(lhs) == static_cast<SharedRealm>(rhs);
    }
    bool operator!=(realm const& lhs, realm const& rhs) {
        return static_cast<SharedRealm>(lhs) != static_cast<SharedRealm>(rhs);
    }
    template <>
    dictionary resolve(const realm& r, thread_safe_reference &&tsr) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<ThreadSafeReference*>(tsr.m_thread_safe_reference)->resolve<Dictionary>(r);
#else
        return reinterpret_cast<ThreadSafeReference*>(tsr.m_thread_safe_reference.get())->resolve<Dictionary>(r);
#endif
    }
    template <>
    object resolve(const realm& r, thread_safe_reference &&tsr) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<ThreadSafeReference*>(tsr.m_thread_safe_reference)->resolve<Object>(r);
#else
        return reinterpret_cast<ThreadSafeReference*>(tsr.m_thread_safe_reference.get())->resolve<Object>(r);
#endif
    }
    void realm::config::set_scheduler(const std::shared_ptr<struct scheduler> &s) {
        get_config()->scheduler = std::make_shared<internal_scheduler>(s);
    }
    void realm::config::set_sync_config(const std::optional<struct sync_config> &s) {
        if (s)
            get_config()->sync_config = static_cast<std::shared_ptr<SyncConfig>>(*s);
        else
            get_config()->sync_config = nullptr;
    }

    void realm::config::set_custom_http_headers(const std::map<std::string, std::string>& headers) {
        if (get_config()->sync_config) {
            get_config()->sync_config->custom_http_headers = headers;
        } else {
            throw std::logic_error("HTTP headers can only be set on a config for a synced Realm.");
        }
    }

    void realm::config::set_schema_version(uint64_t version) {
        get_config()->schema_version = version;
    }

    void realm::config::set_encryption_key(const std::array<char, 64>& encryption_key) {
        auto key = std::vector<char>();
        key.resize(64);
        key.assign(encryption_key.begin(), encryption_key.end());
        reinterpret_cast<RealmConfig*>(&m_config)->encryption_key = std::move(key);
    }

    realm::sync_config realm::config::sync_config() const {
        return get_config()->sync_config;
    }

    struct external_scheduler final : public scheduler {
        // Invoke the given function on the scheduler's thread.
        //
        // This function can be called from any thread.
        void invoke(Function<void()> &&fn) final {
            s->invoke(std::move(fn));
        }

        // Check if the caller is currently running on the scheduler's thread.
        //
        // This function can be called from any thread.
        [[nodiscard]] bool is_on_thread() const noexcept final {
            return s->is_on_thread();
        }

        // Checks if this scheduler instance wraps the same underlying instance.
        // This is up to the platforms to define, but if this method returns true,
        // caching may occur.
        bool is_same_as(const scheduler *other) const noexcept final {
            return this == other;
        }

        // Check if this scheduler actually can support invoke(). Invoking may be
        // either not implemented, not applicable to a scheduler type, or simply not
        // be possible currently (e.g. if the associated event loop is not actually
        // running).
        //
        // This function is not thread-safe.
        [[nodiscard]] bool can_invoke() const noexcept final {
            return s->can_invoke();
        }
        std::shared_ptr<util::Scheduler> s;
        external_scheduler(std::shared_ptr<util::Scheduler>  s) : s(std::move(s)) {}
        ~external_scheduler() final = default;
    };

    struct std::shared_ptr<scheduler> realm::scheduler() const {
        return std::make_shared<external_scheduler>(external_scheduler(m_realm->scheduler()));
    }

    async_open_task realm::get_synchronized_realm(const config &c) {
        return Realm::get_synchronized_realm(c);
    }

    void realm::sync_config::set_error_handler(std::function<void(const sync_session &, const sync_error &)> &&fn) {
        m_config->error_handler = [fn = std::move(fn)](const std::shared_ptr<SyncSession>& session,
                                                       SyncError&& error) {
            fn(session, std::move(error));
        };
    }

    void realm::sync_config::set_stop_policy(realm::sync_session_stop_policy &&v) {
        m_config->stop_policy = static_cast<SyncSessionStopPolicy>(v);
    }

    void realm::sync_config::set_client_resync_mode(realm::client_resync_mode &&v) {
        m_config->client_resync_mode = static_cast<ClientResyncMode>(v);
    }

    realm::sync_config::sync_config(const std::shared_ptr<SyncUser> &user) {
        m_config = std::make_shared<SyncConfig>(user, SyncConfig::FLXSyncEnabled{});
    }

    void realm::config::set_path(const std::string &path) {
        get_config()->path = path;
    }

    bool realm::refresh() {
        return m_realm->refresh();
    }

    bool realm::is_frozen() const {
        return m_realm->is_frozen();
    }

    realm realm::freeze() {
        m_realm->verify_thread();
        if (is_frozen())
            return *this;
        realm realm;
        realm.m_realm = m_realm->freeze();
        realm.m_realm->read_group();
        return realm;
    }

    realm realm::thaw() {
        m_realm->verify_thread();
        if (!is_frozen())
            return *this;
        auto config = m_realm->config();
        config.cache = false;
        return realm(std::move(config));
    }

    void realm::invalidate() {
        m_realm->verify_thread();
        m_realm->invalidate();
    }

    obj realm::import_copy_of(const obj& o) const {
        return m_realm->import_copy_of(o.operator Obj());
    }

    [[nodiscard]] std::optional<sync_session> realm::get_sync_session() const {
        auto& config = m_realm->config().sync_config;
        if (!config) {
            return std::nullopt;
        }
        if (auto session = config->user->session_for_on_disk_path(m_realm->config().path)) {
            return sync_session(std::move(session));
        }
        return std::nullopt;
    }

    table realm::get_table(const uint32_t &key) {
        return m_realm->read_group().get_table(TableKey(key));
    }
}
