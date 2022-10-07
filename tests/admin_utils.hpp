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

namespace Admin {
using namespace realm;

struct Endpoint {
public:
    Endpoint(std::string url, std::string access_token)
    : m_url(std::move(url))
    , m_access_token(std::move(access_token))
    { }

    app::Response request(app::HttpMethod method, bson::BsonDocument&& body = {}) const;

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
    const std::string m_access_token;
    const std::string m_url;
};

struct Session {
    const Endpoint group;
    const Endpoint apps;

    static Session local(std::optional<std::string> baas_url = std::nullopt);

    static Session atlas(const std::string& baas_url, std::string project_id, std::string cluster_name, std::string api_key, std::string private_api_key);

    const std::string& base_url() const { return m_base_url; }

    [[nodiscard]] std::string create_app(bson::BsonArray queryable_fields = {}, std::string name = "test") const;
private:
    const std::string m_base_url;
    const std::string m_access_token;
    const std::optional<std::string> m_cluster_name;

    Session(const std::string& baas_url, const std::string& access_token, const std::string& group_id, std::optional<std::string> cluster_name = std::nullopt);
};

const Session& shared();

} // namespace Admin

