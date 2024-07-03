#include "../admin_utils.hpp"
#include "../main.hpp"
#include "test_objects.hpp"
#include "../utils/networking/proxy_server.hpp"
using namespace realm;

TEST_CASE("websocket_handler_called", "[sync]") {

//    tests::utils::proxy_server::config cfg;
//    cfg.port = 1234;
//    cfg.client_uses_ssl = false;
//    cfg.server_uses_ssl = false;
//    tests::utils::proxy_server server(std::move(cfg));

    proxy_config pc;
    pc.port = 1234;
    pc.address = "127.0.0.1";
    realm::App::configuration config;
//    config.proxy_configuration = pc;
    config.app_id = Admin::Session::shared().cached_app_id();
    config.base_url = Admin::Session::shared().base_url();


    struct foo_socket_provider : public ::realm::networking::default_socket_provider {
        std::unique_ptr<::realm::networking::websocket_interface> connect(std::unique_ptr<::realm::networking::websocket_observer> o,
                                                                          ::realm::networking::websocket_endpoint&& ep) override {
//            url = url.replace("wss://", "ws://");

            return ::realm::networking::default_socket_provider::connect(std::move(o), std::move(ep));
        }
    };

    config.sync_socket_provider = std::make_shared<foo_socket_provider>();

    auto app = realm::App(config);
    app.get_sync_manager().set_log_level(logger::level::all);

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

}