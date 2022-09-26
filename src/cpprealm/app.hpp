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
#include <map>
#include <utility>

#if !__APPLE__
#include <curl/curl.h>
#endif
namespace realm {

namespace internal {

#if !__APPLE__
class CurlGlobalGuard {
public:
    CurlGlobalGuard()
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (++m_users == 1) {
            curl_global_init(CURL_GLOBAL_ALL);
        }
    }

    ~CurlGlobalGuard()
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (--m_users == 0) {
            curl_global_cleanup();
        }
    }

    CurlGlobalGuard(const CurlGlobalGuard&) = delete;
    CurlGlobalGuard(CurlGlobalGuard&&) = delete;
    CurlGlobalGuard& operator=(const CurlGlobalGuard&) = delete;
    CurlGlobalGuard& operator=(CurlGlobalGuard&&) = delete;

private:
    static std::mutex m_mutex;
    static int m_users;
};

std::mutex CurlGlobalGuard::m_mutex = {};
int CurlGlobalGuard::m_users = 0;

size_t curl_write_cb(char* ptr, size_t size, size_t nmemb, std::string* response)
{
    REALM_ASSERT(response);
    size_t realsize = size * nmemb;
    response->append(ptr, realsize);
    return realsize;
}

size_t curl_header_cb(char* buffer, size_t size, size_t nitems, std::map<std::string, std::string>* response_headers)
{
    REALM_ASSERT(response_headers);
    std::string combined(buffer, size * nitems);
    if (auto pos = combined.find(':'); pos != std::string::npos) {
        std::string key = combined.substr(0, pos);
        std::string value = combined.substr(pos + 1);
        while (value.size() > 0 && value[0] == ' ') {
            value = value.substr(1);
        }
        while (value.size() > 0 && (value[value.size() - 1] == '\r' || value[value.size() - 1] == '\n')) {
            value = value.substr(0, value.size() - 1);
        }
        response_headers->insert({key, value});
    }
    else {
        if (combined.size() > 5 && combined.substr(0, 5) != "HTTP/") { // ignore for now HTTP/1.1 ...
            std::cerr << "test transport skipping header: " << combined << std::endl;
        }
    }
    return nitems * size;
}
app::Response do_http_request(const app::Request& request)
{
    CurlGlobalGuard curl_global_guard;
    auto curl = curl_easy_init();
    if (!curl) {
        return app::Response{500, -1};
    }

    struct curl_slist* list = nullptr;
    auto curl_cleanup = util::ScopeExit([&]() noexcept {
        curl_easy_cleanup(curl);
        curl_slist_free_all(list);
    });

    std::string response;
    std::map<std::string, std::string> response_headers;

    /* First set the URL that is about to receive our POST. This URL can
     just as well be a https:// URL if that is what should receive the
     data. */
    curl_easy_setopt(curl, CURLOPT_URL, request.url.c_str());

    /* Now specify the POST data */
    if (request.method == app::HttpMethod::post) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
    }
    else if (request.method == app::HttpMethod::put) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
    }
    else if (request.method == app::HttpMethod::patch) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
    }
    else if (request.method == app::HttpMethod::del) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
    }
    else if (request.method == app::HttpMethod::patch) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, request.timeout_ms);

    for (auto header : request.headers) {
        auto header_str = util::format("%1: %2", header.first, header.second);
        list = curl_slist_append(list, header_str.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_header_cb);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response_headers);

    auto response_code = curl_easy_perform(curl);
    if (response_code != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed when sending request to '%s' with body '%s': %s\n",
                request.url.c_str(), request.body.c_str(), curl_easy_strerror(response_code));
    }
    int http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    return {
        http_code,
        0, // binding_response_code
        std::move(response_headers),
        std::move(response),
    };
}
#endif
#if !__APPLE__
class DefaultTransport : public app::GenericNetworkTransport {
public:
    void send_request_to_server(app::Request&& request,
                                util::UniqueFunction<void(const app::Response&)>&& completion_block)
    {

        completion_block(do_http_request(request));
    }
};
#else
class DefaultTransport : public app::GenericNetworkTransport {
public:
    void send_request_to_server(app::Request&& request,
                                app::HttpCompletion&& completion_block) override;
};
#endif
} // anonymous namespace

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

