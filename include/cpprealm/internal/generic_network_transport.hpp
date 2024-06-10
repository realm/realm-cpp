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

#ifndef CPPREALM_GENERIC_NETWORK_TRANSPORT_CPP
#define CPPREALM_GENERIC_NETWORK_TRANSPORT_CPP

#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/networking/networking.hpp>

namespace realm::internal::networking {
    class DefaultTransport : public ::realm::networking::http_transport_client {
    public:
    DefaultTransport(const std::optional<std::map<std::string, std::string>>& custom_http_headers = std::nullopt,
                     const std::optional<bridge::realm::sync_config::proxy_config>& proxy_config = std::nullopt);
    ~DefaultTransport() = default;

    void send_request_to_server(const ::realm::networking::request& request,
                                std::function<void(const ::realm::networking::response&)>&& completion);
private:
    std::optional<std::map<std::string, std::string>> m_custom_http_headers;
    std::optional<bridge::realm::sync_config::proxy_config> m_proxy_config;
};

} // namespace realm::internal::networking

#endif //CPPREALM_GENERIC_NETWORK_TRANSPORT_CPP
