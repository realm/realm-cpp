#include <cpprealm/app.hpp>
#include <cpprealm/internal/generic_network_transport.hpp>

#include <realm/object-store/sync/app.hpp>
#include <realm/object-store/sync/sync_user.hpp>
#include <realm/object-store/sync/sync_manager.hpp>

#include <utility>

namespace realm::internal::bridge {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
    static_assert(LayoutCheck<storage::AppCredentials, ::realm::app::AppCredentials>{});
    static_assert(LayoutCheck<storage::AppError, ::realm::app::AppError>{});
#elif __i386__
    static_assert(SizeCheck<8, sizeof(realm::app::AppCredentials)>{});
    static_assert(SizeCheck<4, alignof(realm::app::AppCredentials)>{});
    static_assert(SizeCheck<28, sizeof(realm::app::AppError)>{});
    static_assert(SizeCheck<4, alignof(realm::app::AppError)>{});
#elif __x86_64__
    static_assert(SizeCheck<16, sizeof(realm::app::AppCredentials)>{});
    static_assert(SizeCheck<8, alignof(realm::app::AppCredentials)>{});
    #if defined(__clang__)
    static_assert(SizeCheck<48, sizeof(realm::app::AppError)>{});
    static_assert(SizeCheck<8, alignof(realm::app::AppError)>{});
    #elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<56, sizeof(realm::app::AppError)>{});
    static_assert(SizeCheck<8, alignof(realm::app::AppError)>{});
    #endif
#elif __arm__
    static_assert(SizeCheck<8, sizeof(realm::app::AppCredentials)>{});
    static_assert(SizeCheck<4, alignof(realm::app::AppCredentials)>{});
    static_assert(SizeCheck<28, sizeof(realm::app::AppError)>{});
    static_assert(SizeCheck<4, alignof(realm::app::AppError)>{});
#elif __aarch64__
    static_assert(SizeCheck<16, sizeof(realm::app::AppCredentials)>{});
    static_assert(SizeCheck<8, alignof(realm::app::AppCredentials)>{});
#if defined(__clang__)
    static_assert(SizeCheck<48, sizeof(realm::app::AppError)>{});
#elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<56, sizeof(realm::app::AppError)>{});
#endif
    static_assert(SizeCheck<8, alignof(realm::app::AppError)>{});
#elif _WIN32
    #if _DEBUG
    static_assert(SizeCheck<80, sizeof(realm::app::AppError)>{});
    #else
    static_assert(SizeCheck<72, sizeof(realm::app::AppError)>{});
    #endif
    static_assert(SizeCheck<16, sizeof(realm::app::AppCredentials)>{});
    static_assert(SizeCheck<8, alignof(realm::app::AppCredentials)>{});
    
    static_assert(SizeCheck<8, alignof(realm::app::AppError)>{});
#endif
} // namespace realm::internal::bridge

namespace realm {
    static_assert((int)user::state::logged_in == (int)SyncUser::State::LoggedIn);
    static_assert((int)user::state::logged_out == (int)SyncUser::State::LoggedOut);
    static_assert((int)user::state::removed == (int)SyncUser::State::Removed);

    app_error::app_error(const app_error& other) {
        new (&m_error) app::AppError(*reinterpret_cast<const app::AppError*>(&other.m_error));
    }

    app_error& app_error::operator=(const app_error& other) {
        if (this != &other) {
            *reinterpret_cast<app::AppError*>(&m_error) = *reinterpret_cast<const app::AppError*>(&other.m_error);
        }
        return *this;
    }

    app_error::app_error(app_error&& other) {
        new (&m_error) app::AppError(std::move(*reinterpret_cast<app::AppError*>(&other.m_error)));
    }

    app_error& app_error::operator=(app_error&& other) {
        if (this != &other) {
            *reinterpret_cast<app::AppError*>(&m_error) = std::move(*reinterpret_cast<app::AppError*>(&other.m_error));
        }
        return *this;
    }

    app_error::~app_error() {
        reinterpret_cast<app::AppError*>(&m_error)->~AppError();
    }

    app_error::app_error(realm::app::AppError&& error) {
        new (&m_error) app::AppError(std::move(error));
    }

    std::string_view app_error::mesage() const
    {
        return reinterpret_cast<const app::AppError *>(&m_error)->reason();
    }

    std::string_view app_error::link_to_server_logs() const
    {
        return reinterpret_cast<const app::AppError*>(&m_error)->link_to_server_logs;
    }

    bool app_error::is_json_error() const
    {
        return reinterpret_cast<const app::AppError*>(&m_error)->is_json_error();
    }

    bool app_error::is_service_error() const
    {
        return reinterpret_cast<const app::AppError*>(&m_error)->is_service_error();
    }

    bool app_error::is_http_error() const
    {
        return reinterpret_cast<const app::AppError*>(&m_error)->is_http_error();
    }

