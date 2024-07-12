#include "../admin_utils.hpp"
#include "../main.hpp"
#include "test_objects.hpp"

#include "../utils/networking/proxy_server.hpp"
#include <cpprealm/internal/networking/utils.hpp>

#include <thread>

using namespace realm;

TEST_CASE("custom transport to proxy", "[proxy]") {

    tests::utils::proxy_server::config cfg;
    cfg.port = 1234;
    cfg.server_uses_ssl = false; // Set to true if using services.cloud.mongodb.com
    tests::utils::proxy_server server(std::move(cfg));

    std::set<tests::utils::proxy_server::event> proxy_events;
    server.set_callback([&proxy_events](tests::utils::proxy_server::event e) {
        proxy_events.insert(e);
    });

    realm::App::configuration config;
    config.app_id = Admin::Session::shared().create_app({"str_col", "_id"}, "test", false, false);
    config.base_url = Admin::Session::shared().base_url();
    config.enable_caching = true;

    auto transport_config = ::realm::networking::default_transport_configuration();
    proxy_config pc;
    pc.port = 1234;
    pc.address = "127.0.0.1";
    transport_config.proxy_config = pc;
    
    struct foo_socket_provider : public ::realm::networking::default_socket_provider {

        foo_socket_provider(const ::realm::networking::default_transport_configuration& configuration) {
            m_configuration = configuration;
        }
        std::unique_ptr<::realm::networking::websocket_interface> connect(std::unique_ptr<::realm::networking::websocket_observer> o,
                                                                          ::realm::networking::websocket_endpoint&& ep) override {
            m_called = true;
            return ::realm::networking::default_socket_provider::connect(std::move(o), std::move(ep));
        }

        bool was_called() const {
            return m_called;
        }

    private:
        bool m_called = false;
    };

    auto foo_socket = std::make_shared<foo_socket_provider>(transport_config);
    config.sync_socket_provider = foo_socket;

    struct foo_http_transport : public ::realm::networking::default_http_transport {

        foo_http_transport(const ::realm::networking::default_transport_configuration& configuration) {
            m_configuration = configuration;
        }

        void send_request_to_server(const ::realm::networking::request& request,
                                    std::function<void(const ::realm::networking::response&)>&& completion) override {
            auto req_copy = request;
            const std::string from = "https:";
            const std::string to = "http:";
            if (req_copy.url.find(from) == 0) {
                req_copy.url.replace(0, from.length(), to);
            }
            m_called = true;
            return ::realm::networking::default_http_transport::send_request_to_server(req_copy, std::move(completion));
        }

        bool was_called() const {
            return m_called;
        }

    private:
        bool m_called = false;
    };

    auto foo_transport = std::make_shared<foo_http_transport>(transport_config);
    config.http_transport_client = foo_transport;

    auto app = realm::App(config);

    auto user = app.login(realm::App::credentials::anonymous()).get();
    auto flx_sync_config = user.flexible_sync_configuration();
    auto synced_realm = db(flx_sync_config);

    auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                          subs.clear();
                                                      }).get();
    CHECK(update_success == true);
    CHECK(synced_realm.subscriptions().size() == 0);

    update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                     subs.add<AllTypesObject>("foo-strings", [](auto &obj) {
                                                         return obj.str_col == "foo";
                                                     });
                                                     subs.add<AllTypesObjectLink>("foo-link");
                                                 }).get();
    CHECK(update_success == true);
    CHECK(synced_realm.subscriptions().size() == 2);

    auto sub = *synced_realm.subscriptions().find("foo-strings");
    CHECK(sub.name == "foo-strings");
    CHECK(sub.object_class_name == "AllTypesObject");
    CHECK(sub.query_string == "str_col == \"foo\"");

    std::set<tests::utils::proxy_server::event> expected_events;
    expected_events.insert(tests::utils::proxy_server::event::connect);
    expected_events.insert(tests::utils::proxy_server::event::client);
    expected_events.insert(tests::utils::proxy_server::event::nonssl);
    expected_events.insert(tests::utils::proxy_server::event::websocket_upgrade);
    expected_events.insert(tests::utils::proxy_server::event::websocket);

    CHECK(proxy_events == expected_events);
    CHECK(foo_transport->was_called());
    CHECK(foo_socket->was_called());
}

