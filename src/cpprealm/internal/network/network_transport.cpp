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
#include <cpprealm/internal/generic_network_transport.hpp>

#include <realm/sync/network/http.hpp>
#include <realm/sync/network/network.hpp>
#include <realm/sync/noinst/client_impl_base.hpp>

namespace realm::internal {
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

    void DefaultTransport::send_request_to_server(const app::Request& request,
                                                  util::UniqueFunction<void(const app::Response&)>&& completion_block) {
        // Get the host name.
        size_t found = request.url.find_first_of(":");
        std::string domain = request.url.substr(found+3);
        size_t found1 = domain.find_first_of(":");
        size_t found2 = domain.find_first_of("/");
        const std::string host = domain.substr(found1 + 1, found2 - found1 - 1);


        realm::sync::network::Service service;
        using namespace realm::sync::network::ssl;
        Context m_ssl_context;
        DefaultSocket socket{service};


        if (m_proxy_config) {
            std::error_code e;
            auto address = realm::sync::network::make_address(m_proxy_config->address, e);
            if (e.value() != 0) {
                app::Response response;
                response.custom_status_code = util::error::operation_aborted;
                completion_block(std::move(response));
                return;
            }
            realm::sync::network::Endpoint ep = realm::sync::network::Endpoint(address, m_proxy_config->port);
            socket.connect(ep);
        } else {
            realm::sync::network::Endpoint ep;
            try {
                auto resolver = realm::sync::network::Resolver{service};
                auto resolved = resolver.resolve(sync::network::Resolver::Query(host, "443"));
                ep = *resolved.begin();
            } catch (...) {
                app::Response response;
                response.custom_status_code = util::error::operation_aborted;
                completion_block(std::move(response));
                return;
            }
            socket.connect(ep);
        }

        auto logger = util::Logger::get_default_logger();

        if (m_proxy_config) {
            realm::sync::HTTPRequest req;
            req.method = realm::sync::HTTPMethod::Connect;
            req.headers.emplace("Host", util::format("%1:%2", host, "443"));
            realm::sync::HTTPClient<DefaultSocket> m_proxy_client = realm::sync::HTTPClient<DefaultSocket>(socket, logger);
            auto handler = [&](realm::sync::HTTPResponse response, std::error_code ec) mutable {
                if (ec && ec != util::error::operation_aborted) {
                    return;
                }

                if (response.status != realm::sync::HTTPStatus::Ok) {
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

        socket.ssl_stream.emplace(socket, m_ssl_context, Stream::client);
        socket.ssl_stream->set_host_name(host); // Throws

        socket.ssl_stream->set_verify_mode(VerifyMode::peer);
        socket.ssl_stream->set_logger(logger.get());

        realm::sync::HTTPHeaders headers;
        for (auto& [k, v] : request.headers) {
            headers[k] = v;
        }
        headers["Host"] = host;
        headers["User-Agent"] = "Realm C++ SDK";

        if (!request.body.empty()) {
            headers["Content-Length"] = util::to_string(request.body.size());
        }

        if (m_custom_http_headers) {
            for (auto& header : *m_custom_http_headers) {
                headers.emplace(header);
            }
        }

        realm::sync::HTTPClient<DefaultSocket> m_http_client = realm::sync::HTTPClient<DefaultSocket>(socket, logger);
        realm::sync::HTTPMethod method;
        switch (request.method) {
            case app::HttpMethod::get:
                method = realm::sync::HTTPMethod::Get;
                break;
            case app::HttpMethod::put:
                method = realm::sync::HTTPMethod::Put;
                break;
            case app::HttpMethod::post:
                method = realm::sync::HTTPMethod::Post;
                break;
            case app::HttpMethod::del:
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

                    m_http_client.async_request(std::move(req), [cb = std::move(completion_block)](const realm::sync::HTTPResponse& r, const std::error_code& e) {
                        app::Response res;
                        res.body = r.body ? *r.body : "";
                        for (auto& [k, v] : r.headers)  {
                            res.headers[k] = v;
                        }
                        res.http_status_code = static_cast<int>(r.status);
                        res.custom_status_code = 0;
                        cb(res);
                    });
                } else {
                    app::Response response;
                    response.custom_status_code = util::error::operation_aborted;
                    completion_block(std::move(response));
                    return;
                }
            };
            socket.async_handshake(std::move(handler)); // Throws
        });

        service.run();
    }
}