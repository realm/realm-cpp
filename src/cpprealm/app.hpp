////////////////////////////////////////////////////////////////////////////
//
// Copyright 2022 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef realm_app_hpp
#define realm_app_hpp

#include <realm/object-store/sync/app.hpp>
#include <realm/object-store/sync/sync_user.hpp>
#include <realm/object-store/sync/impl/sync_client.hpp>

#include <cpprealm/type_info.hpp>
#include <cpprealm/task.hpp>
#include <cpprealm/db.hpp>
#include <cpprealm/internal/generic_network_transport.hpp>

#include <utility>
#include <future>

namespace realm {
// MARK: User
template <typename ...Ts>
static inline std::promise<thread_safe_reference<db<Ts...>>> async_open(const db_config& config);

// Represents an error state from the server.
struct app_error {

    app_error(realm::app::AppError error) : m_error(error)
    {
    }

    std::error_code error_code() const
    {
        return m_error.error_code;
    }

    std::string_view mesage() const
    {
        return m_error.message;
    }

    std::string_view link_to_server_logs() const
    {
        return m_error.link_to_server_logs;
    }

    bool is_json_error() const
    {
        return error_code().category() == realm::app::json_error_category();
    }

    bool is_service_error() const
    {
        return error_code().category() == realm::app::service_error_category();
    }

    bool is_http_error() const
    {
        return error_code().category() == realm::app::http_error_category();
    }

    bool is_custom_error() const
    {
        return error_code().category() == realm::app::custom_error_category();
    }

    bool is_client_error() const
    {
        return error_code().category() == realm::app::client_error_category();
    }
private:
    realm::app::AppError m_error;
};

/**
 A `User` instance represents a single Realm App user account.

 A user may have one or more credentials associated with it. These credentials
 uniquely identify the user to the authentication provider, and are used to sign
 into a MongoDB Realm user account.

 User objects can be accessed from any thread.
 */
struct User {
    User() = default;
    User(const User&) = default;
    User(User&&) = default;
    User& operator=(const User&) = default;
    User& operator=(User&&) = default;
    explicit User(std::shared_ptr<SyncUser> user, std::shared_ptr<app::App> app)
    : m_user(std::move(user)), m_app(std::move(app))
    {
    }

    /**
     The state of the user object.
     */
    enum class state : uint8_t {
        logged_out,
        logged_in,
        removed,
    };

    /**
     The unique MongoDB Realm string identifying this user.
     Note this is different from an identitiy: A user may have multiple identities but has a single indentifier. See RLMUserIdentity.
     */
    std::string identifier() const
    {
       return m_user->identity();
    }

    /**
     The current state of the user.
     */
    state state() const
    {
        return static_cast<enum state>(m_user->state());
    }

    /**
     The user's refresh token used to access the Realm Application.

     This is required to make HTTP requests to MongoDB Realm's REST API
     for functionality not exposed natively. It should be treated as sensitive data.
     */
    std::string access_token() const
    {
        return m_user->access_token();
    }

    /**
     The user's refresh token used to access the Realm Applcation.

     This is required to make HTTP requests to the Realm App's REST API
     for functionality not exposed natively. It should be treated as sensitive data.
     */
    std::string refresh_token() const
    {
        return m_user->refresh_token();
    }

    db_config flexible_sync_configuration() const
    {
        db_config config;
        config.sync_config = std::make_shared<SyncConfig>(m_user, SyncConfig::FLXSyncEnabled{});
        config.sync_config->error_handler = [](std::shared_ptr<SyncSession> session, SyncError error) {
            std::cerr<<"sync error: "<<error.message<<std::endl;
        };
        config.path = m_user->sync_manager()->path_for_realm(*config.sync_config);
        config.sync_config->client_resync_mode = realm::ClientResyncMode::Manual;
        config.sync_config->stop_policy = SyncSessionStopPolicy::AfterChangesUploaded;
        return config;
    }

#if __cpp_coroutines
    /**
     Logs out the current user

     The users state will be set to `Removed` is they are an anonymous user or `LoggedOut`
     if they are authenticated by an email / password or third party auth clients
     If the logout request fails, this method will still clear local authentication state.
    */
    task<void> log_out() {
        try {
            auto error = co_await make_awaitable<util::Optional<app_error>>([&](auto cb) {
                m_app->log_out(m_user, cb);
            });
            if (error) {
                throw *error;
            }
        } catch (std::exception& err) {
            throw;
        }

        co_return;
    }
#endif

