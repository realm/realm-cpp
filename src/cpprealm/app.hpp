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

#ifndef CPP_REALM_APP_HPP
#define CPP_REALM_APP_HPP

#include <cpprealm/task.hpp>
#include <cpprealm/db.hpp>
#include <cpprealm/internal/generic_network_transport.hpp>
#include <cpprealm/internal/bridge/sync_session.hpp>
#include <cpprealm/internal/bridge/sync_error.hpp>
#include <cpprealm/internal/bridge/sync_manager.hpp>
#include <cpprealm/internal/bridge/realm.hpp>

#include <realm/object-store/util/bson/bson.hpp>
#include <realm/object-store/util/tagged_string.hpp>

#include <utility>
#include <future>

namespace realm {
    using sync_session = internal::bridge::sync_session;
    using sync_error = internal::bridge::sync_error;

    namespace app {
        struct AppError;
        struct AppCredentials;
    }
// MARK: User

// Represents an error state from the server.
struct app_error {
    app_error(realm::app::AppError&& error); //NOLINT(google-explicit-constructor)

    [[nodiscard]] std::string_view mesage() const;

    [[nodiscard]] std::string_view link_to_server_logs() const;

    [[nodiscard]] bool is_json_error() const;

    [[nodiscard]] bool is_service_error() const;

    [[nodiscard]] bool is_http_error() const;

    [[nodiscard]] bool is_custom_error() const;

    [[nodiscard]] bool is_client_error() const;
private:
#ifdef __i386__
    std::aligned_storage<28, 4>::type m_error[1];
#elif __x86_64__
    #if defined(__clang__)
std::aligned_storage<48, 8>::type m_error[1];
    #elif defined(__GNUC__) || defined(__GNUG__)
std::aligned_storage<56, 8>::type m_error[1];
    #endif
#elif __arm__
    std::aligned_storage<28, 4>::type m_error[1];
#elif __aarch64__
    std::aligned_storage<48, 8>::type m_error[1];
#endif
};

/**
 A `User` instance represents a single Realm App user account.

 A user may have one or more credentials associated with it. These credentials
 uniquely identify the user to the authentication provider, and are used to sign
 into a MongoDB Realm user account.

 User objects can be accessed from any thread.
 */
struct user {
    user() = default;
    user(const user&) = default;
    user(user&&) = default;
    user& operator=(const user&) = default;
    user& operator=(user&&) = default;
    explicit user(std::shared_ptr<SyncUser> user)
    : m_user(std::move(user))
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
    [[nodiscard]] std::string identifier() const;

    /**
     The current state of the user.
     */
    [[nodiscard]] state state() const;

    /**
     The user's refresh token used to access the Realm Application.

     This is required to make HTTP requests to MongoDB Realm's REST API
     for functionality not exposed natively. It should be treated as sensitive data.
     */
    [[nodiscard]] std::string access_token() const;

    /**
     The user's refresh token used to access the Realm Applcation.

     This is required to make HTTP requests to the Realm App's REST API
     for functionality not exposed natively. It should be treated as sensitive data.
     */
    [[nodiscard]] std::string refresh_token() const;

    struct internal::bridge::sync_manager sync_manager() const;

    [[nodiscard]] db_config flexible_sync_configuration() const
    {
        db_config config;
        config.set_sync_config(sync_config(m_user));
        config.sync_config().set_error_handler([](const sync_session& session, const sync_error& error) {
            std::cerr<<"sync error: "<<error.message()<<std::endl;
        });
        config.set_path(sync_manager().path_for_realm(config.sync_config()));
        config.sync_config().set_client_resync_mode(realm::internal::bridge::realm::client_resync_mode::Manual);
        config.sync_config().set_stop_policy(realm::internal::bridge::realm::sync_session_stop_policy::AfterChangesUploaded);
        return config;
    }

    /**
     Logs out the current user

     The users state will be set to `Removed` is they are an anonymous user or `LoggedOut`
     if they are authenticated by an email / password or third party auth clients
     If the logout request fails, this method will still clear local authentication state.
     */
    void log_out(std::function<void(std::optional<app_error>)>&& callback) const;

    [[nodiscard]] std::promise<void> log_out() const;

    /**
     The custom data of the user.
     This is configured in your Atlas App Services app.
     */
    [[nodiscard]] std::optional<bson::BsonDocument> custom_data() const;

    /**
     Calls the Atlas App Services function with the provided name and arguments.

     @param name The name of the Atlas App Services function to be called.
     @param arguments The `BsonArray` of arguments to be provided to the function.
     @param callback The completion handler to call when the function call is complete.
     This handler is executed on the thread the method was called from.
     */
    void call_function(const std::string& name, const realm::bson::BsonArray& arguments,
                       std::function<void(std::optional<bson::Bson>&&, std::optional<app_error>)> callback) const;

   /**
    Calls the Atlas App Services function with the provided name and arguments.

    @param name The name of the Atlas App Services function to be called.
    @param arguments The `BsonArray` of arguments to be provided to the function.
    @param callback The completion handler to call when the function call is complete.
    This handler is executed on the thread the method was called from.
    */
    [[nodiscard]] std::promise<std::optional<bson::Bson>> call_function(const std::string& name,
                                                                        const realm::bson::BsonArray& arguments) const;

    /**
     Refresh a user's custom data. This will, in effect, refresh the user's auth session.
     */
    void refresh_custom_user_data(std::function<void(std::optional<app_error>)> callback);

    /**
     Refresh a user's custom data. This will, in effect, refresh the user's auth session.
     */
    [[nodiscard]] std::promise<void> refresh_custom_user_data() const;

    std::shared_ptr<SyncUser> m_user;
};


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
    explicit App(const std::string& app_id, const std::optional<std::string>& base_url = {}, const std::optional<std::string>& path = {});

    struct credentials {
        using auth_code = util::TaggedString<class auth_code_tag>;
        using id_token = util::TaggedString<class id_token_tag>;

        static credentials anonymous();
        static credentials api_key(const std::string& key);
        static credentials facebook(const std::string& access_token);
        static credentials apple(const std::string& id_token);
        static credentials google(const auth_code& auth_code);
        static credentials google(const id_token& id_token);
        static credentials custom(const std::string& token);
        static credentials username_password(const std::string& username, const std::string& password);
        static credentials function(const bson::BsonDocument& payload);
        credentials() = delete;
        credentials(const credentials& credentials) = default;
        credentials(credentials&&) = default;
    private:
        credentials(app::AppCredentials&& credentials) noexcept;
        operator app::AppCredentials() const;
        friend class App;

#ifdef __i386__
    std::aligned_storage<8, 4>::type m_credentials[1];
#elif __x86_64__
    std::aligned_storage<16, 8>::type m_credentials[1];
#elif __arm__
    std::aligned_storage<8, 4>::type m_credentials[1];
#elif __aarch64__
    std::aligned_storage<16, 8>::type m_credentials[1];
#endif
    };

    std::promise<void> register_user(const std::string& username, const std::string& password);
    std::promise<user> login(const credentials& credentials);
    void login(const credentials& credentials, std::function<void(user, std::optional<app_error>)>&& callback);
    [[nodiscard]] internal::bridge::sync_manager get_sync_manager() const;
private:
    std::shared_ptr<app::App> m_app;
};

}
#endif /* CPP_REALM_APP_HPP */
