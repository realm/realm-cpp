#include <cpprealm/app.hpp>

#include <Foundation/NSData.h>
#include <Foundation/NSURL.h>
#include <Foundation/NSURLResponse.h>
#include <Foundation/NSURLSession.h>

namespace realm::internal {
    void DefaultTransport::send_request_to_server(app::Request &&request, app::HttpCompletion &&completion_block) {
        NSMutableURLRequest *urlRequest = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:[NSString stringWithCString:request.url.c_str()
                                                                                                         encoding:NSUTF8StringEncoding]]];

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

        if (!request.headers.empty()) {
            for (auto& header : request.headers) {
                [urlRequest addValue:[NSString stringWithCString:header.second.c_str() encoding:NSUTF8StringEncoding]
                  forHTTPHeaderField:[NSString stringWithCString:header.first.c_str() encoding:NSUTF8StringEncoding]];
            }
        }
        if (request.method != app::HttpMethod::get && !request.body.empty()) {
            [urlRequest setHTTPBody:[[NSString stringWithCString:request.body.c_str() encoding:NSUTF8StringEncoding]
                    dataUsingEncoding:NSUTF8StringEncoding]];
        }
        NSURLSession *session = [NSURLSession sharedSession];
        auto completion_ptr = completion_block.release();
        NSURLSessionDataTask *dataTask = [session dataTaskWithRequest:urlRequest
                                                    completionHandler:[request = std::move(request),
                                                                       completion_ptr](NSData *data, NSURLResponse *response, NSError *error) {
            util::UniqueFunction<void(const app::Request&, const app::Response&)> completion(completion_ptr);
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

            completion(request, app::Response {
                .http_status_code=status_code,
                .body=body
            });
        }];
        [dataTask resume];
    }
}