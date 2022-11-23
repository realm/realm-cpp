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
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or utilied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#include <thread>
#include <filesystem>
#include <sstream>

#include "admin_utils.hpp"

namespace {
using namespace realm;

static app::Response do_http_request(app::Request &&request) {
    static internal::DefaultTransport transport;

    std::promise<app::Response> p;
    transport.send_request_to_server(std::move(request),
                                        [&p](auto&& response){
        p.set_value(std::move(response));
    });
    return p.get_future().get();
}

static std::string authenticate(const std::string& baas_url, const std::string& provider_type, bson::BsonDocument&& credentials)
{
    std::stringstream body;
    body << credentials;
    auto result = do_http_request({
        .method = realm::app::HttpMethod::post,
        .url = util::format("%1/api/admin/v3.0/auth/providers/%2/login", baas_url, provider_type),
        .headers = {
            {"Content-Type", "application/json;charset=utf-8"},
            {"Accept",       "application/json"}
        },
        .body = body.str()
    });
    if (result.http_status_code != 200) {
        REALM_TERMINATE(util::format("Unable to authenticate at %1 with provider '%2': %3", baas_url, provider_type, result.body).c_str());
    }
    auto parsed_response = static_cast<bson::BsonDocument>(bson::parse(result.body));
    return static_cast<std::string>(parsed_response["access_token"]);
}

} // namespace

#if !REALM_WINDOWS && !REALM_MOBILE
#include <unistd.h>

namespace local {
using namespace std::filesystem;

static void write_to_devnull() {
    /* open /dev/null for writing */
    int fd = open("/dev/null", O_WRONLY);

    dup2(fd, 1);    /* make stdout a copy of fd (> /dev/null) */
    dup2(fd, 2);    /* ...and same with stderr */
    close(fd);      /* close fd */
}

struct Process {
    std::string launch_path;
    std::map<std::string, std::string> environment = {};
    std::vector<std::string> arguments = {};

    void run() {
        auto command = launch_path;
        std::vector<const char*> args;
        args.push_back(launch_path.data());
        for (auto& arg : arguments) {
            args.push_back(arg.data());
        }
        args.push_back(nullptr);
        std::vector<const char*> env;
        std::vector<std::string> env_hold;
        for (auto &[k,v] : environment) {
            std::string cat;
            cat.append(k).append("=").append(v);
            env_hold.push_back(cat);
            env.push_back(env_hold[env_hold.size() - 1].data());
        }
        env.push_back(nullptr);

        int ev = execve(command.data(),
                        const_cast<char* const*>(args.data()),
                        const_cast<char* const*>(env.data()));
        if (ev == -1) {
            std::cout<<errno<<std::endl;
        }
    }
};

struct RealmServer {
    static inline path root_url = current_path();
    static inline path build_dir = root_url.string() + "/.baas";
    static inline path bin_dir = build_dir.string() + "/bin";

    static Process launch_add_user_process() {
        auto lib_dir = build_dir.string() + "/lib";
        auto bin_path = "$PATH:" + bin_dir.string();
        auto aws_access_key_id = getenv("AWS_ACCESS_KEY_ID");
        auto aws_secret_access_key = getenv("AWS_SECRET_ACCESS_KEY");
        std::map<std::string, std::string> env = {
                {"PATH",                  bin_path},
                {"DYLD_LIBRARY_PATH",     lib_dir},
                {"LD_LIBRARY_PATH",       lib_dir},
                {"AWS_ACCESS_KEY_ID",     aws_access_key_id},
                {"AWS_SECRET_ACCESS_KEY", aws_secret_access_key}
        };
        auto stitch_root = build_dir.string() + "/go/src/github.com/10gen/stitch";

        auto user_process = Process{
                .launch_path = bin_dir.string() + "/create_user",
                .environment = env,
                .arguments = {
                        "addUser",
                        "-domainID",
                        "000000000000000000000000",
                        "-mongoURI", "mongodb://localhost:26000",
                        "-salt", "DQOWene1723baqD!_@#",
                        "-id", "unique_user@domain.com",
                        "-password", "password"
                }
        };
        std::string command = "";
        auto arguments = user_process.arguments;
        auto environment = user_process.environment;
            if (!environment.empty()) {
                command += "" + std::accumulate(environment.begin(),
                                                environment.end(),
                                                std::string(),
                                                [&](auto one, auto &two) {
                                                    return one + " " + two.first + "=" + two.second;
                                                });
                command += " ";
            }
            command += user_process.launch_path;
            if (!arguments.empty()) {
                command += std::accumulate(arguments.begin(),
                                           arguments.end(),
                                           std::string(),
                                           [](auto one, auto &two) {
                                               return one + " " + two;
                                           });
            }

            if (!environment.empty()) {
                command += "";
            }

        if (system(command.c_str()) == 1) {
            REALM_TERMINATE(util::format("Command '%1' failed.", command).c_str());
        }
        return user_process;
    }

