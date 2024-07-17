////////////////////////////////////////////////////////////////////////////
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

#include <sstream>
#include <thread>
#include <iostream>
#include <cpprealm/networking/http.hpp>
#include <cpprealm/internal/networking/utils.hpp>

#include "admin_utils.hpp"
#include "external/json/json.hpp"

namespace Admin {
    Admin::Session *Admin::Session::instance = nullptr;
    std::mutex Admin::Session::mutex;

    static app::Response do_http_request(app::Request &&request) {
        networking::default_http_transport transport;
        std::promise<app::Response> p;
        std::future<app::Response> f = p.get_future();
        transport.send_request_to_server(::realm::internal::networking::to_request(std::move(request)),
                                         [&p](auto&& response) {
                                             p.set_value(::realm::internal::networking::to_core_response(std::move(response)));
                                         });
        return f.get();
    }

    static std::pair<std::string, std::string> authenticate(const std::string &baas_url, const std::string &provider_type, bson::BsonDocument &&credentials) {
        std::stringstream body;
        body << credentials;
        auto request = app::Request();
        request.method = realm::app::HttpMethod::post;
        request.url = util::format("%1/api/admin/v3.0/auth/providers/%2/login", baas_url, provider_type);
        request.headers = {
                {"Content-Type", "application/json;charset=utf-8"},
                {"Accept", "application/json"}};
        request.body = body.str();

        auto result = do_http_request(std::move(request));
        if (result.http_status_code != 200) {
            REALM_TERMINATE(util::format("Unable to authenticate at %1 with provider '%2': %3", baas_url, provider_type, result.body).c_str());
        }
        auto parsed_response = static_cast<bson::BsonDocument>(bson::parse(result.body));
        return {static_cast<std::string>(parsed_response["access_token"]), static_cast<std::string>(parsed_response["refresh_token"])};
    }


    void Admin::baas_manager::start() {
        auto request = realm::app::Request();
        request.method = realm::app::HttpMethod::post;
        request.url = realm::util::format("%1/startContainer?branch=master", m_baas_base_url);
        request.headers = {
                {"Content-Type", "application/json"},
                {"apiKey", *m_baasaas_api_key}};

        auto result = Admin::do_http_request(std::move(request));

        if (result.http_status_code != 200) {
            REALM_TERMINATE("Unable to start container");
        }

        m_container_id = nlohmann::json::parse(result.body)["id"];
    }

    void Admin::baas_manager::stop(const std::string& container_id) const {
        auto request = realm::app::Request();
        request.method = realm::app::HttpMethod::post;
        request.url = realm::util::format("%1/stopContainer?id=%2", m_baas_base_url, container_id);
        request.headers = {
                {"Content-Type", "application/json"},
                {"apiKey", *m_baasaas_api_key}};

        auto result = Admin::do_http_request(std::move(request));
        if (result.http_status_code != 200) {
            REALM_TERMINATE("Unable to stop container");
        }
    }

    const std::string Admin::baas_manager::container_id() const {
        if (!m_container_id) {
            REALM_TERMINATE("No container has started.");
        }
        return *m_container_id;
    }