    /**
     Logs out the current user

     The users state will be set to `Removed` is they are an anonymous user or `LoggedOut`
     if they are authenticated by an email / password or third party auth clients
     If the logout request fails, this method will still clear local authentication state.
     */
    void log_out(util::UniqueFunction<void(std::optional<app_error>)>&& callback)
    {
        m_app->log_out(m_user, [cb = std::move(callback)](auto error) {
            cb(error ? std::optional<app_error>{app_error(*error)} : std::nullopt);
        });
    }

    std::promise<void> log_out()
    {
      std::promise<void> p;
      m_app->log_out(m_user, [&p](auto err){
                     if (err) p.set_exception(std::make_exception_ptr(*err));
                     else p.set_value();
      });
      return p;
    }

    /**
     The custom data of the user.
     This is configured in your Atlas App Services app.
     */
    std::optional<bson::BsonDocument> custom_data()
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
    void call_function(const std::string& name, const realm::bson::BsonArray& arguments,
                       util::UniqueFunction<void(std::optional<bson::Bson>&&, std::optional<app_error>)> callback)
    {
        m_app->call_function(name, arguments, std::move(callback));
    }

   /**
    Calls the Atlas App Services function with the provided name and arguments.

    @param name The name of the Atlas App Services function to be called.
    @param arguments The `BsonArray` of arguments to be provided to the function.
    @param callback The completion handler to call when the function call is complete.
    This handler is executed on the thread the method was called from.
    */
    std::promise<std::optional<bson::Bson>> call_function(const std::string& name, const realm::bson::BsonArray& arguments)
    {
      std::promise<std::optional<bson::Bson>> p;
      m_app->call_function(name, arguments, [&p](std::optional<bson::Bson>&& bson, std::optional<app_error> err) {
          if (err) p.set_exception(std::make_exception_ptr(*err));
          else p.set_value(std::move(bson));
      });
      return p;
    }

#if __cpp_coroutines
    /**
     Calls the Atlas App Services function with the provided name and arguments.

     @param name The name of the Atlas App Services function to be called.
     @param arguments The `BsonArray` of arguments to be provided to the function.
     @returns An optional Bson object containing the servers response.
     */
    task<std::optional<bson::Bson>> call_function(const std::string& name, const realm::bson::BsonArray& arguments)
    {
        try {
            auto opt_bson = co_await make_awaitable<util::Optional<bson::Bson>>( [&](auto cb) {
                m_app->call_function(m_user, name, arguments, std::move(cb));
            });
            co_return opt_bson;
        } catch (std::exception& err) {
            throw;
        }
    }
#endif

    /**
     Refresh a user's custom data. This will, in effect, refresh the user's auth session.
     */
    void refresh_custom_user_data(util::UniqueFunction<void(std::optional<app_error>)> callback)
    {
        m_user->refresh_custom_data(std::move(callback));
    }

    /**
     Refresh a user's custom data. This will, in effect, refresh the user's auth session.
     */
    std::promise<void> refresh_custom_user_data()
    {
        std::promise<void> p;
        m_user->refresh_custom_data([&p](auto err){
            if (err) p.set_exception(std::make_exception_ptr(*err));
            else p.set_value();
        });
        return p;
    }

#if __cpp_coroutines
    /**
     Refresh a user's custom data. This will, in effect, refresh the user's auth session.
     */
    task<void> refresh_custom_user_data()
    {
        try {
            auto error = co_await make_awaitable<std::optional<app_error>>( [&](auto cb) {
                m_user->refresh_custom_data(std::move(cb));
            });
            if (error) {
                throw *error;
            }
        } catch (std::exception& err) {
            throw;
        }
    }
#endif