    static Process launch_server_process() {
        auto lib_dir = build_dir.string() + "/lib";
        auto bin_path = "$PATH:" + bin_dir.string();
        auto aws_access_key_id = getenv("AWS_ACCESS_KEY_ID");
        auto aws_secret_access_key = getenv("AWS_SECRET_ACCESS_KEY");
        std::map<std::string, std::string> env = {
                {"PATH",                  bin_path},
                {"DYLD_LIBRARY_PATH",     lib_dir},
                {"LD_LIBRARY_PATH",       lib_dir},
                {"AWS_ACCESS_KEY_ID",     aws_access_key_id},
                {"AWS_SECRET_ACCESS_KEY", aws_secret_access_key}
        };
        auto stitch_root = build_dir.string() + "/go/src/github.com/10gen/stitch";
        Process server_process;
        server_process.environment = env;
        create_directory(temp_directory_path());
        server_process.launch_path = bin_dir.string() + "/stitch_server";
        std::string config_overrides = "config_overrides.json";
        for (const auto& dirEntry : recursive_directory_iterator(current_path())) {
            if (dirEntry.path().string().find("config_overrides.json") != std::string::npos) {
                config_overrides = dirEntry.path().string();
            }
        }
        server_process.arguments = {
                "--configFile",
                stitch_root + "/etc/configs/test_config.json",
                "--configFile",
                config_overrides
        };

        server_process.run();
        return server_process;
    }

    static inline void wait_for_server_to_start() {
        auto response = do_http_request({
                                                .url="http://localhost:9090/api/admin/v3.0/groups/groupId/apps/appId"
                                        });
        while (response.body.empty()) {
            std::cout << "Server not up" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            response = do_http_request({
                                               .url="http://localhost:9090/api/admin/v3.0/groups/groupId/apps/appId"
                                       });
        }
        std::cout << "Server started!" << std::endl;
    }

    static Process launch_mongo_process() {
        create_directory(temp_directory_path());
        Process mongo_process;
        mongo_process.launch_path = bin_dir.string() + "/mongod";
        mongo_process.arguments = {
                "--quiet",
                "--dbpath", build_dir.string() + "/db_files",
                "--bind_ip", "localhost",
                "--port", "26000",
                "--replSet", "test"
        };
        mongo_process.run();

        Process init_process;
        init_process.launch_path = bin_dir.string() + "/mongo";
        init_process.arguments = {
            "--port", "26000",
            "--eval", "'rs.initiate()'"
        };
        init_process.run();
        return mongo_process;
    }

    RealmServer() = default;
public:
    static void setup() {
        for (const auto& dirEntry : recursive_directory_iterator(current_path())) {
            if (dirEntry.path().string().find("setup_baas.rb") != std::string::npos) {
                if (system(dirEntry.path().string().c_str()) == -1) {
                    REALM_TERMINATE("Failed to run setup_baas.rb");
                }
            }
        }
    }

public:
    static RealmServer shared;
};

RealmServer RealmServer::shared = RealmServer();
} // namespace local

#endif // !REALM_WINDOWS && !REALM_MOBILE

app::Response Admin::Endpoint::request(app::HttpMethod method, bson::BsonDocument&& body) const
{
    auto body_str = (std::stringstream() << body).str();
    std::string url = m_url + "?bypass_service_change=DestructiveSyncProtocolVersionIncrease";
    auto response = do_http_request({
        .method = method,
        .url = url,

        .headers = {
            {"Authorization", "Bearer " + m_access_token},
            {"Content-Type",  "application/json;charset=utf-8"},
            {"Accept",        "application/json"}
        },
        .body = std::move(body_str)
    });

    if (response.http_status_code >= 400) {
        throw std::runtime_error(util::format("An error occurred while calling %1: %2", url, response.body));
    }

    return response;
}

Admin::Session::Session(const std::string& baas_url, const std::string& access_token, const std::string& group_id, std::optional<std::string> cluster_name)
: group(util::format("%1/api/admin/v3.0/groups/%2", baas_url, group_id), access_token)
, apps(group["apps"])
, m_cluster_name(std::move(cluster_name))
, m_base_url(baas_url)
{

}

