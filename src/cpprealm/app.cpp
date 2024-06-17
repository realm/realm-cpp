#include <cpprealm/app.hpp>
#include <cpprealm/internal/bridge/status.hpp>
#include <cpprealm/internal/generic_network_transport.hpp>
#include <cpprealm/networking/platform_networking.hpp>

#ifndef REALMCXX_VERSION_MAJOR
#include <cpprealm/version_numbers.hpp>
#endif

#include <realm/object-store/sync/app.hpp>
#include <realm/object-store/sync/app_user.hpp>
#include <realm/object-store/sync/sync_manager.hpp>
#include <realm/object-store/sync/sync_user.hpp>
#include <realm/sync/config.hpp>
#include <realm/util/bson/bson.hpp>
#include <realm/util/platform_info.hpp>

#include <utility>

#if defined(REALM_AOSP_VENDOR)
#include <unistd.h>
#endif

namespace realm {
    static_assert((int)user::state::logged_in == (int)SyncUser::State::LoggedIn);
    static_assert((int)user::state::logged_out == (int)SyncUser::State::LoggedOut);
    static_assert((int)user::state::removed == (int)SyncUser::State::Removed);

    app_error::app_error(const app_error& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_error) app::AppError(*reinterpret_cast<const app::AppError*>(&other.m_error));
#else
        m_error = other.m_error;
#endif
    }

    app_error& app_error::operator=(const app_error& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<app::AppError*>(&m_error) = *reinterpret_cast<const app::AppError*>(&other.m_error);
        }
#else
        m_error = other.m_error;
#endif
        return *this;
    }

    app_error::app_error(app_error&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_error) app::AppError(std::move(*reinterpret_cast<app::AppError*>(&other.m_error)));
#else
        m_error = std::move(other.m_error);
#endif
    }

    app_error& app_error::operator=(app_error&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<app::AppError*>(&m_error) = std::move(*reinterpret_cast<app::AppError*>(&other.m_error));
        }
#else
        m_error = std::move(other.m_error);
#endif
        return *this;
    }

    app_error::~app_error() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<app::AppError*>(&m_error)->~AppError();
#endif
    }

    app_error::app_error(realm::app::AppError&& error) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_error) app::AppError(std::move(error));
#else
        m_error = std::make_shared<app::AppError>(std::move(error));
#endif
    }

    std::string_view app_error::message() const
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const app::AppError *>(&m_error)->reason();
#else
        return m_error->reason();
#endif
    }

    std::string_view app_error::link_to_server_logs() const
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const app::AppError*>(&m_error)->link_to_server_logs;
#else
        return m_error->link_to_server_logs;
#endif
    }

    bool app_error::is_json_error() const
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const app::AppError*>(&m_error)->is_json_error();
#else
        return m_error->is_json_error();
#endif
    }

    bool app_error::is_service_error() const
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const app::AppError*>(&m_error)->is_service_error();
#else
        return m_error->is_service_error();
#endif
    }

    bool app_error::is_http_error() const
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const app::AppError*>(&m_error)->is_http_error();
#else
        return m_error->is_http_error();
#endif
    }

    bool app_error::is_custom_error() const
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const app::AppError*>(&m_error)->is_custom_error();
#else
        return m_error->is_custom_error();