TEST_CASE("built in transport to proxy roundtrip", "[proxy]") {

    tests::utils::proxy_server::config cfg;
    cfg.port = 1235;
    cfg.server_uses_ssl = false; // Set to true if using services.cloud.mongodb.com
    tests::utils::proxy_server server(std::move(cfg));

    std::set<tests::utils::proxy_server::event> proxy_events;
    server.set_callback([&proxy_events](tests::utils::proxy_server::event e) {
        proxy_events.insert(e);
    });

    realm::App::configuration config;
    config.app_id = Admin::Session::shared().create_app({"str_col", "_id"}, "test", false, false);
    config.base_url = Admin::Session::shared().base_url();
    config.enable_caching = false;

    auto transport_config = ::realm::networking::default_transport_configuration();
    proxy_config pc;
    pc.port = 1235;
    pc.address = "127.0.0.1";
    transport_config.proxy_config = pc;

    config.sync_socket_provider = std::make_shared<realm::networking::default_socket_provider>(transport_config);
    config.http_transport_client = std::make_shared<realm::networking::default_http_transport>(transport_config);

    auto app = realm::App(config);

    auto user = app.login(realm::App::credentials::anonymous()).get();
    auto flx_sync_config = user.flexible_sync_configuration();
    auto synced_realm = db(flx_sync_config);

    auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
        subs.clear();
    }).get();
    CHECK(update_success == true);
    CHECK(synced_realm.subscriptions().size() == 0);

    update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
        subs.add<AllTypesObject>("foo-strings", [](auto &obj) {
            return obj.str_col == "foo";
        });
        subs.add<AllTypesObjectLink>("foo-link");
    }).get();
    CHECK(update_success == true);
    CHECK(synced_realm.subscriptions().size() == 2);

    auto sub = *synced_realm.subscriptions().find("foo-strings");
    CHECK(sub.name == "foo-strings");
    CHECK(sub.object_class_name == "AllTypesObject");
    CHECK(sub.query_string == "str_col == \"foo\"");

    std::set<tests::utils::proxy_server::event> expected_events;
    expected_events.insert(tests::utils::proxy_server::event::connect);
    expected_events.insert(tests::utils::proxy_server::event::client);
    expected_events.insert(tests::utils::proxy_server::event::nonssl);
    expected_events.insert(tests::utils::proxy_server::event::websocket_upgrade);
    expected_events.insert(tests::utils::proxy_server::event::websocket);

    CHECK(proxy_events == expected_events);
}

TEST_CASE("WebsocketEndpoint", "[proxy]") {
    realm::networking::websocket_endpoint ep;
    ep.url = "wss://my-server.com:443";
    ep.protocols = std::vector<std::string>({"test_protocol"});

    ::realm::networking::default_transport_configuration config;
    config.proxy_config = ::realm::proxy_config();
    config.proxy_config->address = "127.0.0.1";
    config.proxy_config->port = 1234;
    config.proxy_config->username_password = std::pair<std::string, std::string>("foo", "bar");

    config.ssl_trust_certificate_path = "test_path";

    auto res = ::realm::internal::networking::to_core_websocket_endpoint(ep, config);
    CHECK(res.is_ssl);
    CHECK(res.protocols.size() == 1);
    CHECK(res.protocols[0] == "test_protocol");
    CHECK(res.address == "my-server.com");

    CHECK(res.proxy);
    CHECK(res.proxy->port == 1234);
    CHECK(res.proxy->address == "127.0.0.1");
    CHECK(res.headers.find("Proxy-Authorization") != res.headers.end());

    CHECK(res.ssl_trust_certificate_path == "test_path");

    ep.url = "ws://my-server.com:80";
    res = ::realm::internal::networking::to_core_websocket_endpoint(ep, std::nullopt);
    CHECK_FALSE(res.is_ssl);
}