[[nodiscard]] std::string Admin::Session::create_app(bson::BsonArray queryable_fields, std::string app_name) const {
    if (m_cluster_name) {
        app_name += "-" + *m_cluster_name;
    }
    
    auto info = static_cast<bson::BsonDocument>(apps.post({{"name", app_name}}));
    auto app_id = static_cast<std::string>(info["_id"]);

    auto app = apps[app_id];

    static_cast<void>(app["secrets"].post({
        { "name", "customTokenKey" },
        { "value", "My_very_confidential_secretttttt" }
        }));

    static_cast<void>(app["auth_providers"].post({
        {"type", "custom-token"},
        {"config", bson::BsonDocument {
            {"audience", bson::BsonArray()},
            {"signingAlgorithm", "HS256"},
            {"useJWKURI", false}
        }},
        {"secret_config", bson::BsonDocument { {"signingKeys", bson::BsonArray { "customTokenKey" }}} },
        {"metadata_fields", bson::BsonArray {
            bson::BsonDocument {{"required", false}, {"name", "user_data.name"}, {"field_name", "name"}},
            bson::BsonDocument {{"required", false}, {"name", "user_data.occupation"}, {"field_name", "occupation"}},
            bson::BsonDocument {{"required", false}, {"name", "my_metadata.name"}, {"field_name", "anotherName"}}
        }}
    }));

    static_cast<void>(app["auth_providers"].post({{"type", "anon-user"}}));
    static_cast<void>(app["auth_providers"].post({
        {"type", "local-userpass"},
        {"config", bson::BsonDocument {
            {"emailConfirmationUrl", "http://foo.com"},
            {"resetPasswordUrl", "http://foo.com"},
            {"confirmEmailSubject", "Hi"},
            {"resetPasswordSubject", "Bye"},
            {"autoConfirm", true}
        }}
    }));

    auto auth_providers_endpoint = app["auth_providers"];
    auto providers = static_cast<bson::BsonArray>(auth_providers_endpoint.get());
    auto api_key_provider = std::find_if(std::begin(providers),
                                            std::end(providers), [](auto& provider) {
        return static_cast<bson::BsonDocument>(provider)["type"] == "api-key";
    });
    auth_providers_endpoint[static_cast<std::string>(static_cast<bson::BsonDocument>(*api_key_provider)["_id"])]
        ["enable"].put();

    bson::BsonDocument mongodb_service_config;
    std::string mongodb_service_type;
    if (m_cluster_name) {
        mongodb_service_config["clusterName"] = *m_cluster_name;
        mongodb_service_type = "mongodb-atlas";
    } else {
        mongodb_service_config["uri"] = "mongodb://localhost:26000";
        mongodb_service_type = "mongodb";
    }

    static_cast<void>(app["functions"].post({
        {"name", "updateUserData"},
        {"private", false},
        {"can_evaluate", {}},
        {"source",
         R"(
           exports = async function(data) {
               const user = context.user;
               const mongodb = context.services.get(")" + util::format("db-%1", app_name) + R"(");
               const userDataCollection = mongodb.db("test_data").collection("UserData");
               doc = await userDataCollection.updateOne(
                                                       { "user_id": user.id },
                                                       { "$set": data },
                                                       { "upsert": true }
                                                       );
               return doc;
           };
           )"
        }}));

    bson::BsonDocument userData = {
        {"schema", bson::BsonDocument {
                       {"properties", bson::BsonDocument {
                                          {"_id", bson::BsonDocument {{"bsonType", "objectId"}}},
                                          {"name", bson::BsonDocument {{"bsonType", "string"}}},
                                          {"user_id", bson::BsonDocument {{"bsonType", "string"}}},
                                      }},
                       {"required", bson::BsonArray {"_id", "name", "user_id"}},
                       {"title", "UserData"}
                   }},
        {"metadata", bson::BsonDocument {
                         {"data_source", util::format("db-%1", app_name)},
                         {"database", "test_data"},
                         {"collection", "UserData"}}
        },
    };
    static_cast<void>(app["schemas"].post(std::move(userData)));

    bson::BsonDocument mongodb_service_response(app["services"].post({
        {"name", util::format("db-%1", app_name)},
        {"type", mongodb_service_type},
        {"config", mongodb_service_config}
    }));
    std::string mongodb_service_id(mongodb_service_response["_id"]);

    bson::BsonDocument service_config = {
        {"flexible_sync", bson::BsonDocument {
            {"state", "enabled"},
            {"database_name", util::format("test-data-app-%1", app_id)},
            {"queryable_fields_names", queryable_fields},
            {"permissions", bson::BsonDocument{
                {"rules",        bson::BsonDocument()},
                {"defaultRoles", bson::BsonArray{bson::BsonDocument{
                    {"name",      "all"},
                    {"applyWhen", bson::BsonDocument()},
                    {"read",      true},
                    {"write",     true}
                }}}
            }}
        }}
    };

    // The cluster linking must be separated from enabling sync because Atlas
    // takes a few seconds to provision a user for BaaS, meaning enabling sync
    // will fail if we attempt to do it with the same request. It's nondeterministic
    // how long it'll take, so we must retry for a while.
    constexpr int max_attempts = 120;
    for (int attempt = 0; attempt <= max_attempts; attempt++) {
        try {
            app["services"][mongodb_service_id]["config"].patch(std::move(service_config));
            break;
        } catch(std::exception&) {
            if (attempt == max_attempts) {
                REALM_TERMINATE("Could not link Atlas cluster");
            } else {
                util::format(std::cerr, "Could not update MongoDB service after %1 seconds. Will keep retrying.\n", (attempt + 1) * 5);
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        }
    }

    auto config = app["sync"]["config"];
    config.put({{"development_mode_enabled", true}});

    bson::BsonDocument rules = {
        {"database", "test_data"},
        {"collection", "UserData"},
        {"roles", bson::BsonArray {
                      bson::BsonDocument {{"name", "default"},
                                         {"apply_when", {}},
                                         {"insert", true},
                                         {"delete", true},
                                         {"additional_fields", {}}},
                  }
        }
    };

    static_cast<void>(app["services"][static_cast<std::string>(mongodb_service_id)]["rules"].post(rules));
    app["custom_user_data"].patch(bson::BsonDocument {
        {"mongo_service_id", mongodb_service_id},
        {"enabled", true},
        {"database_name", "test_data"},
        {"collection_name", "UserData"},
        {"user_id_field", "user_id"},
    });
    return static_cast<std::string>(info["client_app_id"]);
}