#endif
    }

    bool app_error::is_client_error() const
    {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const app::AppError*>(&m_error)->is_client_error();
#else
        return m_error->is_client_error();
#endif
    }

    user::user(std::shared_ptr<app::User> user) : m_user(std::move(user))
    {
    }

    /**
     The unique MongoDB Realm string identifying this user.
     Note this is different from an identitiy: A user may have multiple identities but has a single indentifier. See RLMUserIdentity.
     */
    [[nodiscard]] std::string user::identifier() const
    {
        return m_user->user_id();
    }

    /**
     The current state of the user.
     */
    [[nodiscard]] enum user::state user::state() const
    {
        return static_cast<enum state>(m_user->state());
    }

    /**
     The user's refresh token used to access the Realm Application.

     This is required to make HTTP requests to MongoDB Realm's REST API
     for functionality not exposed natively. It should be treated as sensitive data.
     */
    [[nodiscard]] std::string user::access_token() const
    {
        return m_user->access_token();
    }

    /**
     The user's refresh token used to access the Realm Applcation.

     This is required to make HTTP requests to the Realm App's REST API
     for functionality not exposed natively. It should be treated as sensitive data.
     */
    [[nodiscard]] std::string user::refresh_token() const
    {
        return m_user->refresh_token();
    }

    /**
     Logs out the current user

     The users state will be set to `Removed` is they are an anonymous user or `LoggedOut`
     if they are authenticated by an email / password or third party auth clients
     If the logout request fails, this method will still clear local authentication state.
     */
    void user::log_out(std::function<void(std::optional<app_error>)>&& callback) const
    {
        m_user->app()->log_out(m_user, [cb = std::move(callback)](auto error) {
            cb(error ? std::optional<app_error>{app_error(std::move(*error))} : std::nullopt);
        });
    }

    std::future<void> user::log_out() const
    {
        std::promise<void> p;
        std::future<void> f = p.get_future();
        m_user->app()->log_out(m_user, [p = std::move(p)](auto err) mutable {
            if (err) {
                p.set_exception(std::make_exception_ptr(app_error(std::move(*err))));
            } else {
                p.set_value();
            }
        });
        return f;
    }

    db_config user::flexible_sync_configuration() const
    {
        db_config config;
        config.set_sync_config(sync_config(m_user));
        config.sync_config().set_error_handler([](const sync_session&, const internal::bridge::sync_error& error) {
            std::cerr << "ADS C++ SDK: Sync Error: " << error.message() << "\n";
        });
        config.set_path(m_user->app()->path_for_realm(*config.sync_config().operator std::shared_ptr<SyncConfig>()));
        config.sync_config().set_stop_policy(realm::internal::bridge::realm::sync_session_stop_policy::after_changes_uploaded);
        config.set_schema_mode(realm::internal::bridge::realm::config::schema_mode::additive_discovered);
        return config;
    }

    std::optional<std::string> user::custom_data() const
    {
        if (auto v = m_user->custom_data()) {
            return bson::Bson(*v).to_string();
        } else {
            return std::nullopt;
        }
    }

    std::optional<bsoncxx::document> user::get_custom_data() const
    {
        if (auto v = m_user->custom_data()) {
            return bsoncxx::document(*v);
        } else {
            return std::nullopt;
        }
    }

    void user::call_function(const std::string& name, const std::string& arguments,
                             std::function<void(std::optional<std::string>, std::optional<app_error>)> callback) const
    {
        m_user->app()->call_function(m_user, name, arguments, std::nullopt, [cb = std::move(callback)](const std::string* bson_string,
                                                                                                                              std::optional<app_error> err) {
            cb(bson_string ? std::optional<std::string>(*bson_string) : std::nullopt, err);
        });
    }

    std::future<std::optional<std::string>> user::call_function(const std::string& name, const std::string& arguments) const
    {
        std::promise<std::optional<std::string>> p;
        std::future<std::optional<std::string>> f = p.get_future();

        m_user->app()->call_function(m_user, name, arguments, std::nullopt, [p = std::move(p)](const std::string* bson_string,
                                                                                                                      std::optional<app_error> err) mutable {
            if (err) {
                p.set_exception(std::make_exception_ptr(app_error(std::move(*err))));
            } else {
                p.set_value(bson_string ? std::optional<std::string>(*bson_string) : std::nullopt);
            }
        });
        return f;
    }

    void user::call_function(const std::string& name, const std::vector<bsoncxx>& args_bson,
                             std::function<void(std::optional<bsoncxx>, std::optional<app_error>)> callback) const
    {
        bson::BsonArray core_bson;
        for(auto& b : args_bson) {
            core_bson.push_back(b);
        }
        m_user->app()->call_function(m_user, name, core_bson, std::nullopt, [cb = std::move(callback)](util::Optional<bson::Bson>&& b,
                                                                                                                              std::optional<app_error> err) {
            cb(b ? std::optional<bsoncxx>(*b) : std::nullopt, err);
        });
    }

    std::future<std::optional<bsoncxx>> user::call_function(const std::string& name, const std::vector<bsoncxx>& args_bson) const
    {
        std::promise<std::optional<bsoncxx>> p;
        std::future<std::optional<bsoncxx>> f = p.get_future();
        bson::BsonArray core_bson;
        for(auto& b : args_bson) {
            core_bson.push_back(b);
        }

        m_user->app()->call_function(m_user, name, core_bson, std::nullopt, [p = std::move(p)](util::Optional<bson::Bson>&& b,
                                                                                                                      std::optional<app_error> err) mutable {
            if (err) {
                p.set_exception(std::make_exception_ptr(app_error(std::move(*err))));
            } else {
                p.set_value(b ? std::optional<bsoncxx>(*b) : std::nullopt);
            }
        });
        return f;
    }

    void user::refresh_custom_user_data(std::function<void(std::optional<app_error>)> callback)
    {
        m_user->refresh_custom_data(std::move(callback));
    }

    /**
     Refresh a user's custom data. This will, in effect, refresh the user's auth session.
     */
    [[nodiscard]] std::future<void> user::refresh_custom_user_data() const
    {
        std::promise<void> p;
        std::future<void> f = p.get_future();
        m_user->refresh_custom_data([p = std::move(p)](auto err) mutable {
            if (err) {
                p.set_exception(std::make_exception_ptr(app_error(std::move(*err))));
            } else {
                p.set_value();
            }
        });
        return f;
    }

    internal::bridge::sync_manager user::sync_manager() const {
        return std::shared_ptr<SyncManager>(m_user->sync_manager());
    }

    bool user::is_logged_in() const {
        return m_user->is_logged_in();
    }

    bool operator==(const user& lhs, const user& rhs) {
        return lhs.m_user == rhs.m_user;
    }

    bool operator!=(const user& lhs, const user& rhs) {
        return lhs.m_user != rhs.m_user;
    }

    App::credentials::credentials() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_credentials) app::AppCredentials();
