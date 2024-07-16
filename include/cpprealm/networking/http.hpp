////////////////////////////////////////////////////////////////////////////
//
// Copyright 2024 Realm Inc.
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

#ifndef CPPREALM_NETWORKING_HTTP_HPP
#define CPPREALM_NETWORKING_HTTP_HPP

#include <cpprealm/networking/networking.hpp>
#include <cpprealm/internal/bridge/realm.hpp>

#ifndef REALMCXX_VERSION_MAJOR
#include <cpprealm/version_numbers.hpp>
#endif

#include <optional>

namespace realm::networking {

    // Interface for providing http transport
    struct http_transport_client {
        virtual ~http_transport_client() = default;
        virtual void send_request_to_server(const request& request,
                                            std::function<void(const response&)>&& completion) = 0;
    };

    /// Produces a http transport client from the factory.
    std::shared_ptr<http_transport_client> make_http_client();
    /// Globally overwrites the default http transport client factory
    [[maybe_unused]] void set_http_client_factory(std::function<std::shared_ptr<http_transport_client>()>&&);

    /// Built in HTTP transport client.
    struct default_http_transport : public http_transport_client {
        struct configuration {
            /**
             * Extra HTTP headers to be set on each request to Atlas Device Sync when using the internal HTTP client.
             */
            std::optional<std::map<std::string, std::string>> custom_http_headers;
            /**
             * Network proxy configuration to be set on each request.
             */
            std::optional<::realm::internal::bridge::realm::sync_config::proxy_config> proxy_config;

            using SSLVerifyCallback = bool(const std::string& server_address,
                                           internal::bridge::realm::sync_config::proxy_config::port_type server_port,
                                           const char* pem_data, size_t pem_size, int preverify_ok, int depth);
            /**
             * If set to false, no validation will take place and the client will accept any certificate.
             */
            bool client_validate_ssl = true;
            /**
             * Used for providing your own root certificate.
             */
            std::optional<std::string> ssl_trust_certificate_path;
            /**
             * `ssl_verify_callback` is used to implement custom SSL certificate
             * verification. It is only used if the protocol is SSL & `ssl_trust_certificate_path`
             * is not set.
             */
            std::function<SSLVerifyCallback> ssl_verify_callback;
        };

        default_http_transport() = default;
        default_http_transport(const configuration& c) : m_configuration(c) {}

        ~default_http_transport() = default;

        void send_request_to_server(const ::realm::networking::request& request,
                                    std::function<void(const ::realm::networking::response&)>&& completion);

    protected:
        configuration m_configuration;
    };
}

#endif //CPPREALM_NETWORKING_HTTP_HPP
