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

#include <cpprealm/app.hpp>
#include "realm/util/http.hpp"
#include <realm/util/network.hpp>
#include "realm/sync/noinst/client_impl_base.hpp"

namespace realm::internal {
    struct AndroidSocket : realm::util::network::Socket {
        AndroidSocket(realm::util::network::Service& service)
                : realm::util::network::Socket(service)
        {
        }

        template <class H>
        void async_write(const char* data, std::size_t size, H handler)
        {
            if (ssl_stream) {
                ssl_stream->async_write(data, size, std::move(handler)); // Throws
            }
            else {
                realm::util::network::Socket::async_write(data, size, std::move(handler)); // Throws
            }
        }

        template <class H>
        void async_read_until(char* buffer, std::size_t size, char delim, H handler)
        {
            if (ssl_stream) {
                ssl_stream->template async_read_until(buffer, size, delim, m_read_buffer, std::move(handler));
            } else {
                realm::util::network::Socket::async_read_until(buffer, size, delim, m_read_buffer, std::move(handler));
            }
        }

        template <class H>
        void async_read(char* buffer, std::size_t size, H handler)
        {
            if (ssl_stream) {
                ssl_stream->async_read(buffer, size, m_read_buffer, std::move(handler));
            } else {
                realm::util::network::Socket::async_read(buffer, size, m_read_buffer, std::move(handler));
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

        util::Optional<util::network::ssl::Stream> ssl_stream;
    private:
        realm::util::network::ReadAheadBuffer m_read_buffer;
    };

    void DefaultTransport::send_request_to_server(const app::Request& request,
                                                  util::UniqueFunction<void(const app::Response&)>&& completion_block) {
        // Get the host name.
        size_t found = request.url.find_first_of(":");
        std::string domain = request.url.substr(found+3);
        size_t found1 = domain.find_first_of(":");
        size_t found2 = domain.find_first_of("/");
        const std::string host = domain.substr(found1 + 1, found2 - found1 - 1);

        realm::util::network::Service service;
        auto resolver = realm::util::network::Resolver{service};
        auto resolved = resolver.resolve(util::network::Resolver::Query(host, "443"));
        realm::util::network::Endpoint ep = *resolved.begin();

        using namespace util::network::ssl;
        util::network::ssl::Context m_ssl_context;
        AndroidSocket socket{service};
        socket.connect(ep);

        socket.ssl_stream.emplace(socket, m_ssl_context, Stream::client);;
        socket.ssl_stream->set_host_name(host); // Throws

        realm::util::HTTPHeaders headers;
        for (auto& [k, v] : request.headers) {
            headers[k] = v;
        }
        headers["Host"] = host;
        headers["User-Agent"] = "Realm C++ SDK Android";

        if (!request.body.empty()) {
            headers["Content-Length"] = util::to_string(request.body.size());
        }

        auto logger = realm::util::StderrLogger();
        realm::util::HTTPClient<AndroidSocket> m_http_client = realm::util::HTTPClient<AndroidSocket>(socket, logger);

        realm::util::HTTPMethod method;
        switch (request.method) {
            case app::HttpMethod::get:
                method = realm::util::HTTPMethod::Get;
                break;
            case app::HttpMethod::put:
                method = realm::util::HTTPMethod::Put;
                break;
            case app::HttpMethod::post:
                method = realm::util::HTTPMethod::Post;
                break;
            case app::HttpMethod::del:
                method = realm::util::HTTPMethod::Delete;
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

        service.post([&]{
            auto handler = [&](std::error_code ec) {
                if (ec.value() == 0) {
                    m_http_client.async_request({.method = method,
                                                 .headers = headers,
                                                 .path = request.url,
                                                 .body = request.body.empty() ? std::nullopt : std::optional<std::string>(request.body)
                                                }, [cb = std::move(completion_block)](realm::util::HTTPResponse r, std::error_code e) {
                        app::Response res;
                        res.body = r.body ? *r.body : "";
                        for (auto& [k, v] : r.headers)  {
                            res.headers[k] = v;
                        }
                        res.http_status_code = static_cast<int>(r.status);
                        res.custom_status_code = 0;
                        cb(res);
                    });
                }
            };
            socket.async_handshake(std::move(handler)); // Throws
        });

        service.run();
    }
}