#else
        m_credentials = std::make_shared<app::AppCredentials>();
#endif
    }

    App::credentials::credentials(const credentials& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_credentials) app::AppCredentials(*reinterpret_cast<const app::AppCredentials*>(&other.m_credentials));
#else
        m_credentials = other.m_credentials;
#endif
    }

    App::credentials& App::credentials::operator=(const credentials& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<app::AppCredentials*>(&m_credentials) = *reinterpret_cast<const app::AppCredentials*>(&other.m_credentials);
        }
#else
        m_credentials = other.m_credentials;
#endif
        return *this;
    }

    App::credentials::credentials(credentials&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_credentials) app::AppCredentials(std::move(*reinterpret_cast<app::AppCredentials*>(&other.m_credentials)));
#else
        m_credentials = std::move(other.m_credentials);
#endif
    }

    App::credentials& App::credentials::operator=(App::credentials&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<app::AppCredentials*>(&m_credentials) = std::move(*reinterpret_cast<app::AppCredentials*>(&other.m_credentials));
        }
#else
        m_credentials = std::move(other.m_credentials);
#endif
        return *this;
    }

    App::credentials::~credentials() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<app::AppCredentials*>(&m_credentials)->~AppCredentials();
#endif
    }

    App::credentials::credentials(app::AppCredentials &&v) noexcept {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_credentials) app::AppCredentials(std::move(v));
#else
        m_credentials = std::make_shared<app::AppCredentials>(std::move(v));
#endif
    }
    App::credentials::operator app::AppCredentials() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const app::AppCredentials*>(&m_credentials);
#else
        return *m_credentials;
