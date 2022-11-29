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
#include <curl/curl.h>

namespace realm::internal {

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
    REALM_ASSERT(response);
    size_t realsize = size * nmemb;
    response->append(ptr, realsize);
    return realsize;
}

static size_t curl_header_cb(char* buffer, size_t size, size_t nitems, std::map<std::string, std::string>* response_headers)
{
    REALM_ASSERT(response_headers);
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

static app::Response do_http_request(const app::Request& request)
{
    CurlGlobalGuard curl_global_guard;
    auto curl = curl_easy_init();
    if (!curl) {
        return app::Response{500, -1};
    }

    struct curl_slist* list = nullptr;
    auto curl_cleanup = util::ScopeExit([&]() noexcept {
        curl_easy_cleanup(curl);
        curl_slist_free_all(list);
    });

    std::string response;
    app::HttpHeaders response_headers;

    /* First set the URL that is about to receive our POST. This URL can
     just as well be a https:// URL if that is what should receive the
     data. */
    curl_easy_setopt(curl, CURLOPT_URL, request.url.c_str());

    /* Now specify the POST data */
    if (request.method == app::HttpMethod::post) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
    }
    else if (request.method == app::HttpMethod::put) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
    }
    else if (request.method == app::HttpMethod::patch) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
    }
    else if (request.method == app::HttpMethod::del) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
    }
    else if (request.method == app::HttpMethod::patch) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, request.timeout_ms);

    for (auto header : request.headers) {
        auto header_str = util::format("%1: %2", header.first, header.second);
        list = curl_slist_append(list, header_str.c_str());
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
    int http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    return {
        http_code,
        0, // binding_response_code
        std::move(response_headers),
        std::move(response),
    };
}
} // namespace

void DefaultTransport::send_request_to_server(const app::Request& request,
                                              util::UniqueFunction<void(const app::Response&)>&& completion_block)
{
    completion_block(do_http_request(request));
}
 

} // namespace realm::internal