    bool app_error::is_custom_error() const
    {
        return reinterpret_cast<const app::AppError*>(&m_error)->is_custom_error();
    }

    bool app_error::is_client_error() const
    {
        return reinterpret_cast<const app::AppError*>(&m_error)->is_client_error();
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
            if (err) p.set_exception(std::make_exception_ptr(*err));
            else p.set_value();
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
            if (err) p.set_exception(std::make_exception_ptr(*err));
            else p.set_value(std::move(bson));
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
            if (err) p.set_exception(std::make_exception_ptr(*err));
            else p.set_value();
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
        new (&m_credentials) app::AppCredentials();
    }

    App::credentials::credentials(const credentials& other) {
        new (&m_credentials) app::AppCredentials(*reinterpret_cast<const app::AppCredentials*>(&other.m_credentials));
    }

    App::credentials& App::credentials::operator=(const credentials& other) {
        if (this != &other) {
            *reinterpret_cast<app::AppCredentials*>(&m_credentials) = *reinterpret_cast<const app::AppCredentials*>(&other.m_credentials);
        }
        return *this;
    }

    App::credentials::credentials(credentials&& other) {
        new (&m_credentials) app::AppCredentials(std::move(*reinterpret_cast<app::AppCredentials*>(&other.m_credentials)));
    }

    App::credentials& App::credentials::operator=(App::credentials&& other) {
        if (this != &other) {
            *reinterpret_cast<app::AppCredentials*>(&m_credentials) = std::move(*reinterpret_cast<app::AppCredentials*>(&other.m_credentials));
        }
        return *this;
    }

    App::credentials::~credentials() {
        reinterpret_cast<app::AppCredentials*>(&m_credentials)->~AppCredentials();
    }

    App::credentials::credentials(app::AppCredentials &&v) noexcept {
        new (&m_credentials) app::AppCredentials(std::move(v));
    }
    App::credentials::operator app::AppCredentials() const {
        return *reinterpret_cast<const app::AppCredentials*>(&m_credentials);
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

    App::App(const std::string &app_id,
             const std::optional<std::string> &base_url,
             const std::optional<std::string> &path,
             const std::optional<std::map<std::string, std::string>> &custom_http_headers) {
#if QT_CORE_LIB
        util::Scheduler::set_default_factory(util::make_qt);
#endif
        SyncClientConfig config;
        bool should_encrypt = !getenv("REALM_DISABLE_METADATA_ENCRYPTION");
#if REALM_DISABLE_METADATA_ENCRYPTION
        config.metadata_mode = SyncManager::MetadataMode::NoEncryption;
#else
        config.metadata_mode = SyncManager::MetadataMode::NoEncryption;
#endif
#ifdef QT_CORE_LIB
        auto qt_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
        if (!std::filesystem::exists(qt_path)) {
            std::filesystem::create_directory(qt_path);
        }
        config.base_file_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
#else
        if (path) {
            config.base_file_path = *path;
        } else {
            config.base_file_path = std::filesystem::current_path().make_preferred().generic_string();
        }
#endif
        config.user_agent_binding_info = "RealmCpp/0.0.1";
        config.user_agent_application_info = app_id;

        auto app_config = app::App::Config();
        app_config.app_id = app_id;
        app_config.transport = std::make_shared<internal::DefaultTransport>(custom_http_headers);
        app_config.base_url = base_url ? base_url : util::Optional<std::string>();
        auto device_info = app::App::Config::DeviceInfo();

        device_info.framework_name = "Realm Cpp",
        device_info.platform_version = "?",
        device_info.sdk_version = "0.0.1",
        device_info.sdk = "Realm Cpp";
        app_config.device_info = std::move(device_info);

        m_app = app::App::get_shared_app(std::move(app_config), config);
    }

    std::future<void> App::register_user(const std::string &username, const std::string &password) {
        std::promise<void> p;
        std::future<void> f = p.get_future();
        m_app->template provider_client<app::App::UsernamePasswordProviderClient>().register_email(username,
                                                                                                   password,
                                                                                                   [p = std::move(p)](auto err) mutable {
                                                                                                       if (err) p.set_exception(std::make_exception_ptr(*err));
                                                                                                       else p.set_value();
                                                                                                   });
        return f;
    }
    std::future<user> App::login(const credentials& credentials) {
        std::promise<user> p;
        std::future<user> f = p.get_future();
        m_app->log_in_with_credentials(credentials, [p = std::move(p)](auto& u, auto err) mutable {
            if (err) p.set_exception(std::make_exception_ptr(*err));
            else p.set_value(user{std::move(u)});
        });
        return f;
    }

    void App::login(const credentials& credentials, std::function<void(user, std::optional<app_error>)>&& callback) {
        m_app->log_in_with_credentials(
                credentials,
                [cb = std::move(callback)](auto &u, auto error) {
                    cb(user{std::move(u)}, error ? std::optional<app_error>{app_error(std::move(*error))} : std::nullopt);
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
}