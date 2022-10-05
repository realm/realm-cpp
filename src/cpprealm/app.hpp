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
#include <utility>
#include <future>

namespace realm {

namespace internal {

class DefaultTransport : public app::GenericNetworkTransport {
public:
    void send_request_to_server(app::Request&& request,
                                app::HttpCompletion&& completion_block) override;
};

} // namespace internal


// MARK: User
template <typename ...Ts>
static inline std::promise<thread_safe_reference<db<Ts...>>> async_open(const db_config& config);
    /**
 A `User` instance represents a single Realm App user account.

 A user may have one or more credentials associated with it. These credentials
 uniquely identify the user to the authentication provider, and are used to sign
 into a MongoDB Realm user account.

 Note that user objects are only vended out via SDK APIs, and cannot be directly
 initialized. User objects can be accessed from any thread.
 */
struct User {
    User() = default;
    User(const User&) = default;
    User(User&&) = default;
    User& operator=(const User&) = default;
    User& operator=(User&&) = default;
    explicit User(std::shared_ptr<SyncUser> user)
    : m_user(std::move(user))
    {
    }

    /**
     The unique MongoDB Realm string identifying this user.
     Note this is different from an identitiy: A user may have multiple identities but has a single indentifier. See RLMUserIdentity.
     */
    std::string identifier() const
    {
       return m_user->identity();
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

    /**
     Open a partition-based realm instance for the given url.

     @param partition_value The  value the Realm is partitioned on.
     @return A `thread_safe_reference` to the synchronized db.
     */
    template <typename ...Ts, typename T>
    std::promise<thread_safe_reference<db<Ts...>>> realm(const T& partition_value) const;

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
    
    std::shared_ptr<SyncUser> m_user;
};

class App {
    static std::unique_ptr<util::Logger> defaultSyncLogger(util::Logger::Level level) {
        struct SyncLogger : public util::RootLogger {
            void do_log(Level, const std::string& message) override {
//                std::cerr<<"sync: "<<message<<std::endl;
            }
        };
        auto logger = std::make_unique<SyncLogger>();
        logger->set_level_threshold(level);
        return std::move(logger);
    }
public:
    explicit App(const std::string& app_id, const std::optional<std::string>& base_url = {})
    {
        #if QT_CORE_LIB
        util::Scheduler::set_default_factory(util::make_qt);
        #endif
        SyncClientConfig config;
        bool should_encrypt = !getenv("REALM_DISABLE_METADATA_ENCRYPTION");
        config.logger_factory = defaultSyncLogger;
        #if REALM_DISABLE_METADATA_ENCRYPTION
        config.metadata_mode = SyncManager::MetadataMode::NoEncryption;
        #else
        config.metadata_mode = SyncManager::MetadataMode::Encryption;
        #endif
        #ifdef QT_CORE_LIB
        auto qt_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
        if (!std::filesystem::exists(qt_path)) {
            std::filesystem::create_directory(qt_path);
        }
        config.base_file_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
        #else
        config.base_file_path = std::filesystem::current_path();
        #endif
        config.user_agent_binding_info = "RealmCpp/0.0.1";
        config.user_agent_application_info = app_id;

        m_app = app::App::get_shared_app(app::App::Config{
            .app_id=app_id,
            .transport = std::make_shared<internal::DefaultTransport>(),
            .base_url = base_url ? base_url : util::Optional<std::string>(),
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
        static Credentials facebook(const std::string access_token)
        {
            return Credentials(app::AppCredentials::facebook(access_token));
        }
        static Credentials apple(const std::string id_token)
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
        static Credentials custom(const std::string token)
        {
            return Credentials(app::AppCredentials::custom(token));
        }
        static Credentials username_password(const std::string username, const std::string password)
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
    m_app->log_in_with_credentials(credentials.m_credentials, [&p](auto& user, auto err) {
        if (err) p.set_exception(std::make_exception_ptr(*err));
        else p.set_value(User{std::move(user)});
    });
    return p;
}
#endif
private:
    std::shared_ptr<app::App> m_app;
};

// MARK: Impl

template <typename ...Ts, typename T>
std::promise<thread_safe_reference<db<Ts...>>> User::realm(const T& partition_value) const
{
    db_config config;
    config.sync_config = std::make_shared<SyncConfig>(m_user, bson::Bson(partition_value));
    config.sync_config->error_handler = [](const std::shared_ptr<SyncSession>& session, const SyncError& error) {
        std::cerr<<"sync error: "<<error.message<<std::endl;
    };
    config.path = m_user->sync_manager()->path_for_realm(*config.sync_config);
    config.sync_config->client_resync_mode = realm::ClientResyncMode::Manual;
    config.sync_config->stop_policy = SyncSessionStopPolicy::AfterChangesUploaded;
    return async_open<Ts...>(std::move(config));
}

}
#endif /* Header_h */
