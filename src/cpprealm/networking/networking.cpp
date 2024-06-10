#include <cpprealm/networking/networking.hpp>

#include <realm/object-store/sync/generic_network_transport.hpp>
#include <realm/sync/socket_provider.hpp>

namespace realm::internal::networking {
    ::realm::networking::request to_request(const ::realm::app::Request& core_request) {
        ::realm::networking::request req;
        req.method = static_cast<::realm::networking::http_method>(core_request.method);
        req.url = core_request.url;
        req.timeout_ms = core_request.timeout_ms;
        req.headers = core_request.headers;
        req.body = core_request.body;
        return req;
    }
    ::realm::app::Request to_core_request(const ::realm::networking::request& req) {
        ::realm::app::Request core_request;
        core_request.method = static_cast<::realm::app::HttpMethod>(req.method);
        core_request.url = req.url;
        core_request.timeout_ms = req.timeout_ms;
        core_request.headers = req.headers;
        core_request.body = req.body;
        return core_request;
    }

    ::realm::networking::response to_response(const ::realm::app::Response& core_response) {
        ::realm::networking::response req;
        req.http_status_code = core_response.http_status_code;
        req.custom_status_code = core_response.custom_status_code;
        req.headers = core_response.headers;
        req.client_error_code = core_response.client_error_code;
        req.body = core_response.body;
        return req;
    }
    ::realm::app::Response to_core_response(const ::realm::networking::response& req) {
        ::realm::app::Response core_response;
        core_response.http_status_code = req.http_status_code;
        core_response.custom_status_code = req.custom_status_code;
        core_response.headers = req.headers;
        core_response.body = req.body;
        if (req.client_error_code) {
            core_response.client_error_code = static_cast<::realm::ErrorCodes::Error>(*req.client_error_code);
        }
        return core_response;
    }

    ::realm::sync::WebSocketEndpoint to_core_websocket_endpoint(const ::realm::networking::ws_endpoint& ep) {
        ::realm::sync::WebSocketEndpoint core_ep;
        core_ep.address = ep.address;
        core_ep.port = ep.port;
        core_ep.path = ep.path;
        core_ep.protocols = ep.protocols;
        core_ep.is_ssl = ep.is_ssl;
        return core_ep;
    }

    ::realm::networking::ws_endpoint to_websocket_endpoint(const ::realm::sync::WebSocketEndpoint& core_ep) {
        ::realm::networking::ws_endpoint ep;
        ep.address = core_ep.address;
        ep.port = core_ep.port;
        ep.path = core_ep.path;
        ep.protocols = core_ep.protocols;
        ep.is_ssl = core_ep.is_ssl;
        return ep;
    }
} //namespace internal::networking