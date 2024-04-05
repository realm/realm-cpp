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

#pragma once

#include <cpprealm/app.hpp>
#include <realm/object-store/sync/app.hpp>

#include <mutex>

namespace Admin {
    using namespace realm;

    struct baas_manager {
        explicit baas_manager(const std::optional<std::string>& baasaas_api_key = std::nullopt)
            : m_baasaas_api_key(baasaas_api_key) {
            if (!m_baasaas_api_key) {
                m_baasaas_api_key = getenv("BAAS_APIKEY");
            }
        }
        ~baas_manager() {
            if (m_container_id)
                stop(*m_container_id);
        }

        // Starts a baasaas container, this container will stop when the baas_manager
        // instance goes out of scope.
        void start();
        // Stops a given container and nulls the current container_id.
        void stop(const std::string& conainter_id) const;
        // Polls the current container and returns the url once available.
        const std::string wait_for_container() const;
        const std::string container_id() const;
    private:
        std::optional<std::string> m_container_id;
        std::optional<std::string> m_baasaas_api_key;
        const std::string m_baas_base_url = "https://us-east-1.aws.data.mongodb-api.com/app/baas-container-service-autzb/endpoint";
    };

    struct Endpoint {
    public:
        Endpoint() = default;
        Endpoint(std::string url, std::string access_token)
        : m_url(std::move(url))
        , m_access_token(std::move(access_token))
        { }

        app::Response request(app::HttpMethod method, bson::BsonDocument&& body = {}) const;
        app::Response request(app::HttpMethod method, const std::string& body) const;

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
            return {m_url + "/" + url, m_access_token};
        }
    private:
        std::string m_access_token;
        std::string m_url;
    };

    struct Session {
        Session(const Session&) = delete;
        Session& operator=(const Session&) = delete;

        Endpoint group;
        Endpoint apps;

        static Session* instance;
        static std::mutex mutex;

        void prepare(const std::optional<std::string>& baas_url = std::nullopt);

        const std::string& base_url() const { return m_base_url; }

        std::string create_app(bson::BsonArray queryable_fields = {}, std::string name = "test", bool is_asymmetric = false, bool disable_recovery_mode = true);
        [[nodiscard]] std::string cached_app_id() const;
        void cache_app_id(const std::string& app_id);
        void enable_sync();
        void disable_sync();
        void refresh_access_token();

        static Session& shared() {
            std::lock_guard<std::mutex> lock(mutex);
            if (instance == nullptr) {
                instance = new Session();
            }
            return *instance;
        }
    private:
        Session() = default;

        std::string m_base_url;
        std::string m_access_token;
        std::string m_refresh_token;
        std::string m_group_id;
        std::optional<std::string> m_cluster_name;
        std::optional<std::string> m_cached_app_id; // client app id
        std::string m_service_id;
        std::string m_app_id;
        bool recovery_mode_disabled = false;
    };

} // namespace Admin

