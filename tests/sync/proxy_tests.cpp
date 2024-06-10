#include "../admin_utils.hpp"
#include "../main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("proxy_enabled_websocket", "[sync]") {

    realm::App::configuration config;
    config.app_id = Admin::Session::shared().cached_app_id();
    config.base_url = Admin::Session::shared().base_url();

    realm::networking::websocket_event_handler handler;
    handler.on_connect = [](realm::networking::ws_endpoint&& ep) {
        ep.is_ssl = false;
        return ep;
    };
    config.websocket_event_handler = std::make_shared<realm::networking::websocket_event_handler>(handler);

    auto app = realm::App(config);
    SECTION("all") {
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
                                                     })
                                 .get();
        CHECK(update_success == true);
        CHECK(synced_realm.subscriptions().size() == 2);

        auto sub = *synced_realm.subscriptions().find("foo-strings");
        CHECK(sub.name == "foo-strings");
        CHECK(sub.object_class_name == "AllTypesObject");
        CHECK(sub.query_string == "str_col == \"foo\"");

        auto non_existent_sub = synced_realm.subscriptions().find("non-existent");
        CHECK((non_existent_sub == std::nullopt) == true);

        synced_realm.write([&synced_realm]() {
            AllTypesObject o;
            o._id = 1;
            o.str_col = "foo";
            synced_realm.add(std::move(o));
        });

        synced_realm.get_sync_session()->wait_for_upload_completion().get();
        synced_realm.get_sync_session()->wait_for_download_completion().get();

        synced_realm.refresh();
        auto objs = synced_realm.objects<AllTypesObject>();

        CHECK(objs[0]._id == (int64_t) 1);

        synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                        subs.update_subscription<AllTypesObject>("foo-strings", [](auto &obj) {
                                            return obj.str_col == "bar" && obj._id == (int64_t) 1230;
                                        });
                                    }).get();

        auto sub2 = *synced_realm.subscriptions().find("foo-strings");
        CHECK(sub2.name == "foo-strings");
        CHECK(sub2.object_class_name == "AllTypesObject");
        CHECK(sub2.query_string == "str_col == \"bar\" and _id == 1230");

        synced_realm.write([&synced_realm]() {
            AllTypesObject o;
            o._id = 1230;
            o.str_col = "bar";
            synced_realm.add(std::move(o));
        });

        synced_realm.get_sync_session()->wait_for_upload_completion().get();
        synced_realm.get_sync_session()->wait_for_download_completion().get();

        synced_realm.refresh();
        objs = synced_realm.objects<AllTypesObject>();
        CHECK(objs.size() == 1);
    }
}