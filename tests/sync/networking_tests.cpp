#include "../admin_utils.hpp"
#include "../main.hpp"
#include "test_objects.hpp"

#include "../utils/networking/proxy_server.hpp"

#include <iostream>

using namespace realm;

TEST_CASE("sends plaintext data to proxy", "[proxy]") {

    tests::utils::proxy_server::config cfg;
    cfg.port = 1234;
    cfg.server_uses_ssl = false; // Set to true if using services.cloud.mongodb.com
    tests::utils::proxy_server server(std::move(cfg));

    std::set<tests::utils::proxy_server::event> proxy_events;
    server.set_callback([&proxy_events](tests::utils::proxy_server::event e) {
        proxy_events.insert(e);
    });

    proxy_config pc;
    pc.port = 1234;
    pc.address = "127.0.0.1";
    realm::App::configuration config;
    config.proxy_configuration = pc;
    config.app_id = Admin::Session::shared().cached_app_id();
    config.base_url = Admin::Session::shared().base_url();

    struct foo_socket_provider : public ::realm::networking::default_socket_provider {
        std::unique_ptr<::realm::networking::websocket_interface> connect(std::unique_ptr<::realm::networking::websocket_observer> o,
                                                                          ::realm::networking::websocket_endpoint&& ep) override {
            const std::string from = "wss:";
            const std::string to = "ws:";
            if (ep.url.find(from) == 0) {
                ep.url.replace(0, from.length(), to);
            }
            m_called = true;
            std::cout << "foo_socket_provider: called" << "\n";
            return ::realm::networking::default_socket_provider::connect(std::move(o), std::move(ep));
        }

        bool was_called() const {
            return m_called;
        }

    private:
        bool m_called = false;
    };

    auto foo_socket = std::make_shared<foo_socket_provider>();
    config.sync_socket_provider = foo_socket;

    struct foo_http_transport : public ::realm::networking::default_http_transport {
        void send_request_to_server(const ::realm::networking::request& request,
                                    std::function<void(const ::realm::networking::response&)>&& completion) override {
            auto req_copy = request;
            const std::string from = "https:";
            const std::string to = "http:";
            if (req_copy.url.find(from) == 0) {
                req_copy.url.replace(0, from.length(), to);
            }
            m_called = true;
            std::cout << "foo_http_transport: called" << "\n";
            return ::realm::networking::default_http_transport::send_request_to_server(req_copy, std::move(completion));
        }

        bool was_called() const {
            return m_called;
        }

    private:
        bool m_called = false;
    };

    auto foo_transport = std::make_shared<foo_http_transport>();
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

    bool is_subset = std::includes(expected_events.begin(), expected_events.end(), proxy_events.begin(), proxy_events.end());
    CHECK(is_subset);
//    CHECK(foo_transport->was_called());
//    CHECK(foo_socket->was_called());
}

TEST_CASE("proxy roundtrip", "[proxy]") {

    tests::utils::proxy_server::config cfg;
    cfg.port = 1234;
    cfg.server_uses_ssl = false; // Set to true if using services.cloud.mongodb.com
    tests::utils::proxy_server server(std::move(cfg));

    std::set<tests::utils::proxy_server::event> proxy_events;
    server.set_callback([&proxy_events](tests::utils::proxy_server::event e) {
        proxy_events.insert(e);
    });

    proxy_config pc;
    pc.port = 1234;
    pc.address = "127.0.0.1";
    realm::App::configuration config;
    config.proxy_configuration = pc;
    config.app_id = Admin::Session::shared().cached_app_id();
    config.base_url = Admin::Session::shared().base_url();

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

    bool is_subset = std::includes(expected_events.begin(), expected_events.end(), proxy_events.begin(), proxy_events.end());
    CHECK(is_subset);
}