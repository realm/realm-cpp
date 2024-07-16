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

#ifndef CPPREALM_NETWORKING_UTILS_HPP
#define CPPREALM_NETWORKING_UTILS_HPP

#include <cpprealm/networking/websocket.hpp>

#include <optional>

namespace realm {
    namespace app {
        struct Request;
        struct Response;
    }
    namespace sync {
        struct WebSocketEndpoint;
    }

    namespace networking {
        struct request;
        struct response;
    }
}

namespace realm::internal::networking {
    ::realm::networking::request to_request(const ::realm::app::Request&);
    ::realm::app::Response to_core_response(const ::realm::networking::response&);

    ::realm::sync::WebSocketEndpoint to_core_websocket_endpoint(const ::realm::networking::sync_socket_provider::websocket_endpoint& ep,
                                                                const std::optional<::realm::networking::default_socket_provider::configuration>& config);
    ::realm::networking::sync_socket_provider::websocket_endpoint to_websocket_endpoint(const ::realm::sync::WebSocketEndpoint& ep);
} //namespace realm::internal::networking

#endif //CPPREALM_NETWORKING_UTILS_HPP
