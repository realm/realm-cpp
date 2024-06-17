#include "../admin_utils.hpp"
#include "../main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("websocket_handler_called", "[sync]") {

    realm::App::configuration config;
    config.app_id = Admin::Session::shared().cached_app_id();
    config.base_url = Admin::Session::shared().base_url();

    realm::networking::websocket_event_handler handler;
    handler.on_connect = [](realm::networking::websocket_endpoint && ep) {
        ep.is_ssl = false;
        return ep;
    };
    config.websocket_event_handler = std::make_shared<realm::networking::websocket_event_handler>(handler);

    auto app = realm::App(config);
    auto user = app.login(realm::App::credentials::anonymous()).get();
    auto flx_sync_config = user.flexible_sync_configuration();
    auto synced_realm = db(flx_sync_config);

    auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                          subs.clear();
                                                      }).get();
    CHECK(update_success == true);
    CHECK(synced_realm.subscriptions().size() == 0);

}