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
#include <cpprealm/internal/generic_network_transport.hpp>

#include <Foundation/NSData.h>
#include <Foundation/NSURL.h>
#include <Foundation/NSURLResponse.h>
#include <Foundation/NSURLSession.h>

namespace realm::internal {
void DefaultTransport::send_request_to_server(const app::Request& request,
                                              util::UniqueFunction<void(const app::Response&)>&& completion_block) {
        NSURL* url = [NSURL URLWithString:[NSString stringWithCString:request.url.c_str()
                                                             encoding:NSUTF8StringEncoding]];
        NSMutableURLRequest *urlRequest = [[NSMutableURLRequest alloc] initWithURL:url];

        switch (request.method) {
            case app::HttpMethod::get:
                [urlRequest setHTTPMethod:@"GET"];
                break;
            case app::HttpMethod::post:
                [urlRequest setHTTPMethod:@"POST"];
                break;
            case app::HttpMethod::put:
                [urlRequest setHTTPMethod:@"PUT"];
                break;
            case app::HttpMethod::patch:
                [urlRequest setHTTPMethod:@"PATCH"];
                break;
            case app::HttpMethod::del:
                [urlRequest setHTTPMethod:@"DELETE"];
                break;
        }

        for (auto& header : request.headers) {
            [urlRequest addValue:[NSString stringWithCString:header.second.c_str() encoding:NSUTF8StringEncoding]
              forHTTPHeaderField:[NSString stringWithCString:header.first.c_str() encoding:NSUTF8StringEncoding]];
        }
        if (m_custom_http_headers) {
            for (auto& header : *m_custom_http_headers) {
                [urlRequest addValue:[NSString stringWithCString:header.second.c_str() encoding:NSUTF8StringEncoding]
                        forHTTPHeaderField:[NSString stringWithCString:header.first.c_str() encoding:NSUTF8StringEncoding]];
            }
        }
        if (request.method != app::HttpMethod::get && !request.body.empty()) {
            [urlRequest setHTTPBody:[[NSString stringWithCString:request.body.c_str() encoding:NSUTF8StringEncoding]
                    dataUsingEncoding:NSUTF8StringEncoding]];
        }
        NSURLSession *session = [NSURLSession sharedSession];
        NSURLSessionDataTask *dataTask = [session dataTaskWithRequest:urlRequest
                                                    completionHandler:[request = std::move(request),
                                                                       completion_ptr = completion_block.release()](NSData *data, NSURLResponse *response, NSError *error) {
            util::UniqueFunction<void(const app::Response&)>&& completion(completion_ptr);
            auto httpResponse = (NSHTTPURLResponse *)response;
            std::string body;
            if (data) {
                body = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] cStringUsingEncoding:NSUTF8StringEncoding];
            }
            int status_code;
            if (error) {
                status_code = 500;
            } else {
                status_code = static_cast<int>(httpResponse.statusCode);
            }

            completion(app::Response {
                .http_status_code=status_code,
                .body=body
            });
        }];
        [dataTask resume];
    }
}