#endif
    }

    App::credentials App::credentials::anonymous()
    {
        return App::credentials(app::AppCredentials::anonymous());
    }
    App::credentials App::credentials::api_key(const std::string& key)
    {
        return credentials(app::AppCredentials::api_key(key));
    }
    App::credentials App::credentials::facebook(const std::string& access_token)
    {
        return credentials(app::AppCredentials::facebook(access_token));
    }
    App::credentials App::credentials::apple(const std::string& id_token)
    {
        return {app::AppCredentials::apple(id_token)};
    }
    App::credentials App::credentials::google_auth_code(const std::string& auth_code)
    {
        app::AuthCode code{auth_code};
        return {app::AppCredentials::google(std::move(code))};
    }
    App::credentials App::credentials::google_id_token(const std::string& id_token)
    {
        app::IdToken code{id_token};
        return {app::AppCredentials::google(std::move(code))};
    }
    App::credentials App::credentials::custom(const std::string& token)
    {
        return credentials(app::AppCredentials::custom(token));
    }
    App::credentials App::credentials::username_password(const std::string& username, const std::string& password)
    {
        return credentials(app::AppCredentials::username_password(username, password));
    }
    App::credentials App::credentials::function(const std::string& payload)
    {
        return credentials(app::AppCredentials::function(payload));
    }

    struct DefaultSyncLogger : public ::realm::util::Logger {
        void do_log(const ::realm::util::LogCategory&, Level l, const std::string& message) {
            std::cout << "ADS C++ SDK: " << ::realm::util::Logger::level_to_string(l) << " : " << message << "\n";
        }
    };

    App::App(const configuration& config) {
#if QT_CORE_LIB
        util::Scheduler::set_default_factory(util::make_qt);
#endif
        auto app_config = app::AppConfig();

        SyncClientConfig client_config;
        client_config.logger_factory = ([](::realm::util::Logger::Level level) {
            auto logger = std::make_unique<::realm::util::ThreadSafeLogger>(std::make_shared<DefaultSyncLogger>());
            logger->set_level_threshold(level);
            return logger;
        });
        client_config.log_level = util::Logger::Level::off;

#if REALM_PLATFORM_APPLE
        bool should_encrypt = !getenv("REALM_DISABLE_METADATA_ENCRYPTION");
#else
        bool should_encrypt = config.metadata_encryption_key && !getenv("REALM_DISABLE_METADATA_ENCRYPTION");
#endif

#ifdef QT_CORE_LIB
        auto qt_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
        if (!std::filesystem::exists(qt_path)) {
            std::filesystem::create_directory(qt_path);
        }
        app_config.base_file_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
#else
        if (config.path) {
            app_config.base_file_path = *config.path;
        } else {
#if defined(REALM_AOSP_VENDOR)
            char cwd[PATH_MAX];
            getcwd(cwd, sizeof(cwd));
            app_config.base_file_path = cwd;
#else
            app_config.base_file_path = std::filesystem::current_path().make_preferred().generic_string();
#endif
        }
#endif
        client_config.user_agent_binding_info = std::string("RealmCpp/") + std::string(REALMCXX_VERSION_STRING);
        client_config.user_agent_application_info = config.app_id;
        app_config.app_id = config.app_id;

        if (config.websocket_event_handler) {
            auto websocket_provider = ::realm::internal::networking::default_sync_socket_provider_factory(util::Logger::get_default_logger(),
                                                                                                          client_config.user_agent_binding_info,
                                                                                                          client_config.user_agent_application_info,
                                                                                                          config.websocket_event_handler);
            client_config.socket_provider = websocket_provider;
        }
        if (config.sync_socket_provider) {
            client_config.socket_provider = ::realm::internal::networking::create_sync_socket_provider_shim(config.sync_socket_provider,
                                                                                                            config.websocket_event_handler);
        }

        if (config.http_transport_client) {
            app_config.transport = internal::networking::create_http_client_shim(config.http_transport_client);
        } else if (config.custom_http_headers || config.proxy_configuration) {
            app_config.transport = internal::networking::create_http_client_shim(std::make_shared<internal::networking::DefaultTransport>(config.custom_http_headers,
                                                                                                                                          config.proxy_configuration));
        }

        app_config.base_url = config.base_url;

        app_config.metadata_mode = should_encrypt ? app::AppConfig::MetadataMode::Encryption : app::AppConfig::MetadataMode::NoEncryption;
        if (config.metadata_encryption_key) {
            auto key = std::vector<char>();
            key.resize(64);
            key.assign(config.metadata_encryption_key->begin(), config.metadata_encryption_key->end());
            app_config.custom_encryption_key = std::move(key);
        }

        auto device_info = app::AppConfig::DeviceInfo();

        device_info.framework_name = "Realm Cpp",
        device_info.platform_version = "?",
        device_info.sdk_version = REALMCXX_VERSION_STRING,
        device_info.sdk = "Realm Cpp";
        app_config.device_info = std::move(device_info);

        app_config.sync_client_config = client_config;
        m_app = app::App::get_app(app::App::CacheMode::Enabled, std::move(app_config));
    }

    App::App(const std::string &app_id,
             const std::optional<std::string> &base_url,
             const std::optional<std::string> &path,
             const std::optional<std::map<std::string, std::string>> &custom_http_headers) {
        configuration c;
        c.app_id = app_id;
        c.base_url = base_url;
        c.path = path;
        c.custom_http_headers = custom_http_headers;
        *this = App(std::move(c));
    }

    std::string App::get_base_url() const {
        return m_app->get_base_url();
    }

    [[nodiscard]] std::string App::path_for_realm(const realm::sync_config& c) const {
        return m_app->path_for_realm(*c.operator std::shared_ptr<SyncConfig>());
    }

    std::future<void> App::register_user(const std::string &username, const std::string &password) {
        std::promise<void> p;
        std::future<void> f = p.get_future();
        m_app->template provider_client<app::App::UsernamePasswordProviderClient>()
                .register_email(username,
                               password,
                               [p = std::move(p)](auto err) mutable {
                                   if (err) {
                                       p.set_exception(std::make_exception_ptr(app_error(std::move(*err))));
                                   } else {
                                       p.set_value();
                                   }
                               });
        return f;
    }
    std::future<user> App::login(const credentials& credentials) {
        std::promise<user> p;
        std::future<user> f = p.get_future();
        m_app->log_in_with_credentials(credentials, [p = std::move(p)](auto& u, auto err) mutable {
            if (err) {
                p.set_exception(std::make_exception_ptr(app_error(std::move(*err))));
            } else {
                p.set_value(user{std::move(u)});
            }
        });
        return f;
    }

    void App::login(const credentials& credentials, std::function<void(user, std::optional<app_error>)>&& callback) {
        m_app->log_in_with_credentials(
                credentials,
                [cb = std::move(callback)](auto &u, auto error) {
                    if (error) {
                        cb(user(), std::optional<app_error>{app_error(std::move(*error))});
                    } else {
                        cb(user(u), std::nullopt);
                    }
                });
    }

    internal::bridge::sync_manager App::get_sync_manager() const {
        return m_app->sync_manager();
    }

    std::optional<user> App::get_current_user() const {
        if (auto u = m_app->current_user()) {
            return user(u);
        }
        return std::nullopt;
    }

    void App::clear_cached_apps() {
        m_app->clear_cached_apps();
    }
    std::optional<App> App::get_cached_app(const std::string& app_id, const std::optional<std::string>& base_url) {
        if (auto a = m_app->get_cached_app(app_id, base_url)) {
            return App(std::move(a));
        }
        return std::nullopt;
    }

#ifdef REALM_ENABLE_EXPERIMENTAL
    [[nodiscard]] std::future<void> App::update_base_url(std::string base_url) const {
        std::promise<void> p;
        std::future<void> f = p.get_future();
        m_app->update_base_url(base_url, ([p = std::move(p)](auto err) mutable {
            if (err) {
                p.set_exception(std::make_exception_ptr(app_error(std::move(*err))));
            } else {
                p.set_value();
            }
        }));
        return f;
    }
#endif
}
