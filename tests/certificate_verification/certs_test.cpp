#include "certs_test.hpp"

#include <iostream>
#include <cpprealm/sdk.hpp>
#include <cpprealm/internal/generic_network_transport.hpp>

int main(int argc, char *argv[]) {

    static realm::internal::DefaultTransport transport;

    realm::app::Request r;
    r.url = "https://realm.mongodb.com";

    std::promise<realm::app::Response> p;
    transport.send_request_to_server(std::move(r),
                                     [&p](auto&& response){
                                         p.set_value(std::move(response));
                                     });
    auto res = p.get_future().get();

    std::cout << res.http_status_code << "\n";
    std::cout << res.body << std::endl;

    return 0;
}