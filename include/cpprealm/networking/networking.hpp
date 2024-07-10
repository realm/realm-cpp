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

#ifndef CPPREALM_NETWORKING_HPP
#define CPPREALM_NETWORKING_HPP

#include <map>
#include <cpprealm/internal/bridge/realm.hpp>

namespace realm::networking {
    /**
     * An HTTP method type.
     */
    enum class http_method { get, post, patch, put, del };
    /**
     * Request/Response headers type
     */
    using http_headers = std::map<std::string, std::string>;

    /**
     * An HTTP request that can be made to an arbitrary server.
     */
    struct request {
        /**
         * The HTTP method of this request.
         */
        http_method method = http_method::get;

        /**
         * The URL to which this request will be made.
         */
        std::string url;

        /**
         * The number of milliseconds that the underlying transport should spend on an HTTP round trip before failing with
         * an error.
         */
        uint64_t timeout_ms = 0;

        /**
         * The HTTP headers of this request - keys are case insensitive.
         */
        http_headers headers;

        /**
         * The body of the request.
         */
        std::string body;
    };

    /**
     * The contents of an HTTP response.
     */
    struct response {
        /**
         * The status code of the HTTP response.
         */
        int http_status_code;

        /**
         * A custom status code provided by the language binding (SDK).
         */
        int custom_status_code;

        /**
         * The headers of the HTTP response - keys are case insensitive.
         */
        http_headers headers;

        /**
         * The body of the HTTP response.
         */
        std::string body;

        /**
         * An error code used by the client to report http processing errors.
         */
        std::optional<std::int32_t> client_error_code;

    };

    struct websocket_endpoint {
        /// Array of one or more websocket protocols.
        std::vector<std::string> protocols;
        /// The websocket url to connect to.
        std::string url;
    };

    enum websocket_err_codes {
        RLM_ERR_WEBSOCKET_OK = 1000,
        RLM_ERR_WEBSOCKET_GOINGAWAY = 1001,
        RLM_ERR_WEBSOCKET_PROTOCOLERROR = 1002,
        RLM_ERR_WEBSOCKET_UNSUPPORTEDDATA = 1003,
        RLM_ERR_WEBSOCKET_RESERVED = 1004,
        RLM_ERR_WEBSOCKET_NOSTATUSRECEIVED = 1005,
        RLM_ERR_WEBSOCKET_ABNORMALCLOSURE = 1006,
        RLM_ERR_WEBSOCKET_INVALIDPAYLOADDATA = 1007,
        RLM_ERR_WEBSOCKET_POLICYVIOLATION = 1008,
        RLM_ERR_WEBSOCKET_MESSAGETOOBIG = 1009,
        RLM_ERR_WEBSOCKET_INAVALIDEXTENSION = 1010,
        RLM_ERR_WEBSOCKET_INTERNALSERVERERROR = 1011,
        RLM_ERR_WEBSOCKET_TLSHANDSHAKEFAILED = 1015,

        RLM_ERR_WEBSOCKET_UNAUTHORIZED = 4001,
        RLM_ERR_WEBSOCKET_FORBIDDEN = 4002,
        RLM_ERR_WEBSOCKET_MOVEDPERMANENTLY = 4003,
        RLM_ERR_WEBSOCKET_CLIENT_TOO_OLD = 4004,
        RLM_ERR_WEBSOCKET_CLIENT_TOO_NEW = 4005,
        RLM_ERR_WEBSOCKET_PROTOCOL_MISMATCH = 4006,

        RLM_ERR_WEBSOCKET_RESOLVE_FAILED = 4400,
        RLM_ERR_WEBSOCKET_CONNECTION_FAILED = 4401,
        RLM_ERR_WEBSOCKET_READ_ERROR = 4402,
        RLM_ERR_WEBSOCKET_WRITE_ERROR = 4403,
        RLM_ERR_WEBSOCKET_RETRY_ERROR = 4404,
        RLM_ERR_WEBSOCKET_FATAL_ERROR = 4405,
    };

    // Interface for providing http transport
    struct http_transport_client {
        virtual ~http_transport_client() = default;
        virtual void send_request_to_server(const request& request,
                                            std::function<void(const response&)>&& completion) = 0;
    };

} //namespace realm::networking

namespace realm {
    namespace app {
        struct Request;
        struct Response;
    }
    namespace sync {
        struct WebSocketEndpoint;
    }
}

namespace realm::internal::networking {
    ::realm::networking::request to_request(const ::realm::app::Request&);
    ::realm::app::Request to_core_request(const ::realm::networking::request&);

    ::realm::networking::response to_response(const ::realm::app::Response&);
    ::realm::app::Response to_core_response(const ::realm::networking::response&);

    ::realm::sync::WebSocketEndpoint to_core_websocket_endpoint(const ::realm::networking::websocket_endpoint& ep,
                                                                const std::optional<internal::bridge::realm::sync_config::proxy_config>& pc,
                                                                const std::optional<std::map<std::string, std::string>>& custom_headers);
    ::realm::networking::websocket_endpoint to_websocket_endpoint(const ::realm::sync::WebSocketEndpoint& ep);
} //namespace realm::internal::networking

#endif//CPPREALM_NETWORKING_HPP
