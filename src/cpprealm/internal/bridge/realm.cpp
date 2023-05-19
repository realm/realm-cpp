#include <cpprealm/internal/bridge/realm.hpp>

#include <cpprealm/app.hpp>
#include <cpprealm/analytics.hpp>
#include <cpprealm/logger.hpp>
#include <cpprealm/scheduler.hpp>
#include <cpprealm/internal/bridge/object_schema.hpp>
#include <cpprealm/internal/bridge/schema.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/object.hpp>
#include <cpprealm/internal/bridge/dictionary.hpp>
#include <cpprealm/internal/bridge/thread_safe_reference.hpp>
#include <cpprealm/internal/bridge/async_open_task.hpp>
#include <cpprealm/internal/bridge/sync_session.hpp>
#include <cpprealm/internal/bridge/sync_error.hpp>

#include <realm/sync/config.hpp>

#include <realm/object-store/schema.hpp>
#include <realm/object-store/shared_realm.hpp>
#include <realm/object-store/thread_safe_reference.hpp>
#include <realm/object-store/dictionary.hpp>
#include <realm/object-store/sync/sync_user.hpp>
#include <realm/object-store/sync/sync_session.hpp>

#include <realm/object-store/util/scheduler.hpp>

#include <filesystem>

namespace realm::internal::bridge {
#ifdef __i386__
    static_assert(SizeCheck<192, sizeof(Realm::Config)>{});
    static_assert(SizeCheck<8, alignof(Realm::Config)>{});
#elif __x86_64__
    #if defined(__clang__)
    static_assert(SizeCheck<368, sizeof(Realm::Config)>{});
    static_assert(SizeCheck<16, alignof(Realm::Config)>{});
    #elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<328, sizeof(Realm::Config)>{});
    static_assert(SizeCheck<8, alignof(Realm::Config)>{});
    #endif
#elif __arm__
    static_assert(SizeCheck<192, sizeof(Realm::Config)>{});
    static_assert(SizeCheck<8, alignof(Realm::Config)>{});
#elif __aarch64__
    #if __ANDROID__
    static_assert(SizeCheck<368, sizeof(Realm::Config)>{});
    static_assert(SizeCheck<16, alignof(Realm::Config)>{});
    #else
#if defined(__clang__)
    static_assert(SizeCheck<312, sizeof(Realm::Config)>{});
#elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<328, sizeof(Realm::Config)>{});
#endif
    static_assert(SizeCheck<8, alignof(Realm::Config)>{});
    #endif
#elif _WIN32
    static_assert(SizeCheck<456, sizeof(Realm::Config)>{});
    static_assert(SizeCheck<8, alignof(Realm::Config)>{});
#endif

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
        new (&m_config) RealmConfig(config);
    }

    realm::config::config(const config& other) {
        new (&m_config) RealmConfig(*reinterpret_cast<const RealmConfig*>(&other.m_config));
    }

    realm::config& realm::config::operator=(const config& other) {
        if (this != &other) {
            *reinterpret_cast<RealmConfig*>(&m_config) = *reinterpret_cast<const RealmConfig*>(&other.m_config);
        }
        return *this;
    }

    realm::config::config(config&& other) {
        new (&m_config) RealmConfig(std::move(*reinterpret_cast<RealmConfig*>(&other.m_config)));
    }

    realm::config& realm::config::operator=(config&& other) {
        if (this != &other) {
            *reinterpret_cast<RealmConfig*>(&m_config) = std::move(*reinterpret_cast<RealmConfig*>(&other.m_config));
        }
        return *this;
    }

    realm::config::~config() {
        reinterpret_cast<RealmConfig*>(&m_config)->~RealmConfig();
    }

    realm::config::config(const RealmConfig &v) {
        new (&m_config) RealmConfig(v);
    }
    realm::config::config(const std::string& path,
                          const std::shared_ptr<struct scheduler>& scheduler) {
        RealmConfig config;
        config.cache = true;
        config.path = path;
        config.scheduler = std::make_shared<internal_scheduler>(scheduler);
        config.schema_version = 0;
        new (&m_config) RealmConfig(config);
    }

    realm::sync_config::sync_config(const std::shared_ptr<SyncConfig> &v) {
        m_config = v;
    }
    realm::sync_config::operator std::shared_ptr<SyncConfig>() const {
        return m_config;
    }

    std::string realm::config::path() const {
        return reinterpret_cast<const RealmConfig*>(&m_config)->path;
    }
    realm::config realm::get_config() const {
        return m_realm->config();
    }
    void realm::config::set_schema(const std::vector<object_schema> &v) {
        std::vector<ObjectSchema> v2;
        for (auto& os : v) {
            v2.push_back(os);
        }
        reinterpret_cast<RealmConfig*>(&m_config)->schema_version = 0;
        reinterpret_cast<RealmConfig*>(&m_config)->schema = v2;
    }
    schema realm::schema() const {
        return m_realm->schema();
    }

    table realm::table_for_object_type(const std::string &object_type) {
        return read_group().get_table(object_type);
    }

    realm::realm() {

    }
    realm::config::operator RealmConfig() const {
        return *reinterpret_cast<const RealmConfig*>(&m_config);
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
        return reinterpret_cast<ThreadSafeReference*>(tsr.m_thread_safe_reference)->resolve<Dictionary>(r);
    }
    template <>
    object resolve(const realm& r, thread_safe_reference &&tsr) {
        return reinterpret_cast<ThreadSafeReference*>(tsr.m_thread_safe_reference)->resolve<Object>(r);
    }
    void realm::config::set_scheduler(const std::shared_ptr<struct scheduler> &s) {
        reinterpret_cast<RealmConfig*>(&m_config)->scheduler = std::make_shared<internal_scheduler>(s);
    }
    void realm::config::set_sync_config(const std::optional<struct sync_config> &s) {
        if (s)
            reinterpret_cast<RealmConfig*>(&m_config)->sync_config = static_cast<std::shared_ptr<SyncConfig>>(*s);
        else
            reinterpret_cast<RealmConfig*>(&m_config)->sync_config = nullptr;
    }

    realm::sync_config realm::config::sync_config() const {
        return reinterpret_cast<const RealmConfig*>(&m_config)->sync_config;
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
        reinterpret_cast<RealmConfig*>(&m_config)->path = path;
    }

    bool realm::refresh() {
        return m_realm->refresh();
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

}
