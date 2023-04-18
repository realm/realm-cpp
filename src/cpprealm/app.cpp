#include <cpprealm/app.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/object-store/sync/app.hpp>
#include <realm/object-store/sync/app_credentials.hpp>
#include <realm/object-store/sync/sync_user.hpp>

#include <utility>

namespace realm {
    static_assert((int)user::state::logged_in == (int)SyncUser::State::LoggedIn);
    static_assert((int)user::state::logged_out == (int)SyncUser::State::LoggedOut);
    static_assert((int)user::state::removed == (int)SyncUser::State::Removed);

    app_error::app_error(realm::app::AppError&& error) {
        m_error = std::make_shared<realm::app::AppError>(app::AppError(std::move(error)));
    }

    std::string_view app_error::mesage() const
    {
        return m_error->reason();
    }

    std::string_view app_error::link_to_server_logs() const
    {
        return m_error->link_to_server_logs;
    }

    bool app_error::is_json_error() const
    {
        return m_error->is_json_error();
    }

    bool app_error::is_service_error() const
    {
        return m_error->is_service_error();
    }

    bool app_error::is_http_error() const
    {
        return m_error->is_http_error();
    }

    bool app_error::is_custom_error() const
    {
        return m_error->is_custom_error();
    }

    bool app_error::is_client_error() const
    {
        return m_error->is_client_error();
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

    std::promise<void> user::log_out() const
    {
        std::promise<void> p;
        m_user->sync_manager()->app().lock()->log_out(m_user, [&p](auto err){
            if (err) p.set_exception(std::make_exception_ptr(*err));
            else p.set_value();
        });
        return p;
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
    std::promise<std::optional<bson::Bson>> user::call_function(const std::string& name, const realm::bson::BsonArray& arguments) const
    {
        std::promise<std::optional<bson::Bson>> p;
        m_user->sync_manager()->app().lock()->call_function(name, arguments, [&p](std::optional<bson::Bson>&& bson, std::optional<app_error> err) {
            if (err) p.set_exception(std::make_exception_ptr(*err));
            else p.set_value(std::move(bson));
        });
        return p;
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
    [[nodiscard]] std::promise<void> user::refresh_custom_user_data() const
    {
        std::promise<void> p;
        m_user->refresh_custom_data([&p](auto err){
            if (err) p.set_exception(std::make_exception_ptr(*err));
            else p.set_value();
        });
        return p;
    }

    internal::bridge::sync_manager user::sync_manager() const {
        return m_user->sync_manager();
    }

    App::credentials::credentials(app::AppCredentials &&v) noexcept {
        m_credentials = std::make_shared<app::AppCredentials>(app::AppCredentials(std::move(v)));
    }
    App::credentials::operator app::AppCredentials() const {
        return *m_credentials;
    }

    App::credentials App::credentials::anonymous()
    {
        return App::credentials(app::AppCredentials::anonymous());
    }
    App::credentials App::credentials::api_key(const std::string& key)
    {
        return credentials(app::AppCredentials::user_api_key(key));
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

    App::App(const std::string &app_id, const std::optional<std::string> &base_url, const std::optional<std::string> &path) {
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
        if (path)
            config.base_file_path = *path;
        else
            config.base_file_path = std::filesystem::current_path();
#endif
        config.user_agent_binding_info = "RealmCpp/0.0.1";
        config.user_agent_application_info = app_id;

        m_app = app::App::get_shared_app(app::App::Config{
                                                 .app_id = app_id,
                                                 .transport = std::make_shared<internal::DefaultTransport>(),
                                                 .base_url = base_url ? base_url : util::Optional<std::string>(),
                                                 .device_info = {
                                                         .platform = "Realm Cpp",
                                                         .platform_version = "?",
                                                         .sdk_version = "0.0.1",
                                                         .sdk = "Realm Cpp"
                                                 }},
                                         config);
    }


    std::promise<void> App::register_user(const std::string& username, const std::string& password) {
        std::promise<void> p;
        m_app->template provider_client<app::App::UsernamePasswordProviderClient>().register_email(username,
                                                                                                   password,
                                                                                                   [&p](auto err){
                                                                                                       if (err) p.set_exception(std::make_exception_ptr(*err));
                                                                                                       else p.set_value();
                                                                                                   });
        return p;
    }
    std::promise<user> App::login(const credentials& credentials) {
        std::promise<user> p;
        m_app->log_in_with_credentials(credentials, [&p](auto& u, auto err) {
            if (err) p.set_exception(std::make_exception_ptr(*err));
            else p.set_value(user{std::move(u)});
        });
        return p;
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

}