Admin::Session Admin::Session::local(std::optional<std::string> baas_url)
{
    std::string base_url = baas_url.value_or("http://localhost:9090");

    bson::BsonDocument credentials {
        {"username", "unique_user@domain.com"},
        {"password", "password"}
    };
    std::string access_token = authenticate(base_url, "local-userpass", std::move(credentials));

    auto result = do_http_request({
        .url = base_url + "/api/admin/v3.0/auth/profile",
        .headers = {
            {"Authorization", "Bearer " + access_token}
        }
    });
    auto parsed_response = static_cast<bson::BsonDocument>(bson::parse(result.body));
    auto roles = static_cast<bson::BsonArray>(parsed_response["roles"]);
    auto group_id = static_cast<std::string>(static_cast<bson::BsonDocument>(roles[0])["group_id"]);

    return Session(base_url, access_token, group_id);
}

Admin::Session Admin::Session::atlas(const std::string& baas_url, std::string project_id, std::string cluster_name, std::string api_key, std::string private_api_key)
{
    bson::BsonDocument credentials {
        {"username", std::move(api_key)},
        {"apiKey", std::move(private_api_key)}
    };
    std::string access_token = authenticate(baas_url, "mongodb-cloud", std::move(credentials));

    return Session(baas_url, access_token, std::move(project_id), std::move(cluster_name));
}

static Admin::Session make_default_session() {
    if (const char* baas_endpoint = getenv("REALM_BAAS_ENDPOINT")) {
        const char* project_id = getenv("REALM_ATLAS_PROJECT_ID");
        REALM_ASSERT_RELEASE(project_id);

        const char* cluster_name = getenv("REALM_ATLAS_CLUSTER_NAME");
        REALM_ASSERT_RELEASE(cluster_name);

        const char* api_key = getenv("REALM_ATLAS_API_KEY");
        REALM_ASSERT_RELEASE(api_key);

        const char* private_api_key = getenv("REALM_ATLAS_PRIVATE_API_KEY");
        REALM_ASSERT_RELEASE(private_api_key);

        return Admin::Session::atlas(baas_endpoint, project_id, cluster_name, api_key, private_api_key);
    } else if (const char* local_endpoint = getenv("REALM_LOCAL_ENDPOINT")) {
        return Admin::Session::local(local_endpoint);
    } else {
#if !REALM_WINDOWS && !REALM_MOBILE
        using namespace local;
        RealmServer::setup();

        if (fork()) {
            if (fork()) {
                if (fork()) {
                    // parent
                } else {
                    // third child
                    write_to_devnull();
                    RealmServer::launch_add_user_process();
                    exit(0);
                }
            } else {
                // second child
                write_to_devnull();
                RealmServer::launch_server_process();
                exit(0);
            }
        } else {
            // first child
            write_to_devnull();
            RealmServer::launch_mongo_process();
            exit(0);
        }
        RealmServer::wait_for_server_to_start();
        RealmServer::launch_add_user_process();

        return Admin::Session::local();
#else
        REALM_TERMINATE("Automatic local tests are unsupported on this platform. Set the REALM_LOCAL_ENDPOINT environment variable to the base URL of a running BAAS server.");
#endif // !REALM_WINDOWS && !REALM_MOBILE
    }
}

const Admin::Session& Admin::shared() {
    static Admin::Session session(make_default_session());
    return session;
}
