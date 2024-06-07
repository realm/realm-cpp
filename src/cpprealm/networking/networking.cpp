#include <cpprealm/networking/networking.hpp>

#include <realm/object-store/sync/generic_network_transport.hpp>

namespace realm::networking {
    request to_request(const ::realm::app::Request& core_request) {
        request req;
        req.method = static_cast<http_method>(core_request.method);
        req.url = core_request.url;
        req.timeout_ms = core_request.timeout_ms;
        req.headers = core_request.headers;
        req.body = core_request.body;
        return req;
    }
    ::realm::app::Request to_core_request(const request& req) {
        ::realm::app::Request core_request;
        core_request.method = static_cast<app::HttpMethod>(req.method);
        core_request.url = req.url;
        core_request.timeout_ms = req.timeout_ms;
        core_request.headers = req.headers;
        core_request.body = req.body;
        return core_request;
    }

    response to_response(const ::realm::app::Response& core_response) {
        response req;
        req.http_status_code = core_response.http_status_code;
        req.custom_status_code = core_response.custom_status_code;
        req.headers = core_response.headers;
        req.client_error_code = core_response.client_error_code;
        req.body = core_response.body;
        return req;
    }
    ::realm::app::Response to_core_response(const response& req) {
        ::realm::app::Response core_response;
        core_response.http_status_code = req.http_status_code;
        core_response.custom_status_code = req.custom_status_code;
        core_response.headers = req.headers;
        core_response.body = req.body;
        if (req.client_error_code) {
            core_response.client_error_code = static_cast<ErrorCodes::Error>(*req.client_error_code);
        }
        return core_response;
    }
}