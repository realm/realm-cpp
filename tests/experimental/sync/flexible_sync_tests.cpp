#include "../../admin_utils.hpp"
#include "../../main.hpp"
#include "../../sync_test_utils.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("flexible_sync_beta", "[sync]") {
    auto app = realm::App(Admin::shared().cached_app_id(), Admin::shared().base_url());
    SECTION("all") {
        app.get_sync_manager().set_log_level(logger::level::all);
        auto user = app.login(realm::App::credentials::anonymous()).get();
        auto flx_sync_config = user.flexible_sync_configuration();
        auto synced_realm = experimental::db(flx_sync_config);

        auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                              subs.clear();
                                                          }).get();
        CHECK(update_success == true);
        CHECK(synced_realm.subscriptions().size() == 0);

        update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                         subs.add<experimental::AllTypesObject>("foo-strings", [](auto &obj) {
                                                             return obj.str_col == "foo";
                                                         });
                                                         subs.add<experimental::AllTypesObjectLink>("foo-link");
                                                     }).get();
        CHECK(update_success == true);
        CHECK(synced_realm.subscriptions().size() == 2);

        auto sub = *synced_realm.subscriptions().find("foo-strings");
        CHECK(sub.name == "foo-strings");
        CHECK(sub.object_class_name == "AllTypesObject");
        CHECK(sub.query_string == "str_col == \"foo\"");

        auto non_existent_sub = synced_realm.subscriptions().find("non-existent");
        CHECK((non_existent_sub == std::nullopt) == true);

        synced_realm.write([&synced_realm]() {
            experimental::AllTypesObject o;
            o._id = 1;
            o.str_col = "foo";
            synced_realm.add(std::move(o));
        });

        test::wait_for_sync_uploads(user).get();
        test::wait_for_sync_downloads(user).get();
        synced_realm.refresh();
        auto objs = synced_realm.objects<experimental::AllTypesObject>();

        CHECK(objs[0]._id == (int64_t) 1);

        synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                        subs.update_subscription<experimental::AllTypesObject>("foo-strings", [](auto &obj) {
                                            return obj.str_col == "bar" && obj._id == (int64_t)1230;
                                        });
                                    }).get();

        auto sub2 = *synced_realm.subscriptions().find("foo-strings");
        CHECK(sub2.name == "foo-strings");
        CHECK(sub2.object_class_name == "AllTypesObject");
        CHECK(sub2.query_string == "str_col == \"bar\" and _id == 1230");

        synced_realm.write([&synced_realm]() {
            experimental::AllTypesObject o;
            o._id = 1230;
            o.str_col = "bar";
            synced_realm.add(std::move(o));
        });

        test::wait_for_sync_uploads(user).get();
        test::wait_for_sync_downloads(user).get();

        synced_realm.refresh();
        objs = synced_realm.objects<experimental::AllTypesObject>();
        CHECK(objs.size() == 1);
    }

    SECTION("encrypted sync realm") {
        auto encrypted_app = realm::App(Admin::shared().cached_app_id(), Admin::shared().base_url());
        auto user = encrypted_app.login(realm::App::credentials::anonymous()).get();
        auto flx_sync_config = user.flexible_sync_configuration();
        flx_sync_config.set_encryption_key({0,0,0,0,0,0,0,0, 1,1,0,0,0,0,0,0, 2,2,0,0,0,0,0,0, 3,3,0,0,0,0,0,0, 4,4,0,0,0,0,0,0, 5,5,0,0,0,0,0,0, 6,6,0,0,0,0,0,0, 7,7,0,0,0,0,0,0});
        auto synced_realm = experimental::db(flx_sync_config);

        auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                              subs.clear();
                                                          }).get();
        CHECK(update_success == true);
        CHECK(synced_realm.subscriptions().size() == 0);
        // Missing encryption key
        auto flx_sync_config2 = user.flexible_sync_configuration();
        REQUIRE_THROWS(experimental::db(flx_sync_config2));
    }
}