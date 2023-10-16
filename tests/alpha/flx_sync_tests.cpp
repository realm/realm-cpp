#include "../admin_utils.hpp"
#include "../main.hpp"
#include "test_objects.hpp"
#include "../sync_test_utils.hpp"

using namespace realm;

TEST_CASE("flx_sync", "[sync]") {
    auto app = realm::App(realm::App::configuration({Admin::shared().cached_app_id(), Admin::shared().base_url()}));
    SECTION("all") {
        app.get_sync_manager().set_log_level(logger::level::off);
        auto user = app.login(realm::App::credentials::anonymous()).get();
        auto flx_sync_config = user.flexible_sync_configuration();
        auto synced_realm = realm::async_open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(flx_sync_config).get_future().get().resolve();

        auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                              subs.clear();
                                                          }).get();
        CHECK(update_success == true);
        CHECK(synced_realm.subscriptions().size() == 0);

        update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                         subs.add<AllTypesObject>("foo-strings", [](auto &obj) {
                                                             return obj.str_col == "foo";
                                                         });
                                                     }).get();
        CHECK(update_success == true);

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

        test::wait_for_sync_uploads(user).get();
        test::wait_for_sync_downloads(user).get();
        synced_realm.write([]() {}); // refresh realm
        auto objs = synced_realm.objects<AllTypesObject>();

        CHECK(objs[0]._id == 1);

        synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
            subs.update_subscription<AllTypesObject>("foo-strings", [](auto &obj) {
                return obj.str_col == "bar" && obj._id == 1230;
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

        test::wait_for_sync_uploads(user).get();
        test::wait_for_sync_downloads(user).get();

        synced_realm.refresh();
        objs = synced_realm.objects<AllTypesObject>();
        CHECK(objs.size() == 1);

        synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
            subs.update_subscription<AllTypesObject>("foo-strings");
        }).get();

        auto sub3 = *synced_realm.subscriptions().find("foo-strings");
        CHECK(sub3.name == "foo-strings");
        CHECK(sub3.object_class_name == "AllTypesObject");
        CHECK(sub3.query_string == "TRUEPREDICATE");

        test::wait_for_sync_uploads(user).get();
        test::wait_for_sync_downloads(user).get();

        synced_realm.refresh();
        objs = synced_realm.objects<AllTypesObject>();
        CHECK(objs.size() == 2);
    }
}

TEST_CASE("realm_is_populated_on_async_open", "[sync]") {
    auto app = realm::App(realm::App::configuration({Admin::shared().cached_app_id(), Admin::shared().base_url()}));
    SECTION("all") {
        {
            auto user = app.login(realm::App::credentials::anonymous()).get();
            auto flx_sync_config = user.flexible_sync_configuration();
            auto synced_realm = realm::async_open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(flx_sync_config).get_future().get().resolve();

            auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                                  subs.clear();
                                                              }).get();
            CHECK(update_success == true);
            CHECK(synced_realm.subscriptions().size() == 0);

            update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                             subs.add<AllTypesObject>("foo-strings", [](auto &obj) {
                                                                 return obj.str_col == "foo";
                                                             });
                                                         }).get();
            CHECK(update_success == true);

            synced_realm.write([&synced_realm]() {
                for (size_t i = 0; i < 1000; i++) {
                    AllTypesObject o;
                    o._id = i;
                    o.str_col = "foo";
                    synced_realm.add(std::move(o));
                }
            });

            auto sync_session = synced_realm.get_sync_session();
            sync_session->wait_for_upload_completion().get();

            auto objs = synced_realm.objects<AllTypesObject>();
            CHECK(objs.size() == 1000);
        }

        {
            app.register_user("foo@mongodb.com", "foobar").get();
            auto user = app.login(realm::App::credentials::username_password("foo@mongodb.com", "foobar")).get();
            auto synced_realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(user.flexible_sync_configuration());
            synced_realm.refresh();
            auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                                  subs.add<AllTypesObject>("foo-strings", [](auto &obj) {
                                                                      return obj.str_col == "foo";
                                                                  });
                                                              }).get();
            CHECK(update_success == true);
            auto objs = synced_realm.objects<AllTypesObject>();
            CHECK(objs.size() < 1000);

            auto sync_session = synced_realm.get_sync_session();
            sync_session->wait_for_download_completion().get();
            synced_realm.refresh();
            CHECK(objs.size() == 1000);
        }
    }
}
