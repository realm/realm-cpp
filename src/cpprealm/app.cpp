#include <cpprealm/app.hpp>
#include <cpprealm/internal/generic_network_transport.hpp>

#ifndef REALMCXX_VERSION_MAJOR
#include <cpprealm/internal/version_numbers.hpp>
#endif

#include <realm/object-store/sync/app.hpp>
#include <realm/object-store/sync/sync_manager.hpp>
#include <realm/object-store/sync/sync_user.hpp>

#include <utility>

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

    user::user(std::shared_ptr<SyncUser> user) : m_user(std::move(user))
    {
    }

    /**
     The unique MongoDB Realm string identifying this user.
     Note this is different from an identitiy: A user may have multiple identities but has a single indentifier. See RLMUserIdentity.
     */
    [[nodiscard]] std::string user::identifier() const
    {
        return m_user->identity();
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
        m_user->sync_manager()->app().lock()->log_out(m_user, [cb = std::move(callback)](auto error) {
            cb(error ? std::optional<app_error>{app_error(std::move(*error))} : std::nullopt);
        });
    }

    std::future<void> user::log_out() const
    {
        std::promise<void> p;
        std::future<void> f = p.get_future();
        m_user->sync_manager()->app().lock()->log_out(m_user, [p = std::move(p)](auto err) mutable {
            if (err) {
                p.set_exception(std::make_exception_ptr(app_error(std::move(*err))));
            } else {
                p.set_value();
            }
        });
        return f;
    }

    /**
     The custom data of the user.
     This is configured in your Atlas App Services app.
     */
    std::optional<bson::BsonDocument> user::custom_data() const
    {
        return m_user->custom_data();
    }

    /**
     Calls the Atlas App Services function with the provided name and arguments.

     @param name The name of the Atlas App Services function to be called.
     @param arguments The `BsonArray` of arguments to be provided to the function.
     @param callback The completion handler to call when the function call is complete.
     This handler is executed on the thread the method was called from.
     */
    void user::call_function(const std::string& name, const realm::bson::BsonArray& arguments,
                             std::function<void(std::optional<bson::Bson>&&, std::optional<app_error>)> callback) const
    {
        m_user->sync_manager()->app().lock()->call_function(name, arguments, std::move(callback));
    }

    /**
     Calls the Atlas App Services function with the provided name and arguments.

     @param name The name of the Atlas App Services function to be called.
     @param arguments The `BsonArray` of arguments to be provided to the function.
     @param callback The completion handler to call when the function call is complete.
     This handler is executed on the thread the method was called from.
     */
    std::future<std::optional<bson::Bson>> user::call_function(const std::string& name, const realm::bson::BsonArray& arguments) const
    {
        std::promise<std::optional<bson::Bson>> p;
        std::future<std::optional<bson::Bson>> f = p.get_future();
        m_user->sync_manager()->app().lock()->call_function(name, arguments, [p = std::move(p)](std::optional<bson::Bson>&& bson, std::optional<app_error> err) mutable {
            if (err) {
                p.set_exception(std::make_exception_ptr(app_error(std::move(*err))));
            } else {
                p.set_value(std::move(bson));
            }
        });
        return f;
    }

    /**
     Refresh a user's custom data. This will, in effect, refresh the user's auth session.
     */
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
        return m_user->sync_manager();
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
    App::credentials App::credentials::google(const auth_code& auth_code)
    {
        app::AuthCode code{static_cast<std::string>(auth_code)};
        return {app::AppCredentials::google(std::move(code))};
    }
    App::credentials App::credentials::google(const id_token& id_token)
    {
        app::IdToken code{static_cast<std::string>(id_token)};
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
    App::credentials App::credentials::function(const bson::BsonDocument& payload)
    {
        return credentials(app::AppCredentials::function(payload));
    }

    App::App(const configuration& config) {
#if QT_CORE_LIB
        util::Scheduler::set_default_factory(util::make_qt);
#endif
        SyncClientConfig client_config;

#if REALM_PLATFORM_APPLE
        bool should_encrypt = !getenv("REALM_DISABLE_METADATA_ENCRYPTION");
#else
        bool should_encrypt = config.metadata_encryption_key && !getenv("REALM_DISABLE_METADATA_ENCRYPTION");
#endif
        client_config.metadata_mode = should_encrypt ? SyncManager::MetadataMode::Encryption : SyncManager::MetadataMode::NoEncryption;
        if (config.metadata_encryption_key) {
            auto key = std::vector<char>();
            key.resize(64);
            key.assign(config.metadata_encryption_key->begin(), config.metadata_encryption_key->end());
            client_config.custom_encryption_key = std::move(key);
        }

#ifdef QT_CORE_LIB
        auto qt_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
        if (!std::filesystem::exists(qt_path)) {
            std::filesystem::create_directory(qt_path);
        }
        config.base_file_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
#else
        if (config.path) {
            client_config.base_file_path = *config.path;
        } else {
            client_config.base_file_path = std::filesystem::current_path().make_preferred().generic_string();
        }
#endif
        client_config.user_agent_binding_info = std::string("RealmCpp/") + std::string(REALMCXX_VERSION_STRING);
        client_config.user_agent_application_info = config.app_id;

        auto app_config = app::App::Config();
        app_config.app_id = config.app_id;
        app_config.transport = std::make_shared<internal::DefaultTransport>(config.custom_http_headers,
                                                                            config.proxy_configuration);
        app_config.base_url = config.base_url;
        auto device_info = app::App::Config::DeviceInfo();

        device_info.framework_name = "Realm Cpp",
        device_info.platform_version = "?",
        device_info.sdk_version = REALMCXX_VERSION_STRING,
        device_info.sdk = "Realm Cpp";
        app_config.device_info = std::move(device_info);

        m_app = app::App::get_shared_app(std::move(app_config), client_config);
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
        if (auto u = m_app->sync_manager()->get_current_user()) {
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

}