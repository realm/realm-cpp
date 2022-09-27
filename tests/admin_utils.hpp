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

#ifndef REALM_ADMIN_UTILS_HPP
#define REALM_ADMIN_UTILS_HPP

#include <cstdio>
#include <filesystem>
#include <iostream>
#include <map>
#include <numeric>
#include <thread>
#include <semaphore>
#include <string>
#include <sstream>
#include <vector>

#include <sys/wait.h>

#include <cpprealm/schema.hpp>
#include <cpprealm/app.hpp>
#include <realm/object-store/sync/app.hpp>
#include <realm/object-store/object_schema.hpp>
#include <realm/object-store/schema.hpp>
#include <realm/object-store/property.hpp>
#include <realm/util/scope_exit.hpp>

#if __APPLE__
#include <CoreFoundation/CFBundle.h>
#endif
using namespace std::filesystem;

namespace {
    inline auto transport = realm::internal::DefaultTransport();
    using namespace realm;

    realm::app::Response do_http_request(app::Request &&request) {
        app::Response r_response;
        std::condition_variable cv;
        std::mutex m;
        bool ready = false;
        std::unique_lock lk(m);
        transport.send_request_to_server(std::move(request),
                                         [&r_response, &ready, &lk, &cv](auto request, auto response){
            r_response = std::move(response);
            ready = true;
            lk.unlock();
            cv.notify_one();
        });
        cv.wait(lk, [&ready]{return ready;});
        return r_response;
    }

    struct Process {
        std::string launch_path;
        std::map<std::string, std::string> environment = {};
        std::vector<std::string> arguments = {};
        std::thread thread;
        std::atomic<FILE*> pipe;
        std::atomic<bool> is_running;
        std::atomic<bool> is_cancelled;
        int termination_status = -1;
        std::atomic<int> pid = -1;

        ~Process() {
            is_running = false;
            is_cancelled = true;

            if (thread.joinable()) {
                thread.join();
            }
        }
        void run() {
            auto command = std::string();
            if (!environment.empty()) {
                command += "" + std::accumulate(environment.begin(),
                                                environment.end(),
                                                std::string(),
                                                [&](auto one, auto &two) {
                                                    return one + " " + two.first + "=" + two.second;
                                                });
                command += " ";
            }
            command += launch_path;
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

            thread = std::thread([&, command] {
                is_running = true;
                pipe = popen(command.c_str(), "w");
                int child_pid;
                fscanf(pipe, "%d", &child_pid);
                pid = child_pid;
                char path[PATH_MAX];
                while (pipe != nullptr && fgets(path, PATH_MAX, pipe) != nullptr && !is_cancelled) {
                    std::cout<<path<<std::endl;
                }

                is_running = false;
            });
        }

        void wait_until_exit() noexcept {
            while (pipe == nullptr || pid == -1) {
            }
            waitpid(pid, nullptr, 0);
        }
    };

    std::string bson_type(const PropertyType &type) {
        switch (type & ~PropertyType::Flags) {
            case realm::PropertyType::UUID:
                return "uuid";
            case realm::PropertyType::Mixed:
                return "mixed";
            case realm::PropertyType::Bool:
                return "bool";
            case realm::PropertyType::Data:
                return "binData";
            case realm::PropertyType::Date:
                return "date";
            case realm::PropertyType::Decimal:
                return "decimal";
            case realm::PropertyType::Double:
                return "double";
            case realm::PropertyType::Float:
                return "float";
            case realm::PropertyType::Int:
                return "long";
            case realm::PropertyType::Object:
                return "object";
            case realm::PropertyType::ObjectId:
                return "objectId";
            case realm::PropertyType::String:
                return "string";
            case realm::PropertyType::LinkingObjects:
                return "linkingObjects";
            default:
                abort();
        }
    }

    bson::BsonDocument stitch_rule(const Property &property, const ObjectSchema &schema) {
        std::string type;
        if (property.type == PropertyType::Object) {
            type = bson_type(schema.primary_key_property()->type);
        } else {
            type = bson_type(property.type);
        }

        if (is_array(property.type)) {
            return {
                    {"bsonType", "array"},
                    {"items",    bson::BsonDocument{
                            {"bsonType", type}
                    }
                    }
            };
        }
        if (is_set(property.type)) {
            return {
                    {"bsonType",    "array"},
                    {"uniqueItems", true},
                    {"items",       bson::BsonDocument{
                            {"bsonType", type}
                    }
                    }
            };
        }
        if (is_dictionary(property.type)) {
            return {
                    {"bsonType",             "object"},
                    {"properties",           bson::BsonDocument()},
                    {"additionalProperties", bson::BsonDocument{
                            {"bsonType", type}
                    }
                    }
            };
        }
        return {
                {"bsonType", type}
        };
    }

