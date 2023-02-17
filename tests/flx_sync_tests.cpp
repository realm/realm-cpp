#include "admin_utils.hpp"
#include "main.hpp"
#include "test_objects.hpp"
#include "sync_test_utils.hpp"

using namespace realm;

TEST_CASE("flx_sync", "[flx][sync]") {
    SECTION("all") {
        auto app = realm::App(Admin::shared().create_app({"str_col", "_id"}), Admin::shared().base_url());
        auto user = app.login(realm::App::credentials::anonymous()).get_future().get();
        auto flx_sync_config = user.flexible_sync_configuration();
        auto p = realm::async_open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(flx_sync_config);
        auto tsr = p.get_future().get();
        auto synced_realm = tsr.resolve();

        auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
            subs.clear();
        }).get_future().get();
        CHECK(update_success == true);
        CHECK(synced_realm.subscriptions().size() == 0);

        update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
            subs.add<AllTypesObject>("foo-strings", [](auto &obj) {
                return obj.str_col == "foo";
            });
        }).get_future().get();
        CHECK(update_success == true);
        CHECK(synced_realm.subscriptions().size() == 1);

        auto sub = *synced_realm.subscriptions().find("foo-strings");
        CHECK(sub.name == "foo-strings");
        CHECK(sub.object_class_name == "AllTypesObject");
        CHECK(sub.query_string == "str_col == \"foo\"");

        auto non_existent_sub = synced_realm.subscriptions().find("non-existent");
        CHECK((non_existent_sub == std::nullopt) == true);

        synced_realm.write([&synced_realm]() {
            synced_realm.add(AllTypesObject{._id=1, .str_col="foo"});
        });

        test::wait_for_sync_uploads(user).get_future().get();
        test::wait_for_sync_downloads(user).get_future().get();
        synced_realm.write([]() {}); // refresh realm
        auto objs = synced_realm.objects<AllTypesObject>();

        CHECK(objs[0]._id == 1);

        synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
            subs.update_subscription<AllTypesObject>("foo-strings", [](auto &obj) {
                return obj.str_col == "bar" && obj._id == 123;
            });
        }).get_future().get();

        auto sub2 = *synced_realm.subscriptions().find("foo-strings");
        CHECK(sub2.name == "foo-strings");
        CHECK(sub2.object_class_name == "AllTypesObject");
        CHECK(sub2.query_string == "str_col == \"bar\" and _id == 123");

        synced_realm.write([&synced_realm]() {
            synced_realm.add(AllTypesObject{._id=123, .str_col="bar"});
        });

        test::wait_for_sync_uploads(user).get_future().get();
        test::wait_for_sync_downloads(user).get_future().get();

        synced_realm.refresh();
        objs = synced_realm.objects<AllTypesObject>();
        CHECK(objs.size() == 1);

        synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
            subs.update_subscription<AllTypesObject>("foo-strings");
        }).get_future().get();

        auto sub3 = *synced_realm.subscriptions().find("foo-strings");
        CHECK(sub3.name == "foo-strings");
        CHECK(sub3.object_class_name == "AllTypesObject");
        CHECK(sub3.query_string == "TRUEPREDICATE");

        test::wait_for_sync_uploads(user).get_future().get();
        test::wait_for_sync_downloads(user).get_future().get();

        synced_realm.refresh();
        objs = synced_realm.objects<AllTypesObject>();
        CHECK(objs.size() == 2);
    }
}

TEST_CASE("tsr") {
    realm_path path;
    auto realm = realm::open<Person, Dog>({path});

    auto person = Person { .name = "John", .age = 17 };
    person.dog = Dog {.name = "Fido"};

    realm.write([&realm, &person] {
        realm.add(person);
    });

    auto tsr = realm::thread_safe_reference<Person>(person);
    std::promise<void> p;
    auto t = std::thread([&tsr, &p, &path]() {
        auto realm = realm::open<Person, Dog>({path});
        auto person = realm.resolve(std::move(tsr));
        CHECK(*person.age == 17);
        realm.write([&] { realm.remove(person); });
        p.set_value();
    });
    t.join();
    p.get_future().get();
}

TEST_CASE("realm_is_populated_on_async_open") {
    auto app = realm::App(Admin::shared().create_app({"str_col", "_id"}), Admin::shared().base_url());
    {
        auto user = app.login(realm::App::credentials::anonymous()).get_future().get();
        auto flx_sync_config = user.flexible_sync_configuration();
        auto synced_realm = realm::async_open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(flx_sync_config).get_future().get().resolve();

        auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
            subs.clear();
        }).get_future().get();
        CHECK(update_success == true);
        CHECK(synced_realm.subscriptions().size() == 0);

        update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
            subs.add<AllTypesObject>("foo-strings", [](auto &obj) {
                return obj.str_col == "foo";
            });
        }).get_future().get();
        CHECK(update_success == true);

        synced_realm.write([&synced_realm]() {
            for (size_t i = 0; i < 1000; i++) {
                synced_realm.add(AllTypesObject{._id=i, .str_col="foo"});
            }
        });

        auto sync_session = synced_realm.get_sync_session();
        auto upload_completion = sync_session->wait_for_upload_completion();
        upload_completion.get_future().get();
    }

    {
        app.register_user("foo@mongodb.com", "foobar").get_future().get();
        auto user = app.login(realm::App::credentials::username_password("foo@mongodb.com", "foobar")).get_future().get();
        auto synced_realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(user.flexible_sync_configuration());
        synced_realm.refresh();
        auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                         subs.add<AllTypesObject>("foo-strings", [](auto &obj) {
                                                             return obj.str_col == "foo";
                                                         });
                                                     }).get_future().get();
        CHECK(update_success == true);
        auto objs = synced_realm.objects<AllTypesObject>();
        CHECK(objs.size() < 1000);

        auto sync_session = synced_realm.get_sync_session();
        sync_session->wait_for_download_completion().get_future().get();
        synced_realm.refresh();
        CHECK(objs.size() == 1000);
    }
}