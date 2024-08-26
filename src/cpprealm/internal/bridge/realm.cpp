#include <cpprealm/internal/bridge/realm.hpp>

#include <cpprealm/logger.hpp>
#include <cpprealm/internal/bridge/dictionary.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/object.hpp>
#include <cpprealm/internal/bridge/object_schema.hpp>
#include <cpprealm/internal/bridge/schema.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/thread_safe_reference.hpp>
#include <cpprealm/internal/scheduler/realm_core_scheduler.hpp>
#include <cpprealm/schedulers/default_scheduler.hpp>

#include <realm/object-store/dictionary.hpp>
#include <realm/object-store/object_store.hpp>

#include <realm/object-store/schema.hpp>
#include <realm/object-store/shared_realm.hpp>
#include <realm/object-store/thread_safe_reference.hpp>
#include <realm/object-store/util/scheduler.hpp>

#if defined(REALM_AOSP_VENDOR)
#include <unistd.h>
#else
#include <filesystem>
#endif

namespace realm::internal::bridge {
    static_assert((uint8_t)realm::config::schema_mode::automatic == (uint8_t)::realm::SchemaMode::Automatic);
    static_assert((uint8_t)realm::config::schema_mode::immutable == (uint8_t)::realm::SchemaMode::Immutable);
    static_assert((uint8_t)realm::config::schema_mode::read_only == (uint8_t)::realm::SchemaMode::ReadOnly);
    static_assert((uint8_t)realm::config::schema_mode::soft_reset_file == (uint8_t)::realm::SchemaMode::SoftResetFile);
    static_assert((uint8_t)realm::config::schema_mode::hard_reset_file == (uint8_t)::realm::SchemaMode::HardResetFile);
    static_assert((uint8_t)realm::config::schema_mode::additive_discovered == (uint8_t)::realm::SchemaMode::AdditiveDiscovered);
    static_assert((uint8_t)realm::config::schema_mode::additive_explicit == (uint8_t)::realm::SchemaMode::AdditiveExplicit);
    static_assert((uint8_t)realm::config::schema_mode::manual == (uint8_t)::realm::SchemaMode::Manual);

    class null_logger : public ::realm::logger {
    public:
        null_logger() {
           set_level_threshold(logger::level::off);
        };
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

    realm::realm(thread_safe_reference&& tsr, const std::optional<std::shared_ptr<struct scheduler>>& s) {
        if (s) {
            m_realm = Realm::get_shared_realm(std::move(tsr), create_scheduler_shim(*s));
        } else {
            m_realm = Realm::get_shared_realm(std::move(tsr), create_scheduler_shim(default_scheduler::make_default()));
        }
    }

    realm::config::config() {
        RealmConfig config;
        config.cache = true;
#if defined(REALM_AOSP_VENDOR)
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        std::string path = cwd;
        path.append("/default.realm");
        config.path = path;
#else
        config.path = std::filesystem::current_path().append("default.realm").generic_string();
#endif
        config.scheduler = create_scheduler_shim(default_scheduler::make_default());
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
        config.scheduler = create_scheduler_shim(scheduler);
        config.schema_version = 0;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_config) RealmConfig(config);
#else
        m_config = std::make_shared<RealmConfig>(config);
#endif
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
            auto logger = std::make_shared<null_logger>();
            logger->set_level_threshold(logger::level::off);
            set_default_logger(logger);
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
        if (auto core_scheduler = dynamic_cast<realm_core_scheduler *>(s.get())) {
            get_config()->scheduler = core_scheduler->operator std::shared_ptr<util::Scheduler>();
            return;
        }
        get_config()->scheduler = create_scheduler_shim(s);
    }

    void realm::config::set_schema_version(uint64_t version) {
        get_config()->schema_version = version;
    }

    void realm::config::set_encryption_key(const std::array<char, 64>& encryption_key) {
        auto key = std::vector<char>();
        key.resize(64);
        key.assign(encryption_key.begin(), encryption_key.end());
        get_config()->encryption_key = std::move(key);
    }

    void realm::config::should_compact_on_launch(std::function<bool(uint64_t total_bytes, uint64_t unused_bytes)>&& fn) {
        get_config()->should_compact_on_launch_function = std::move(fn);
    }

    struct std::shared_ptr<scheduler> realm::config::scheduler() const {
        return std::make_shared<realm_core_scheduler>(realm_core_scheduler(get_config()->scheduler));
    }

    struct std::shared_ptr<scheduler> realm::scheduler() const {
        return std::make_shared<realm_core_scheduler>(realm_core_scheduler(m_realm->scheduler()));
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
        config.cache = true;
        config.scheduler = create_scheduler_shim(default_scheduler::make_default());
        return realm(std::move(config));
    }

    void realm::close() {
        m_realm->close();
    }

    bool realm::is_closed() {
        return m_realm->is_closed();
    }

    void realm::invalidate() {
        m_realm->verify_thread();
        m_realm->invalidate();
    }

    obj realm::import_copy_of(const obj& o) const {
        return m_realm->import_copy_of(o.operator Obj());
    }

    table realm::get_table(const uint32_t &key) {
        return m_realm->read_group().get_table(TableKey(key));
    }
}
