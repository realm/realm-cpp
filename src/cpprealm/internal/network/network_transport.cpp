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

#if __has_include(<realm/util/config.h>)
#include <realm/util/config.h>
#endif
#include <cpprealm/app.hpp>
#include <cpprealm/networking/platform_networking.hpp>
#include <cpprealm/networking/networking.hpp>

#include <realm/object-store/sync/generic_network_transport.hpp>
#include <realm/sync/network/http.hpp>
#include <realm/sync/network/network.hpp>
#include <realm/sync/network/network_ssl.hpp>
#include <realm/util/base64.hpp>
#include <realm/util/uri.hpp>

#include <regex>

namespace realm::networking {

    struct DefaultSocket : realm::sync::network::Socket {
        DefaultSocket(realm::sync::network::Service& service)
            : realm::sync::network::Socket(service)
        {
        }

        template <class H>
        void async_write(const char* data, std::size_t size, H handler)
        {
            if (ssl_stream) {
                ssl_stream->async_write(data, size, std::move(handler)); // Throws
            }
            else {
                realm::sync::network::Socket::async_write(data, size, std::move(handler)); // Throws
            }
        }

        template <class H>
        void async_read_until(char* buffer, std::size_t size, char delim, H handler)
        {
            if (ssl_stream) {
                ssl_stream->async_read_until(buffer, size, delim, m_read_buffer, std::move(handler));
            } else {
                realm::sync::network::Socket::async_read_until(buffer, size, delim, m_read_buffer, std::move(handler));
            }
        }

        template <class H>
        void async_read(char* buffer, std::size_t size, H handler)
        {
            if (ssl_stream) {
                ssl_stream->async_read(buffer, size, m_read_buffer, std::move(handler));
            } else {
                realm::sync::network::Socket::async_read(buffer, size, m_read_buffer, std::move(handler));
            }
        }

        template <class H>
        void async_handshake(H handler)
        {
            if (!ssl_stream) {
                REALM_TERMINATE("ssl_stream is not initialized");
            }
            ssl_stream->async_handshake(std::move(handler)); // Throws
        }

        util::Optional<realm::sync::network::ssl::Stream> ssl_stream;
    private:
        realm::sync::network::ReadAheadBuffer m_read_buffer;
    };

    inline bool is_valid_ipv4(const std::string& ip) {
        std::stringstream ss(ip);
        std::string segment;
        std::vector<std::string> segments;

        while (std::getline(ss, segment, '.')) {
            segments.push_back(segment);
        }
        if (segments.size() != 4) return false;

        for (const std::string& seg : segments) {
            if (seg.empty() || seg.size() > 3) return false;

            for (char c : seg) {
                if (!isdigit(c)) return false;
            }

            int num = std::stoi(seg);
            if (num < 0 || num > 255) return false;
        }

        return true;
    }

    enum class URLScheme {
        HTTP,
        HTTPS,
        Unknown
    };

    URLScheme get_url_scheme(const std::string& scheme) {
        if (scheme == "https:") {
            return URLScheme::HTTPS;
        } else if (scheme == "http:") {
            return URLScheme::HTTP;
        } else {
            return URLScheme::Unknown;
        }
    }

