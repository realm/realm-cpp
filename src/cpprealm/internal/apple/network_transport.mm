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

#include <Foundation/NSData.h>
#include <Foundation/NSURL.h>
#include <Foundation/NSURLCache.h>
#include <Foundation/NSURLResponse.h>
#include <Foundation/NSURLSession.h>

#include <realm/object-store/sync/generic_network_transport.hpp>
#include <realm/util/base64.hpp>

namespace realm::networking {

    void default_http_transport::send_request_to_server(const request& request,
                                                        std::function<void(const response&)>&& completion_block) {
        NSURL* url = [NSURL URLWithString:[NSString stringWithCString:request.url.c_str()
                                                             encoding:NSUTF8StringEncoding]];
        NSMutableURLRequest *urlRequest = [[NSMutableURLRequest alloc] initWithURL:url];

        switch (request.method) {
            case http_method::get:
                [urlRequest setHTTPMethod:@"GET"];
                break;
            case http_method::post:
                [urlRequest setHTTPMethod:@"POST"];
                break;
            case http_method::put:
                [urlRequest setHTTPMethod:@"PUT"];
                break;
            case http_method::patch:
                [urlRequest setHTTPMethod:@"PATCH"];
                break;
            case http_method::del:
                [urlRequest setHTTPMethod:@"DELETE"];
                break;
        }

        for (auto& header : request.headers) {
            [urlRequest addValue:[NSString stringWithCString:header.second.c_str() encoding:NSUTF8StringEncoding]
              forHTTPHeaderField:[NSString stringWithCString:header.first.c_str() encoding:NSUTF8StringEncoding]];
        }
        if (m_configuration.custom_http_headers) {
            for (auto& header : *m_configuration.custom_http_headers) {
                [urlRequest addValue:[NSString stringWithCString:header.second.c_str() encoding:NSUTF8StringEncoding]
                        forHTTPHeaderField:[NSString stringWithCString:header.first.c_str() encoding:NSUTF8StringEncoding]];
            }
        }
        if (request.method != http_method::get && !request.body.empty()) {
            [urlRequest setHTTPBody:[[NSString stringWithCString:request.body.c_str() encoding:NSUTF8StringEncoding]
                    dataUsingEncoding:NSUTF8StringEncoding]];
        }

        NSURLSession *session;
        if (m_configuration.proxy_config) {
            NSURLSessionConfiguration *sessionConfiguration = [NSURLSessionConfiguration defaultSessionConfiguration];
            NSString *proxyHost = @(m_configuration.proxy_config->address.c_str());
            NSInteger proxyPort = m_configuration.proxy_config->port;
            sessionConfiguration.connectionProxyDictionary = @{
                @"HTTPSEnable": @YES,
                @"HTTPSProxy": @(proxyPort),
                @"HTTPSPort": proxyHost,
            };
            sessionConfiguration.requestCachePolicy = NSURLRequestCachePolicy::NSURLRequestReloadIgnoringLocalCacheData;
            urlRequest.cachePolicy = NSURLRequestCachePolicy::NSURLRequestReloadIgnoringLocalCacheData;
            [[NSURLCache sharedURLCache] removeAllCachedResponses];

            session = [NSURLSession sessionWithConfiguration:sessionConfiguration];
            if (m_configuration.proxy_config->username_password) {
                auto userpass = util::format("%1:%2", m_configuration.proxy_config->username_password->first,
                                             m_configuration.proxy_config->username_password->second);
                std::string encoded_userpass;
                encoded_userpass.resize(realm::util::base64_encoded_size(userpass.length()));
                realm::util::base64_encode(userpass, encoded_userpass);
                [urlRequest addValue:@(util::format("Basic %1", encoded_userpass).c_str()) forHTTPHeaderField:@"'Proxy-Authorization'"];
            }

        } else {
            session = [NSURLSession sharedSession];
        }

        NSURLSessionDataTask *dataTask = [session dataTaskWithRequest:urlRequest
                                                    completionHandler:[request = std::move(request),
                                                                       completion = completion_block](NSData *data, NSURLResponse *response, NSError *error) {
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

            completion({
                .http_status_code=status_code,
                .body=body
            });
        }];
        [dataTask resume];
    }
}