    bson::BsonDocument stitch_rule(const ObjectSchema &schema, std::string id = "") {
        bson::BsonDocument stitchProperties;

        bson::BsonDocument relationships;

        // First pass we only add the properties to the schema as we can't add
        // links until the targets of the links exist.
        auto pk = schema.primary_key_property();
        if (pk) {
            stitchProperties[pk->name] = stitch_rule(*pk, schema);
        }
        for (auto &property: schema.persisted_properties) {
            if (property.type != realm::PropertyType::Object) {
                stitchProperties[property.name] = stitch_rule(property, schema);
            } else if (!id.empty()) {
                stitchProperties[property.name] = stitch_rule(property, schema);
                relationships[property.name] = bson::BsonDocument{
                        {"ref",         "#/relationship/mongodb1/test_data/" + property.object_type},
                        {"foreign_key", "_id"},
                        {"is_list",     is_array(property.type) || is_set(property.type) ||
                                        is_dictionary(property.type)}
                };
            }
        }
        bson::BsonArray required_properties;
        for (auto& property : schema.persisted_properties) {
            if (!(is_nullable(property.type) ||
                is_mixed(property.type) ||
                is_array(property.type) ||
                is_dictionary(property.type) ||
                is_set(property.type))) {
                required_properties.push_back(property.name);
            }
        }
        return {
                {"_id",           id},
                {"schema",        bson::BsonDocument{
                        {"properties", stitchProperties},
                        {"required",   required_properties},
                        {"title",      schema.name}
                }},
                {"metadata",      bson::BsonDocument{
                        {"data_source", "mongodb1"},
                        {"database",    "test_data"},
                        {"collection",  schema.name},
                }},
                {"relationships", relationships}
        };
    }
}

struct Admin {
    struct Session {
        static Admin::Session shared;
        std::string access_token;
        std::string group_id;

        struct Endpoint {
        public:
            std::string access_token;
            std::string group_id;
            std::string m_url;

            app::Response request(app::HttpMethod method, bson::BsonDocument&& body = {}) const {
                auto body_str = (std::stringstream() << body).str();
                return do_http_request({
                                               .method = method,
                                               .url = m_url +
                                                      "?bypass_service_change=DestructiveSyncProtocolVersionIncrease",

                                               .headers = {
                                                       {"Authorization", "Bearer " + access_token},
                                                       {"Content-Type",  "application/json;charset=utf-8"},
                                                       {"Accept",        "application/json"}
                                               },
                                               .body = std::move(body_str)
                                       });
            };

            [[nodiscard]] bson::Bson get() const {
                return bson::parse(request(app::HttpMethod::get).body);
            }

            [[nodiscard]] bson::Bson post(bson::BsonDocument body) const {
                return bson::parse(request(app::HttpMethod::post, std::move(body)).body);
            }

            bson::Bson put(bson::BsonDocument&& body = {}) const {
                auto response = request(app::HttpMethod::put, std::move(body));
                if (response.body.empty()) {
                    return {};
                }
                return bson::parse(response.body);
            }
            bson::Bson patch(bson::BsonDocument&& body = {}) const {
                auto response = request(app::HttpMethod::patch, std::move(body));
                if (response.body.empty()) {
                    return {};
                }
                return bson::parse(response.body);
            }
            Endpoint operator[](const std::string& url) const {
                return {access_token, group_id, m_url + "/" + url};
            }
        };
        Endpoint apps;

        static inline Session login() {
            std::stringstream body;
            body << bson::BsonDocument{
                    {"provider", "userpass"},
                    {"username", "unique_user@domain.com"},
                    {"password", "password"}
            };
            auto result = do_http_request(app::Request{
                                                  .method = realm::app::HttpMethod::post,
                                                  .url = "http://localhost:9090/api/admin/v3.0/auth/providers/local-userpass/login",
                                                  .headers = {
                                                          {"Content-Type", "application/json;charset=utf-8"},
                                                          {"Accept",       "application/json"}
                                                  },
                                                  .body = body.str()
                                          });
            if (result.http_status_code != 200) {
                abort();
            }
            auto parsed_response = static_cast<bson::BsonDocument>(bson::parse(result.body));
            std::string access_token = static_cast<std::string>(parsed_response["access_token"]);
            result = do_http_request({
                                             .url = "http://localhost:9090/api/admin/v3.0/auth/profile",
                                             .headers = {
                                                     {"Authorization", "Bearer " + access_token}
                                             }
                                     });
            parsed_response = static_cast<bson::BsonDocument>(bson::parse(result.body));
            auto roles = static_cast<bson::BsonArray>(parsed_response["roles"]);
            auto group_id = static_cast<std::string>(static_cast<bson::BsonDocument>(roles[0])["group_id"]);

            Session session = {access_token, group_id};
            session.apps = Endpoint{access_token,
                            group_id,
                            "http://localhost:9090/api/admin/v3.0/groups/" + group_id + "/apps"};
            return session;
        }