    void default_http_transport::send_request_to_server(const ::realm::networking::request& request,
                                                        std::function<void(const ::realm::networking::response&)>&& completion_block) {
        const auto uri = realm::util::Uri(request.url);
        std::string userinfo, host, port;
        uri.get_auth(userinfo, host, port);

        realm::sync::network::Service service;
        using namespace realm::sync::network::ssl;
        Context m_ssl_context;
        DefaultSocket socket{service};
        realm::sync::network::Endpoint ep;
        const bool is_localhost = (host == "localhost");
        const bool is_ipv4 = is_valid_ipv4(host);
        const URLScheme url_scheme = get_url_scheme(uri.get_scheme());

        if (port.empty()) {
            port = is_localhost ? "9090" : "443";
        }

        try {
            auto resolver = realm::sync::network::Resolver{service};
            if (m_configuration.proxy_config) {
                std::string proxy_userinfo, proxy_host, proxy_port;
                const auto proxy_uri = realm::util::Uri(m_configuration.proxy_config->address);
                proxy_uri.get_auth(proxy_userinfo, proxy_host, proxy_port);
                if (proxy_host.empty()) {
                    std::error_code e;
                    auto address = realm::sync::network::make_address(m_configuration.proxy_config->address, e);
                    if (e.value() > 0) {
                        ::realm::networking::response response;
                        response.custom_status_code = e.value();
                        response.body = e.message();
                        completion_block(std::move(response));
                        return;
                    }
                    ep = realm::sync::network::Endpoint(address, m_configuration.proxy_config->port);
                } else {
                    auto resolved = resolver.resolve(sync::network::Resolver::Query(proxy_host, std::to_string(m_configuration.proxy_config->port)));
                    ep = *resolved.begin();
                }
            } else {
                if (is_ipv4) {
                    std::error_code e;
                    auto address = realm::sync::network::make_address(host, e);
                    ep = realm::sync::network::Endpoint(address, stoi(port));
                } else {
                    auto resolved = resolver.resolve(sync::network::Resolver::Query(host, is_localhost ? "9090" : "443"));
                    ep = *resolved.begin();
                }
            }
            socket.connect(ep);
        } catch (...) {
            ::realm::networking::response response;
            response.custom_status_code = util::error::operation_aborted;
            completion_block(std::move(response));
            return;
        }

        auto logger = util::Logger::get_default_logger();

        if (m_configuration.proxy_config) {
            realm::sync::HTTPRequest req;
            req.method = realm::sync::HTTPMethod::Connect;
            req.headers.emplace("Host", util::format("%1:%2", host, port));

            if (m_configuration.proxy_config->username_password) {
                auto userpass = util::format("%1:%2", m_configuration.proxy_config->username_password->first, m_configuration.proxy_config->username_password->second);
                std::string encoded_userpass;
                encoded_userpass.resize(realm::util::base64_encoded_size(userpass.length()));
                realm::util::base64_encode(userpass, encoded_userpass);
                req.headers.emplace("Proxy-Authorization", util::format("Basic %1", encoded_userpass));
            }
            realm::sync::HTTPClient<DefaultSocket> m_proxy_client = realm::sync::HTTPClient<DefaultSocket>(socket, logger);
            auto handler = [&](realm::sync::HTTPResponse response, std::error_code ec) {
                if (ec && ec != util::error::operation_aborted) {
                    ::realm::networking::response res;
                    res.custom_status_code = util::error::operation_aborted;
                    completion_block(std::move(res));
                    return;
                }

                if (response.status != realm::sync::HTTPStatus::Ok) {
                    ::realm::networking::response res;
                    res.http_status_code = static_cast<uint16_t>(response.status);
                    completion_block(std::move(res));
                    return;
                }
                service.stop();
            };

            m_proxy_client.async_request(req, std::move(handler)); // Throws

            service.run_until_stopped();
            service.reset();
        }

#if REALM_INCLUDE_CERTS
        m_ssl_context.use_included_certificate_roots();
#endif
        if (m_configuration.ssl_trust_certificate_path) {
            m_ssl_context.use_certificate_chain_file(*m_configuration.ssl_trust_certificate_path);
        } else {
            m_ssl_context.use_default_verify();
        }

        if (url_scheme == URLScheme::HTTPS) {
            socket.ssl_stream.emplace(socket, m_ssl_context, Stream::client);
            socket.ssl_stream->set_host_name(host); // Throws

            socket.ssl_stream->set_verify_mode(VerifyMode::peer);
            socket.ssl_stream->set_logger(logger.get());
        }

        realm::sync::HTTPHeaders headers;
        for (auto& [k, v] : request.headers) {
            headers[k] = v;
        }
        headers["Host"] = host;
        headers["User-Agent"] = "Realm C++ SDK";

        if (!request.body.empty()) {
            headers["Content-Length"] = util::to_string(request.body.size());
        }

        if (m_configuration.custom_http_headers) {
            for (auto& header : *m_configuration.custom_http_headers) {
                headers.emplace(header);
            }
        }

        realm::sync::HTTPClient<DefaultSocket> m_http_client = realm::sync::HTTPClient<DefaultSocket>(socket, logger);
        realm::sync::HTTPMethod method;
        switch (request.method) {
            case ::realm::networking::http_method::get:
                method = realm::sync::HTTPMethod::Get;
                break;
            case ::realm::networking::http_method::put:
                method = realm::sync::HTTPMethod::Put;
                break;
            case ::realm::networking::http_method::post:
                method = realm::sync::HTTPMethod::Post;
                break;
            case ::realm::networking::http_method::patch:
                method = realm::sync::HTTPMethod::Patch;
                break;
            case ::realm::networking::http_method::del:
                method = realm::sync::HTTPMethod::Delete;
                break;
            default:
                REALM_UNREACHABLE();
        }

        /*
         * Flow of events:
         * 1. hostname is resolved from DNS
         * 2. socket establishes connection with host
         * 3. Runloop is enqeued with task via service.post
         * 4. SSL handshake is performed
         * 5. HTTP request is send over the wire
         * 6. Response is converted to object store type and passed back to SDK
         */

        service.post([&](realm::Status&&){
            auto handler = [&](std::error_code ec) {
                if (ec.value() == 0) {
                    realm::sync::HTTPRequest req;
                    req.method = method;
                    req.headers = headers;
                    req.path = request.url;
                    req.body = request.body.empty() ? std::nullopt : std::optional<std::string>(request.body);

                    m_http_client.async_request(std::move(req), [cb = std::move(completion_block)](const realm::sync::HTTPResponse& r, const std::error_code&) {
                        ::realm::networking::response res;
                        res.body = r.body ? *r.body : "";
                        for (auto& [k, v] : r.headers)  {
                            res.headers[k] = v;
                        }
                        res.http_status_code = static_cast<int>(r.status);
                        res.custom_status_code = 0;
                        cb(res);
                    });
                } else {
                    ::realm::networking::response response;
                    response.custom_status_code = util::error::operation_aborted;
                    completion_block(std::move(response));
                    return;
                }
            };
            if (url_scheme == URLScheme::HTTPS) {
                socket.async_handshake(std::move(handler)); // Throws
            } else {
                handler(std::error_code());
            }
        });

        service.run();
    }
} //namespace realm::networking

