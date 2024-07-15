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
#include <cpprealm/networking/platform_networking.hpp>

#include <realm/util/to_string.hpp>

#include <curl/curl.h>
#include <cstring>

namespace realm::networking {

    namespace {

        class CurlGlobalGuard {
        public:
            CurlGlobalGuard()
            {
                std::lock_guard<std::mutex> lk(m_mutex);
                if (++m_users == 1) {
                    curl_global_init(CURL_GLOBAL_ALL);
                }
            }

            ~CurlGlobalGuard()
            {
                std::lock_guard<std::mutex> lk(m_mutex);
                if (--m_users == 0) {
                    curl_global_cleanup();
                }
            }

            CurlGlobalGuard(const CurlGlobalGuard&) = delete;
            CurlGlobalGuard(CurlGlobalGuard&&) = delete;
            CurlGlobalGuard& operator=(const CurlGlobalGuard&) = delete;
            CurlGlobalGuard& operator=(CurlGlobalGuard&&) = delete;

        private:
            static std::mutex m_mutex;
            static int m_users;
        };

        std::mutex CurlGlobalGuard::m_mutex = {};
        int CurlGlobalGuard::m_users = 0;

        static size_t curl_write_cb(char* ptr, size_t size, size_t nmemb, std::string* response)
        {
            size_t realsize = size * nmemb;
            response->append(ptr, realsize);
            return realsize;
        }

        static size_t curl_header_cb(char* buffer, size_t size, size_t nitems, std::map<std::string, std::string>* response_headers)
        {
            std::string combined(buffer, size * nitems);
            if (auto pos = combined.find(':'); pos != std::string::npos) {
                std::string key = combined.substr(0, pos);
                std::string value = combined.substr(pos + 1);
                while (value.size() > 0 && value[0] == ' ') {
                    value = value.substr(1);
                }
                while (value.size() > 0 && (value[value.size() - 1] == '\r' || value[value.size() - 1] == '\n')) {
                    value = value.substr(0, value.size() - 1);
                }
                response_headers->insert({key, value});
            }
            else {
                if (combined.size() > 5 && combined.substr(0, 5) != "HTTP/") { // ignore for now HTTP/1.1 ...
                    std::cerr << "test transport skipping header: " << combined << std::endl;
                }
            }
            return nitems * size;
        }
    } // namespace

    using SSLVerifyCallback = std::function<bool(const std::string& server_address,
                                   int server_port,
                                   const char* pem_data, size_t pem_size,
                                   int preverify_ok, int depth)>;

    CURLcode ssl_ctx_callback(CURL *curl, void */*sslctx*/, SSLVerifyCallback *parm) {
        auto verify_callback = (SSLVerifyCallback)(*parm);

        char *url;
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
        std::string server_address(url);

        long port;
        curl_easy_getinfo(curl, CURLINFO_PRIMARY_PORT, &port);

        const char *pem_data = "-----BEGIN CERTIFICATE-----\n...certificate data...\n-----END CERTIFICATE-----\n";
        size_t pem_size = std::strlen(pem_data);

        int preverify_ok = 1;
        int depth = 0;

        bool result = verify_callback(server_address, port, pem_data, pem_size, preverify_ok, depth);
        return result ? CURLE_OK : CURLE_SSL_CERTPROBLEM;
    }

    void default_http_transport::send_request_to_server(const ::realm::networking::request& request,
                                                        std::function<void(const ::realm::networking::response&)>&& completion_block) {
        CurlGlobalGuard curl_global_guard;
        auto curl = curl_easy_init();
        if (!curl) {
            completion_block(::realm::networking::response{500, -1, {}, "", std::nullopt});
            return;
        }

        struct curl_slist* list = nullptr;

        std::string response;
        ::realm::networking::http_headers response_headers;

        curl_easy_setopt(curl, CURLOPT_URL, request.url.c_str());


        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, m_configuration.client_validate_ssl);

        if (m_configuration.ssl_verify_callback) {
            curl_easy_setopt(curl, CURLOPT_SSL_CTX_FUNCTION, ssl_ctx_callback);
            curl_easy_setopt(curl, CURLOPT_SSL_CTX_DATA, &m_configuration.ssl_verify_callback);
        }

        if (m_configuration.ssl_trust_certificate_path) {
            curl_easy_setopt(curl, CURLOPT_CAINFO, m_configuration.ssl_trust_certificate_path->c_str());
        }

        if (m_configuration.proxy_config) {
            curl_easy_setopt(curl, CURLOPT_PROXY, util::format("%1:%2", m_configuration.proxy_config->address, m_configuration.proxy_config->port).c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1L);
            if (m_configuration.proxy_config->username_password) {
                curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, util::format("%1:%2", m_configuration.proxy_config->username_password->first, m_configuration.proxy_config->username_password->second).c_str());
            }
        }

        /* Now specify the POST data */
        if (request.method == ::realm::networking::http_method::post) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
        }
        else if (request.method == ::realm::networking::http_method::put) {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
        }
        else if (request.method == ::realm::networking::http_method::patch) {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
        }
        else if (request.method == ::realm::networking::http_method::del) {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
        }
        else if (request.method == ::realm::networking::http_method::patch) {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_TIMEOUT, request.timeout_ms);

        for (auto header : request.headers) {
            auto header_str = util::format("%1: %2", header.first, header.second);
            list = curl_slist_append(list, header_str.c_str());
        }
        if (m_configuration.custom_http_headers) {
            for (auto header : *m_configuration.custom_http_headers) {
                auto header_str = util::format("%1: %2", header.first, header.second);
                list = curl_slist_append(list, header_str.c_str());
            }
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_header_cb);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response_headers);

        auto response_code = curl_easy_perform(curl);
        if (response_code != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed when sending request to '%s' with body '%s': %s\n",
                    request.url.c_str(), request.body.c_str(), curl_easy_strerror(response_code));
        }
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        curl_easy_cleanup(curl);
        curl_slist_free_all(list);
        completion_block({
                static_cast<int>(http_code),
                0, // binding_response_code
                std::move(response_headers),
                std::move(response),
                std::nullopt
        });
    }
} // namespace realm::networking