        template <type_info::ObjectBasePersistable ...Ts>
        [[nodiscard]] std::string create_app(bson::BsonArray queryable_fields = {}) const {
            auto info = static_cast<bson::BsonDocument>(apps.post({{"name", "test"}}));
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

            static_cast<void>(app["secrets"].post({
                                        {     "name", "BackingDB_uri"},
                                        {"value", "mongodb://localhost:26000"}
                                }));

            auto service_response = app["services"].post({
                                                               { "name", "mongodb1"},
                                                               {"type", "mongodb"},
                                                               {"config", bson::BsonDocument {
                                                                       {"uri", "mongodb://localhost:26000"}
                                                               }}
            });
            auto service_id = static_cast<bson::BsonDocument>(service_response)["_id"];

            // Creating the schema is a two-step process where we first add all the
            // objects with their properties to them so that we can add relationships

            bson::BsonArray schema_creations;
            bson::BsonDocument schema_ids;
            bson::BsonArray asymmetric_tables;
            std::vector<ObjectSchema> schema;
            (schema.push_back(Ts::schema::to_core_schema()), ...);
            for (auto& os : schema) {
                if (os.table_type == ObjectSchema::ObjectType::TopLevelAsymmetric) {
                    asymmetric_tables.push_back(os.name);
                }
                schema_creations.push_back(app["schemas"].post(stitch_rule(os)));
            }
            for (auto& schema : schema_creations) {
                auto metadata = static_cast<bson::BsonDocument>(static_cast<bson::BsonDocument>(schema)["metadata"]);
                schema_ids[static_cast<std::string>(metadata["collection"])] = static_cast<bson::BsonDocument>(schema)["_id"];
            }

            for (auto& os : schema) {
                auto id = static_cast<std::string>(schema_ids[os.name]);
                app["schemas"][id].put(stitch_rule(os, id));
            }

            bson::BsonDocument service_config = {
                    {"flexible_sync", bson::BsonDocument {
                            {"state", "enabled"},
                            {"database_name", "test_data"},
                            {"queryable_fields_names", queryable_fields},
                            {"asymmetric_tables", asymmetric_tables},
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
            app["services"][static_cast<std::string>(service_id)]["config"].patch(std::move(service_config));
            auto config = app["sync"]["config"];
            config.put(bson::BsonDocument {{"development_mode_enabled", true}});
            return static_cast<std::string>(info["client_app_id"]);
        }
    };
};
namespace {
    class RealmServer {
        static inline path root_url = current_path();
        static inline path build_dir = root_url.string() + "/.baas";
        static inline path bin_dir = build_dir.string() + "/bin";
        Process server_process;
        Process mongo_process;

        void launch_server_process() {
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

            for (auto i = 0; i < 5; i++) {
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
                user_process.run();
                user_process.wait_until_exit();
                if (user_process.termination_status == 0) {
                    break;
                }
            }

            server_process.environment = env;
            std::filesystem::create_directory(temp_directory_path());
            server_process.launch_path = bin_dir.string() + "/stitch_server";
            std::string config_overrides = "config_overrides.json";
            for (const auto& dirEntry : recursive_directory_iterator(std::filesystem::current_path())) {
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
            wait_for_server_to_start();
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

        void launch_mongo_process() {
            create_directory(temp_directory_path());

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
            init_process.wait_until_exit();
        }

        RealmServer() = default;
    public:
        Admin::Session login() {
            auto setup_process = Process();
            for (const auto& dirEntry : recursive_directory_iterator(std::filesystem::current_path())) {
                if (dirEntry.path().string().find("setup_baas.rb") != std::string::npos) {
                    setup_process.launch_path = "ruby " + dirEntry.path().string();
                }
            }
            setup_process.run();
            setup_process.wait_until_exit();

            launch_mongo_process();
            launch_server_process();
            return Admin::Session::login();
        }

    public:
        static RealmServer shared;
    };

    RealmServer RealmServer::shared = RealmServer();
}

#endif //REALM_ADMIN_UTILS_HPP
