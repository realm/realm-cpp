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

#include <cpprealm/experimental/db.hpp>

#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/bridge/sync_error.hpp>
#include <cpprealm/internal/bridge/sync_manager.hpp>
#include <cpprealm/internal/bridge/sync_session.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/object-store/sync/app_credentials.hpp>
#include <realm/object-store/util/bson/bson.hpp>

#include <future>
#include <utility>
#include <iostream>

namespace realm {
    using sync_session = internal::bridge::sync_session;
    class SyncUser;

    namespace app {
        class App;
        struct AppError;
    }// namespace app
    namespace internal::bridge {
        struct sync_error;
    }

// MARK: User

// Represents an error state from the server.
struct app_error {
    app_error() = delete;
    app_error(const app_error& other) ;
    app_error& operator=(const app_error& other) ;
    app_error(app_error&& other);
    app_error& operator=(app_error&& other);
    ~app_error();
    app_error(realm::app::AppError&& error); //NOLINT(google-explicit-constructor)

    [[nodiscard]] std::string_view message() const;

    [[nodiscard]] std::string_view link_to_server_logs() const;

    [[nodiscard]] bool is_json_error() const;

    [[nodiscard]] bool is_service_error() const;

    [[nodiscard]] bool is_http_error() const;

    [[nodiscard]] bool is_custom_error() const;

    [[nodiscard]] bool is_client_error() const;
private:
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
    internal::bridge::storage::AppError m_error[1];
#else
    std::shared_ptr<realm::app::AppError> m_error;
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
    explicit user(std::shared_ptr<SyncUser> user);
    bool is_logged_in() const;

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
        config.sync_config().set_error_handler([](const sync_session& session, const internal::bridge::sync_error& error) {
            std::cerr<<"sync error: "<<error.message()<<std::endl;
        });
        config.set_path(sync_manager().path_for_realm(config.sync_config()));
        config.sync_config().set_client_resync_mode(realm::internal::bridge::realm::client_resync_mode::Manual);
        config.sync_config().set_stop_policy(realm::internal::bridge::realm::sync_session_stop_policy::AfterChangesUploaded);
        config.set_schema_mode(realm::internal::bridge::realm::config::schema_mode::AdditiveDiscovered);
        return config;
    }

    /**
     Logs out the current user

     The users state will be set to `Removed` is they are an anonymous user or `LoggedOut`
     if they are authenticated by an email / password or third party auth clients
     If the logout request fails, this method will still clear local authentication state.
     */
    void log_out(std::function<void(std::optional<app_error>)>&& callback) const;

    [[nodiscard]] std::future<void> log_out() const;

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
    [[nodiscard]] std::future<std::optional<bson::Bson>> call_function(const std::string& name,
                                                                       const realm::bson::BsonArray& arguments) const;

    /**
     Refresh a user's custom data. This will, in effect, refresh the user's auth session.
     */
    void refresh_custom_user_data(std::function<void(std::optional<app_error>)> callback);

    /**
     Refresh a user's custom data. This will, in effect, refresh the user's auth session.
     */
    [[nodiscard]] std::future<void> refresh_custom_user_data() const;

    std::shared_ptr<SyncUser> m_user;
};

bool operator==(const user& lhs, const user& rhs);
bool operator!=(const user& lhs, const user& rhs);

class App {
public:
    struct configuration {
        std::string app_id;
        std::optional<std::string> base_url;
        std::optional<std::string> path;
        std::optional<std::map<std::string, std::string>> custom_http_headers;
        std::optional<std::array<char, 64>> metadata_encryption_key;
    };

    [[deprecated("Use App(const configuration&) instead.")]]
    explicit App(const std::string& app_id,
                 const std::optional<std::string>& base_url = {},
                 const std::optional<std::string>& path = {},
                 const std::optional<std::map<std::string, std::string>>& custom_http_headers = {});

    App(const configuration&);

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
        credentials();
        credentials(const credentials& other) ;
        credentials& operator=(const credentials& other) ;
        credentials(credentials&& other);
        credentials& operator=(credentials&& other);
        ~credentials();
    private:
        credentials(app::AppCredentials&& credentials) noexcept;
        operator app::AppCredentials() const;
        friend class App;

#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
    internal::bridge::storage::AppCredentials m_credentials[1];
#else
        std::shared_ptr<app::AppCredentials> m_credentials;
#endif
    };

    std::future<void> register_user(const std::string& username, const std::string& password);
    std::future<user> login(const credentials &credentials);
    void login(const credentials& credentials, std::function<void(user, std::optional<app_error>)>&& callback);
    [[nodiscard]] internal::bridge::sync_manager get_sync_manager() const;
    [[nodiscard]] std::optional<user> get_current_user() const;
    void clear_cached_apps();
    std::optional<App> get_cached_app(const std::string& app_id, const std::optional<std::string>& base_url);
private:
    std::shared_ptr<app::App> m_app;
    App(std::shared_ptr<app::App>&& a) : m_app(std::move(a)) { }
};

}
#endif /* CPP_REALM_APP_HPP */
