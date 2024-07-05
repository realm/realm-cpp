#include <cpprealm/networking/networking.hpp>

#include <realm/object-store/sync/generic_network_transport.hpp>
#include <realm/sync/socket_provider.hpp>
#include <realm/util/base64.hpp>
#include <realm/util/uri.hpp>
#include <realm/sync/protocol.hpp>

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

    ::realm::sync::ProtocolEnvelope to_protocol_envelope(const std::string& protocol) noexcept
    {
        if (protocol == "realm:") {
            return ::realm::sync::ProtocolEnvelope::realm;
        } else if (protocol == "realms:") {
            return ::realm::sync::ProtocolEnvelope::realms;
        } else if (protocol == "ws:") {
            return ::realm::sync::ProtocolEnvelope::ws;
        } else if (protocol == "wss:") {
            return ::realm::sync::ProtocolEnvelope::wss;
        }
        REALM_TERMINATE("Unrecognized websocket protocol");
    }

    ::realm::sync::WebSocketEndpoint to_core_websocket_endpoint(const ::realm::networking::websocket_endpoint& ep) {
        ::realm::sync::WebSocketEndpoint core_ep;

        auto uri = util::Uri(ep.url);
        auto protocol = to_protocol_envelope(uri.get_scheme());

        auto uri_path = uri.get_auth();
        if (uri_path.find("//") == 0) {
            uri_path = uri_path.substr(2);
        }

        std::string address;
        std::string port;
        size_t colon_pos = uri_path.find(':');
        if (colon_pos != std::string::npos) {
            // Extract the address
            address = uri_path.substr(0, colon_pos);
            // Extract the port
            port = uri_path.substr(colon_pos + 1);
        } else {
            REALM_TERMINATE("Invalid URL");
        }

        core_ep.address = address;
        core_ep.port = std::stoi(port);
        core_ep.path = uri.get_path() + uri.get_query();
        core_ep.protocols = ep.protocols;
        core_ep.is_ssl = ::realm::sync::is_ssl(protocol);

        if (ep.proxy_configuration) {
            core_ep.proxy = SyncConfig::ProxyConfig();
            core_ep.proxy->address = ep.proxy_configuration->address;
            core_ep.proxy->port = ep.proxy_configuration->port;
            if (ep.proxy_configuration->username_password) {
                auto userpass = util::format("%1:%2", ep.proxy_configuration->username_password->first, ep.proxy_configuration->username_password->second);
                std::string encoded_userpass;
                encoded_userpass.resize(realm::util::base64_encoded_size(userpass.length()));
                realm::util::base64_encode(userpass, encoded_userpass);
                core_ep.headers.emplace("Proxy-Authorization", util::format("Basic %1", encoded_userpass));
            }
        }

        return core_ep;
    }

    ::realm::networking::websocket_endpoint to_websocket_endpoint(const ::realm::sync::WebSocketEndpoint& core_ep,
                                                                  std::optional<internal::bridge::realm::sync_config::proxy_config> pc) {
        ::realm::networking::websocket_endpoint ep;
        ep.protocols = core_ep.protocols;
        const auto& port = core_ep.proxy ? core_ep.proxy->port : core_ep.port;
        ep.url = util::format("%1://%2:%3%4", core_ep.is_ssl ? "wss" : "ws", core_ep.proxy ? core_ep.proxy->address : core_ep.address, port, core_ep.path);
        ep.proxy_configuration = pc;
        return ep;
    }
} //namespace internal::networking
