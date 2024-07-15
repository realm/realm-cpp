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

} //namespace realm::networking

#endif//CPPREALM_NETWORKING_HPP
