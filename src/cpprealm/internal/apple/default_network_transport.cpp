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

#include "../default_network_transport.hpp"

#include <CFNetwork/CFHTTPMessage.h>
#include <CFNetwork/CFHTTPStream.h>
#include <CoreFoundation/CFString.h>

using namespace realm;
using namespace realm::internal;

void DefaultTransport::send_request_to_server(app::Request&& request, app::HttpCompletion&& completion_block)
{
    auto url = CFStringCreateWithCString(kCFAllocatorDefault, request.url.c_str(), kCFStringEncodingUTF8);
    CFURLRef myURL = CFURLCreateWithString(kCFAllocatorDefault, url, nullptr);
    CFStringRef method;
    switch (request.method) {
        case app::HttpMethod::get:
            method = CFSTR("GET");
            break;
        case app::HttpMethod::post:
            method = CFSTR("POST");
            break;
        case app::HttpMethod::put:
            method = CFSTR("PUT");
            break;
        case app::HttpMethod::patch:
            method = CFSTR("PATCH");
            break;
        case app::HttpMethod::del:
            method = CFSTR("DELETE");
            break;
    }
    CFHTTPMessageRef myRequest =
            CFHTTPMessageCreateRequest(kCFAllocatorDefault, method, myURL,
                                        kCFHTTPVersion1_1);
    if (request.method != app::HttpMethod::get) {
        auto body_string = CFStringCreateWithCString(kCFAllocatorDefault, request.body.c_str(),
                                                        kCFStringEncodingUTF8);
        CFDataRef bodyData = CFStringCreateExternalRepresentation(kCFAllocatorDefault,
                                                                    body_string, kCFStringEncodingUTF8, 0);
        CFHTTPMessageSetBody(myRequest, bodyData);
        CFRelease(body_string);
        CFRelease(bodyData);
    }
    if (!request.headers.empty()) {
        for (auto &header: request.headers) {
            auto name = CFStringCreateWithCString(kCFAllocatorDefault, header.first.c_str(), kCFStringEncodingUTF8);
            auto value = CFStringCreateWithCString(kCFAllocatorDefault, header.second.c_str(),
                                                    kCFStringEncodingUTF8);
            CFHTTPMessageSetHeaderFieldValue(myRequest, name, value);
            CFRelease(name);
            CFRelease(value);
        }
    }
    CFReadStreamRef myReadStream = CFReadStreamCreateForHTTPRequest(kCFAllocatorDefault, myRequest);
    CFReadStreamOpen(myReadStream);
    auto error = CFReadStreamGetError(myReadStream);
    if (error.error) {
        abort();
    }
    std::string response_string;

    UInt8 buffer[2048];
    auto bytes_read = CFReadStreamRead(myReadStream, buffer, sizeof(buffer));
    if (bytes_read != -1) {
        response_string = std::string((char *) buffer, bytes_read);
        while (bytes_read != 0) {
            bytes_read = CFReadStreamRead(myReadStream, buffer, sizeof(buffer));
            if (bytes_read != 0) {
                response_string += std::string((char *) buffer, bytes_read);
            }
        }
    }
    auto myResponse = (CFHTTPMessageRef)CFReadStreamCopyProperty(myReadStream, kCFStreamPropertyHTTPResponseHeader);
    const UInt32 myErrCode = CFHTTPMessageGetResponseStatusCode(myResponse);
    CFReadStreamClose(myReadStream);
    CFRelease(myRequest);
    CFRelease(myURL);
    CFRelease(url);
    CFRelease(method);
    CFRelease(myReadStream);
    if (myResponse)
        CFRelease(myResponse);
    completion_block(request, {
        .http_status_code = static_cast<int>(myErrCode),
        .body = response_string
    });
}