    const std::string Admin::baas_manager::wait_for_container() const {
        if (!m_container_id) {
            REALM_TERMINATE("No container has started.");
        }
        std::optional<std::string> url;
        size_t attempts = 0;
        while (!url) {
            auto request = realm::app::Request();
            request.method = realm::app::HttpMethod::get;
            request.url = realm::util::format("%1/containerStatus?id=%2", m_baas_base_url, *m_container_id);
            request.headers = {
                    {"Content-Type", "application/json;charset=utf-8"},
                    {"apiKey", *m_baasaas_api_key}};

            auto result = Admin::do_http_request(std::move(request));
            auto json = nlohmann::json::parse(result.body);
            if (json.contains("httpUrl")) {
                url = json["httpUrl"];
            }
            attempts++;
            std::cout << "Baasaas container not ready after attempt " << attempts << " will try again in 3 seconds."
                      << "\n";
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }

        bool has_started = false;
        while (!has_started) {
            auto request = realm::app::Request();
            request.method = realm::app::HttpMethod::get;
            request.url = realm::util::format("%1/api/private/v1.0/version", *url);

            auto result = Admin::do_http_request(std::move(request));
            if (result.http_status_code >= 200 && result.http_status_code < 300) {
                break;
            }
            attempts++;
            std::cout << "Baasaas server not ready after attempt " << attempts << " will try again in 3 seconds."
                      << "\n";
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        return *url;
    }

    realm::app::Response Admin::Endpoint::request(app::HttpMethod method, bson::BsonDocument &&body) const {
        std::stringstream ss;
        ss << body;
        auto body_str = ss.str();

        return request(method, body_str);
    }

    realm::app::Response Admin::Endpoint::request(app::HttpMethod method, const std::string &body) const {
        std::string url = m_url + "?bypass_service_change=DestructiveSyncProtocolVersionIncrease";

        auto request = app::Request();
        request.method = method;
        request.url = url;
        request.headers = {
                {"Authorization", "Bearer " + m_access_token},
                {"Content-Type", "application/json;charset=utf-8"},
                {"Accept", "application/json"}};
        request.body = std::move(body);
        auto response = Admin::do_http_request(std::move(request));

        if (response.http_status_code >= 400) {
            throw std::runtime_error(util::format("An error occurred while calling %1: %2", url, response.body));
        }

        return response;
    }

    std::string Admin::Session::create_app(bson::BsonArray queryable_fields, std::string app_name, bool is_asymmetric, bool disable_recovery_mode) {
        refresh_access_token();
        recovery_mode_disabled = disable_recovery_mode;
        auto info = static_cast<bson::BsonDocument>(apps.post({{"name", app_name}}));
        app_name = static_cast<std::string>(info["client_app_id"]);

        if (m_cluster_name) {
            app_name += "-" + *m_cluster_name;
        }
        m_app_id = static_cast<std::string>(info["_id"]);

        auto app = apps[m_app_id];

        static_cast<void>(app["secrets"].post({{"name", "customTokenKey"},
                                               {"value", "My_very_confidential_secretttttt"}}));

        static_cast<void>(app["auth_providers"].post({{"type", "custom-token"},
                                                      {"config", bson::BsonDocument{
                                                                         {"audience", bson::BsonArray()},
                                                                         {"signingAlgorithm", "HS256"},
                                                                         {"useJWKURI", false}}},
                                                      {"secret_config", bson::BsonDocument{{"signingKeys", bson::BsonArray{"customTokenKey"}}}},
                                                      {"metadata_fields", bson::BsonArray{bson::BsonDocument{{"required", false}, {"name", "user_data.name"}, {"field_name", "name"}}, bson::BsonDocument{{"required", false}, {"name", "user_data.occupation"}, {"field_name", "occupation"}}, bson::BsonDocument{{"required", false}, {"name", "my_metadata.name"}, {"field_name", "anotherName"}}}}}));

        static_cast<void>(app["auth_providers"].post({{"type", "anon-user"}}));
        static_cast<void>(app["auth_providers"].post({{"type", "local-userpass"},
                                                      {"config", bson::BsonDocument{
                                                                         {"emailConfirmationUrl", "http://foo.com"},
                                                                         {"resetPasswordUrl", "http://foo.com"},
                                                                         {"confirmEmailSubject", "Hi"},
                                                                         {"resetPasswordSubject", "Bye"},
                                                                         {"autoConfirm", true}}}}));

        auto auth_providers_endpoint = app["auth_providers"];
        auto providers = static_cast<bson::BsonArray>(auth_providers_endpoint.get());
        auto api_key_provider = std::find_if(std::begin(providers),
                                             std::end(providers), [](auto &provider) {
                                                 return static_cast<bson::BsonDocument>(provider)["type"] == "api-key";
                                             });
        auth_providers_endpoint[static_cast<std::string>(static_cast<bson::BsonDocument>(*api_key_provider)["_id"])]
                               ["enable"]
                                       .put();

        bson::BsonDocument mongodb_service_config;
        std::string mongodb_service_type;
        if (m_cluster_name) {
            mongodb_service_config["clusterName"] = *m_cluster_name;
            mongodb_service_type = "mongodb-atlas";
        } else {
            mongodb_service_config["uri"] = "mongodb://localhost:26000";
            mongodb_service_type = "mongodb";
        }

        static_cast<void>(app["functions"].post({{"name", "updateUserData"},
                                                 {"private", false},
                                                 {"can_evaluate", {}},
                                                 {"source",
                                                  R"(
           exports = async function(data) {
               const user = context.user;
               const mongodb = context.services.get(")" + util::format("db-%1", app_name) +
                                                          R"(");
               const userDataCollection = mongodb.db("test_data").collection("UserData");
               doc = await userDataCollection.updateOne(
                                                       { "user_id": user.id },
                                                       { "$set": data },
                                                       { "upsert": true }
                                                       );
               return doc;
           };
           )"}}));

        static_cast<void>(app["functions"].post({{"name", "asymmetricSyncData"},
                                                 {"private", false},
                                                 {"can_evaluate", {}},
                                                 {"source",
                                                  R"(
               exports = async function(data) {
                   const user = context.user;
                   const mongodb = context.services.get(")" +
                                                          util::format("db-%1", app_name) + R"(");
                   const objectCollection = mongodb.db(")" +
                                                          util::format("test_data") + R"(").collection("AllTypesAsymmetricObject");
                   doc = await objectCollection.find({"_id": BSON.ObjectId(data._id)});
                   return doc;
               };
           )"}}));

        static_cast<void>(app["functions"].post({{"name", "deleteClientResetObjects"},
                                                 {"private", false},
                                                 {"can_evaluate", {}},
                                                 {"source",
                                                  R"(
               exports = async function(data) {
                   const user = context.user;
                   const mongodb = context.services.get(")" +
                                                          util::format("db-%1", app_name) + R"(");
                   const objectCollection = mongodb.db(")" +
                                                          util::format("test_data") + R"(").collection("client_reset_obj");
                   doc = await objectCollection.deleteMany({});
                   return doc;
               };
           )"}}));

        static_cast<void>(app["functions"].post({{"name", "insertClientResetObject"},
                                                 {"private", false},
                                                 {"can_evaluate", {}},
                                                 {"source",
                                                  R"(
               exports = async function(data) {
                   const user = context.user;
                   const mongodb = context.services.get(")" +
                                                          util::format("db-%1", app_name) + R"(");
                   const objectCollection = mongodb.db(")" +
                                                          util::format("test_data") + R"(").collection("client_reset_obj");
                   doc = await objectCollection.insertOne({ _id: 1, str_col: 'remote obj' });
                   return doc;
               };
           )"}}));

        bson::BsonDocument userData = {
                {"schema", bson::BsonDocument{
                                   {"properties", bson::BsonDocument{
                                                          {"_id", bson::BsonDocument{{"bsonType", "objectId"}}},
                                                          {"name", bson::BsonDocument{{"bsonType", "string"}}},
                                                          {"user_id", bson::BsonDocument{{"bsonType", "string"}}},
                                                  }},
                                   {"required", bson::BsonArray{"_id", "name", "user_id"}},
                                   {"title", "UserData"}}},
                {"metadata", bson::BsonDocument{{"data_source", util::format("db-%1", app_name)}, {"database", "test_data"}, {"collection", "UserData"}}}};

        auto embeddedFooSchema = bson::BsonDocument{
                {"title", "EmbeddedFoo"},
                {"bsonType", "object"},
                {"required", bson::BsonArray({"bar"})},
                {"properties", bson::BsonDocument{
                                       {"bar", bson::BsonDocument{{"bsonType", "long"}}}}}};

        bson::BsonDocument asymmetricObject = {
                {"schema", bson::BsonDocument{
                                   {"properties", bson::BsonDocument{
                                                          {"_id", bson::BsonDocument{{"bsonType", "objectId"}}},
                                                          {"bool_col", bson::BsonDocument{{"bsonType", "bool"}}},
                                                          {"str_col", bson::BsonDocument{{"bsonType", "string"}}},
                                                          {"enum_col", bson::BsonDocument{{"bsonType", "long"}}},
                                                          {"date_col", bson::BsonDocument{{"bsonType", "date"}}},
                                                          {"uuid_col", bson::BsonDocument{{"bsonType", "uuid"}}},
                                                          {"binary_col", bson::BsonDocument{{"bsonType", "binData"}}},
                                                          {"mixed_col", bson::BsonDocument{{"bsonType", "mixed"}}},
                                                          {"opt_int_col", bson::BsonDocument{{"bsonType", "long"}}},
                                                          {"opt_str_col", bson::BsonDocument{{"bsonType", "string"}}},
                                                          {"opt_bool_col", bson::BsonDocument{{"bsonType", "bool"}}},
                                                          {"opt_binary_col", bson::BsonDocument{{"bsonType", "binData"}}},
                                                          {"opt_date_col", bson::BsonDocument{{"bsonType", "date"}}},
                                                          {"opt_enum_col", bson::BsonDocument{{"bsonType", "long"}}},
                                                          {"opt_embedded_obj_col", embeddedFooSchema},
                                                          {"opt_uuid_col", bson::BsonDocument{{"bsonType", "uuid"}}},

                                                          {"list_int_col", bson::BsonDocument{{"bsonType", "array"}, {"items", bson::BsonDocument{{"bsonType", "long"}}}}},
                                                          {"list_bool_col", bson::BsonDocument{{"bsonType", "array"}, {"items", bson::BsonDocument{{"bsonType", "bool"}}}}},
                                                          {"list_str_col", bson::BsonDocument{{"bsonType", "array"}, {"items", bson::BsonDocument{{"bsonType", "string"}}}}},
                                                          {"list_uuid_col", bson::BsonDocument{{"bsonType", "array"}, {"items", bson::BsonDocument{{"bsonType", "uuid"}}}}},
                                                          {"list_binary_col", bson::BsonDocument{{"bsonType", "array"}, {"items", bson::BsonDocument{{"bsonType", "binData"}}}}},
                                                          {"list_date_col", bson::BsonDocument{{"bsonType", "array"}, {"items", bson::BsonDocument{{"bsonType", "date"}}}}},
                                                          {"list_mixed_col", bson::BsonDocument{{"bsonType", "array"}, {"items", bson::BsonDocument{{"bsonType", "mixed"}}}}},
                                                          {"list_embedded_obj_col", bson::BsonDocument{{"bsonType", "array"}, {"items", embeddedFooSchema}}},

                                                          {"map_int_col", bson::BsonDocument{{"bsonType", "object"}, {"additionalProperties", bson::BsonDocument{{"bsonType", "long"}}}}},
                                                          {"map_str_col", bson::BsonDocument{{"bsonType", "object"}, {"additionalProperties", bson::BsonDocument{{"bsonType", "string"}}}}},
                                                          {"map_bool_col", bson::BsonDocument{{"bsonType", "object"}, {"additionalProperties", bson::BsonDocument{{"bsonType", "bool"}}}}},
                                                          {"map_enum_col", bson::BsonDocument{{"bsonType", "object"}, {"additionalProperties", bson::BsonDocument{{"bsonType", "long"}}}}},
                                                          {"map_date_col", bson::BsonDocument{{"bsonType", "object"}, {"additionalProperties", bson::BsonDocument{{"bsonType", "date"}}}}},
                                                          {"map_uuid_col", bson::BsonDocument{{"bsonType", "object"}, {"additionalProperties", bson::BsonDocument{{"bsonType", "uuid"}}}}},
                                                          {"map_mixed_col", bson::BsonDocument{{"bsonType", "object"}, {"additionalProperties", bson::BsonDocument{{"bsonType", "mixed"}}}}},
                                                          {"map_embedded_col", bson::BsonDocument{{"bsonType", "object"}, {"additionalProperties", embeddedFooSchema}}},
                                                  }},
                                   {"required", bson::BsonArray{"_id", "bool_col", "str_col", "enum_col", "date_col", "uuid_col", "binary_col"}},
                                   {"title", "AllTypesAsymmetricObject"}}},
                {"metadata", bson::BsonDocument{{"data_source", util::format("db-%1", app_name)}, {"database", "test_data"}, {"collection", "AllTypesAsymmetricObject"}}}};

        if (is_asymmetric)
            static_cast<void>(app["schemas"].post(std::move(asymmetricObject)));

        bson::BsonDocument mongodb_service_response(app["services"].post({{"name", util::format("db-%1", app_name)},
                                                                          {"type", mongodb_service_type},
                                                                          {"config", mongodb_service_config}}));
        std::string mongodb_service_id(mongodb_service_response["_id"]);
        m_service_id = mongodb_service_id;

        app["custom_user_data"].patch(bson::BsonDocument{
                {"mongo_service_id", mongodb_service_id},
                {"enabled", true},
                {"database_name", "test_data"},
                {"collection_name", "UserData"},
                {"user_id_field", "user_id"},
        });

        bson::BsonDocument user_data_rule = {
                {"database", "test_data"},
                {"collection", "UserData"},
                {"roles", bson::BsonArray{
                                  bson::BsonDocument{{"name", "default"},
                                                     {"apply_when", {}},
                                                     {"insert", true},
                                                     {"delete", true},
                                                     {"additional_fields", {}},
                                                     {"document_filters", bson::BsonDocument({{"read", true}, {"write", true}})},
                                                     {"read", true},
                                                     {"write", true},
                                                     {"insert", true},
                                                     {"delete", true}}}}};

        bson::BsonDocument asymmetric_object_rule = {
                {"database", app_name},
                {"collection", "AllTypesAsymmetricObject"},
                {"roles", bson::BsonArray{
                                  bson::BsonDocument{{"name", "default"},
                                                     {"apply_when", {}},
                                                     {"insert", true},
                                                     {"delete", true},
                                                     {"additional_fields", {}},
                                                     {"document_filters", bson::BsonDocument({{"read", true}, {"write", true}})},
                                                     {"read", true},
                                                     {"write", true},
                                                     {"insert", true},
                                                     {"delete", true}}}}};

        static_cast<void>(app["services"][static_cast<std::string>(mongodb_service_id)]["rules"].post(user_data_rule));
        static_cast<void>(app["services"][static_cast<std::string>(mongodb_service_id)]["rules"].post(asymmetric_object_rule));

        bson::BsonDocument service_config = {
                {"flexible_sync", bson::BsonDocument{
                                          {"type", "flexible"},
                                          {"state", "enabled"},
                                          {"database_name", "test_data"},
                                          {"enabled", true},
                                          {"is_recovery_mode_disabled", recovery_mode_disabled},
                                          {"queryable_fields_names", queryable_fields},
                                          {"asymmetric_tables", bson::BsonArray({"AllTypesAsymmetricObject"})}

                                  }}};

        bson::BsonDocument default_rule = {{"roles", bson::BsonArray({bson::BsonDocument({{"name", "all"},
                                                                                          {"apply_when", {}},
                                                                                          {"document_filters", bson::BsonDocument({{"read", true}, {"write", true}})},
                                                                                          {"write", true},
                                                                                          {"read", true},
                                                                                          {"insert", true},
                                                                                          {"delete", true}})})}};

        static_cast<void>(app["services"][mongodb_service_id]["default_rule"].post(default_rule));

        // The cluster linking must be separated from enabling sync because Atlas
        // takes a few seconds to provision a user for BaaS, meaning enabling sync
        // will fail if we attempt to do it with the same request. It's nondeterministic
        // how long it'll take, so we must retry for a while.
        constexpr int max_attempts = 120;
        for (int attempt = 0; attempt <= max_attempts; attempt++) {
            try {
                app["services"][mongodb_service_id]["config"].patch(std::move(service_config));
                break;
            } catch (std::exception &) {
                if (attempt == max_attempts) {
                    REALM_TERMINATE("Could not link Atlas cluster");
                } else {
                    util::format(std::cerr, "Could not update MongoDB service after %1 seconds. Will keep retrying.\n", (attempt + 1) * 5);
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                }
            }
        }

        app["sync"]["config"].put({{"development_mode_enabled", true}});

        return *static_cast<std::optional<std::string>>(info["client_app_id"]);
    }

    void Admin::Session::cache_app_id(const std::string &app_id) {
        m_cached_app_id = app_id;
    }


    [[nodiscard]] std::string Admin::Session::cached_app_id() const {
        REALM_ASSERT(m_cached_app_id);
        return *m_cached_app_id;
    }

    void Admin::Session::prepare(const std::optional<std::string> &baas_url) {
        if (baas_url)
            m_base_url = *baas_url;
        else
            m_base_url = "http://localhost:9090";
        bson::BsonDocument credentials{
                {"username", "unique_user@domain.com"},
                {"password", "password"}};
        std::pair<std::string, std::string> tokens = authenticate(m_base_url, "local-userpass", std::move(credentials));

        app::Request request;
        request.url = m_base_url + "/api/admin/v3.0/auth/profile";
        request.headers = {
                {"Authorization", "Bearer " + tokens.first}};

        auto result = do_http_request(std::move(request));
        auto parsed_response = static_cast<bson::BsonDocument>(bson::parse(result.body));
        auto roles = static_cast<bson::BsonArray>(parsed_response["roles"]);
        auto group_id = static_cast<std::string>(static_cast<bson::BsonDocument>(roles[0])["group_id"]);

        m_access_token = tokens.first;
        m_refresh_token = tokens.second;
        m_group_id = group_id;

        group = Endpoint(util::format("%1/api/admin/v3.0/groups/%2", m_base_url, group_id), m_access_token);
        apps = group["apps"];
    }

    void Admin::Session::enable_sync() {
        bson::BsonDocument service_config = {
                {"flexible_sync", bson::BsonDocument{
                                          {"state", "enabled"},
                                          {"is_recovery_mode_disabled", recovery_mode_disabled},
                                          {"enabled", true}}}};

        apps[m_app_id]["services"][m_service_id]["config"].patch(std::move(service_config));
        for (int i = 0; i < 5; ++i) {
            auto c = apps[m_app_id]["services"][m_service_id]["config"].get();
            if (c.to_string().find("enabled") != std::string::npos) {
                return;
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        std::runtime_error("Sync could not be enabled in time.");
    }
    void Admin::Session::disable_sync() {
        bson::BsonDocument service_config = {
                {"flexible_sync", bson::BsonDocument{
                                          {"state", "disabled"},
                                          {"enabled", false}}}};

        apps[m_app_id]["services"][m_service_id]["config"].patch(std::move(service_config));
        for (int i = 0; i < 5; ++i) {
            auto c = apps[m_app_id]["services"][m_service_id]["config"].get();
            if (c.to_string().find("disabled") != std::string::npos) {
                return;
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        std::runtime_error("Sync could not be disabled in time.");
    }

    void Admin::Session::refresh_access_token() {
        std::stringstream body;
        auto request = app::Request();
        request.method = realm::app::HttpMethod::post;
        request.url = util::format("%1/api/admin/v3.0/auth/session", m_base_url);
        request.headers = {
                {"Content-Type", "application/json;charset=utf-8"},
                {"Accept", "application/json"},
                {"Authorization", util::format("Bearer %1", m_refresh_token)}};
        request.body = body.str();

        auto result = do_http_request(std::move(request));
        if (result.http_status_code >= 400) {
            REALM_TERMINATE("Unable to refresh access token");
        }
        auto parsed_response = static_cast<bson::BsonDocument>(bson::parse(result.body));
        m_access_token = static_cast<std::string>(parsed_response["access_token"]);
    }

    void Admin::Session::trigger_client_reset(int64_t file_ident) const
    {
        auto app = apps[m_app_id]["sync"]["force_reset"].request(app::HttpMethod::put, util::format("{\"file_ident\": %1}", file_ident));
    }
}