    std::shared_ptr<app::App> m_app;
    std::shared_ptr<SyncUser> m_user;
};
static_assert((int)User::state::logged_in  == (int)SyncUser::State::LoggedIn);
static_assert((int)User::state::logged_out == (int)SyncUser::State::LoggedOut);
static_assert((int)User::state::removed    == (int)SyncUser::State::Removed);

class App {
    static std::unique_ptr<util::Logger> defaultSyncLogger(util::Logger::Level level) {
    struct SyncLogger : public util::Logger {
            void do_log(Level level, const std::string& message) override {
                std::string copy = message;
                switch (level) {
                    case Level::all:
                    case Level::warn:
                        std::cout<<"sync: "<<copy<<std::endl;
                        break;
                    case Level::error:
                        std::cerr<<"sync: "<<copy<<std::endl;
                        break;
                    default:
                        break;
                }
            }
        };
        auto logger = std::make_unique<SyncLogger>();
        logger->set_level_threshold(level);
        return std::move(logger);
    }
public:
    explicit App(const std::string& app_id,
                 const std::optional<std::string>& base_url = std::nullopt,
                 const std::optional<std::string>& file_path = std::nullopt)
    {
        #if QT_CORE_LIB
        util::Scheduler::set_default_factory(util::make_qt);
        #endif
        SyncClientConfig config;
        config.logger_factory = defaultSyncLogger;
        // TODO: Add metadata encryption support.
        config.metadata_mode = SyncManager::MetadataMode::NoEncryption;
        #ifdef QT_CORE_LIB
        auto qt_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
        if (!std::filesystem::exists(qt_path)) {
            std::filesystem::create_directory(qt_path);
        }
        config.base_file_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
        #else
        if (file_path)
            config.base_file_path = *file_path;
        else
            config.base_file_path = std::filesystem::current_path();
        #endif
        config.user_agent_binding_info = "RealmCpp/0.0.1";
        config.user_agent_application_info = app_id;

        m_app = app::App::get_shared_app(app::App::Config{
            .app_id=app_id,
            .transport = std::make_shared<internal::DefaultTransport>(),
            .base_url = base_url ? base_url : std::nullopt,
            .platform="Realm Cpp",
            .platform_version="?",
            .sdk_version="0.0.1",
        }, config);
    }

    struct Credentials {
        static Credentials anonymous()
        {
            return Credentials(app::AppCredentials::anonymous());
        }
        static Credentials api_key(const std::string& key)
        {
            return Credentials(app::AppCredentials::user_api_key(key));
        }
        static Credentials facebook(const std::string& access_token)
        {
            return Credentials(app::AppCredentials::facebook(access_token));
        }
        static Credentials apple(const std::string& id_token)
        {
            return Credentials(app::AppCredentials::apple(id_token));
        }
        static Credentials google(app::AuthCode auth_code)
        {
            return Credentials(app::AppCredentials::google(std::move(auth_code)));
        }
        static Credentials google(app::IdToken id_token)
        {
            return Credentials(app::AppCredentials::google(std::move(id_token)));
        }
        static Credentials custom(const std::string& token)
        {
            return Credentials(app::AppCredentials::custom(token));
        }
        static Credentials username_password(const std::string& username, const std::string& password)
        {
            return Credentials(app::AppCredentials::username_password(username, password));
        }
        static Credentials function(const bson::BsonDocument& payload)
        {
            return Credentials(app::AppCredentials::function(payload));
        }
        Credentials() = delete;
        Credentials(const Credentials& credentials) = default;
        Credentials(Credentials&&) = default;
    private:
        explicit Credentials(app::AppCredentials&& credentials)
        : m_credentials(credentials)
        {
        }
        friend class App;
        app::AppCredentials m_credentials;
    };

#if __cpp_coroutines
    task<void> register_user(const std::string username, const std::string password) {
        auto error = co_await make_awaitable<util::Optional<app::AppError>>([&](auto cb) {
            m_app->template provider_client<app::App::UsernamePasswordProviderClient>().register_email(username,
                                                                                                       password,
                                                                                                       cb);
        });
        if (error) {
            throw *error;
        }
        co_return;
    }

    task<User> login(const Credentials& credentials) {
        auto user = co_await make_awaitable<std::shared_ptr<SyncUser>>([this, credentials](auto cb) {
            m_app->log_in_with_credentials(credentials.m_credentials, cb);
        });
        co_return std::move(User{std::move(user)});
    }
#else
std::promise<void> register_user(const std::string& username, const std::string& password) {
    std::promise<void> p;
    m_app->template provider_client<app::App::UsernamePasswordProviderClient>().register_email(username,
                                                                                               password,
                                                                                               [&p](auto err){
        if (err) p.set_exception(std::make_exception_ptr(*err));
        else p.set_value();
    });
    return p;
}
std::promise<User> login(const Credentials& credentials) {
    std::promise<User> p;
    m_app->log_in_with_credentials(credentials.m_credentials, [&p, this](auto& user, auto err) {
        if (err) p.set_exception(std::make_exception_ptr(*err));
        else p.set_value(User{std::move(user), m_app});
    });
    return p;
}

void login(const Credentials& credentials, util::UniqueFunction<void(User, std::optional<app_error>)>&& callback) {
  m_app->log_in_with_credentials(
      credentials.m_credentials,
      [cb = std::move(callback), this](auto &u, auto error) {
        cb(User{std::move(u), m_app}, error ? std::optional<app_error>{app_error(*error)} : std::nullopt);
      });
}
#endif
private:
    std::shared_ptr<app::App> m_app;
};

}
#endif /* Header_h */