// MARK: User

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
    explicit User(std::shared_ptr<SyncUser> user, std::shared_ptr<app::App> app)
    : m_user(std::move(user)), m_app(std::move(app))
    {
    }

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

    /**
     Open a partition-based realm instance for the given url.

     @param partition_value The  value the Realm is partitioned on.
     @return A `thread_safe_reference` to the synchronized db.
     */
    template <type_info::ObjectBasePersistable ...Ts, typename T>
    task<thread_safe_reference<db<Ts...>>> realm(const T& partition_value) const requires (type_info::StringPersistable<T> || type_info::IntPersistable<T>);

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

    task<std::optional<app_error>> log_out() {
        auto error = co_await make_awaitable<util::Optional<app_error>>([&] (auto cb)
                                                                        {
                                                                            m_app->log_out(m_user, cb);
                                                                        });
        co_return error ? std::optional<app_error>{app_error(*error)} : std::nullopt;
    }

    void log_out(util::UniqueFunction<void(std::optional<app_error>)>&& callback)
    {
        m_app->log_out(m_user, [cb = std::move(callback)](auto error) {
            cb(error ? std::optional<app_error>{app_error(*error)} : std::nullopt);
        });
    }

    std::map<std::string, std::any> custom_data()
    {
        std::map<std::string, std::any> custom_data_map;
        if (auto bson = m_user->custom_data()) {
            for (auto it = bson->begin(); it != bson->end(); ++it) {
                const auto& entry = (*it);
                custom_data_map[entry.first.data()] = entry.second;
            }
        }
        return custom_data_map;
    }

    std::shared_ptr<app::App> m_app;
    std::shared_ptr<SyncUser> m_user;
};
static_assert((int)User::state::logged_in  == (int)SyncUser::State::LoggedIn);
static_assert((int)User::state::logged_out == (int)SyncUser::State::LoggedOut);
static_assert((int)User::state::removed    == (int)SyncUser::State::Removed);

class App {
    static std::unique_ptr<util::Logger> defaultSyncLogger(util::Logger::Level level) {
        struct SyncLogger : public util::RootLogger {
            void do_log(Level, const std::string& message) override {
                std::cout<<"sync: "<<message<<std::endl;
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
        try {
            auto user = co_await make_awaitable<std::shared_ptr<SyncUser>>([this, credentials = std::move(credentials)](auto cb) {
                m_app->log_in_with_credentials(credentials.m_credentials, cb);
            });
            co_return std::move(User{std::move(user), m_app});
        } catch (std::exception& err) {
            throw;
        }
    }

    void login(const Credentials& credentials, util::UniqueFunction<void(User, std::optional<app_error>)>&& callback) {
        m_app->log_in_with_credentials(credentials.m_credentials, [cb = std::move(callback), this](auto& user, auto error) {
            cb(User{std::move(user), m_app}, error ? std::optional<app_error>{app_error(*error)} : std::nullopt);
        });
    }

    struct function {
        void operator [](const std::string& name) {

        };
    };

    function function()
    {
        return {};
    }

private:
    std::shared_ptr<app::App> m_app;
};

// MARK: Impl

template <type_info::ObjectBasePersistable ...Ts, typename T>
task<thread_safe_reference<db<Ts...>>> User::realm(const T& partition_value) const requires (type_info::StringPersistable<T> || type_info::IntPersistable<T>)
{
    db_config config;
    config.sync_config = std::make_shared<SyncConfig>(m_user, bson::Bson(partition_value));
    config.sync_config->error_handler = [](const std::shared_ptr<SyncSession>& session, const SyncError& error) {
        std::cerr<<"sync error: "<<error.message<<std::endl;
    };
    config.path = m_user->sync_manager()->path_for_realm(*config.sync_config);
    config.sync_config->client_resync_mode = realm::ClientResyncMode::Manual;
    config.sync_config->stop_policy = SyncSessionStopPolicy::AfterChangesUploaded;
    auto tsr = co_await async_open<Ts...>(std::move(config));
    co_return tsr;
}

}
#endif /* Header_h */
