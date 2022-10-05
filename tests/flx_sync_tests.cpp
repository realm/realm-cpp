#include "admin_utils.hpp"
#include "main.hpp"
#include "test_objects.hpp"
#include "sync_test_utils.hpp"

using namespace realm;

TEST_CASE("flx_sync", "[flx]") {
    SECTION("all") {
        auto app = realm::App(Admin::shared().create_app<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(
                {"str_col", "_id"}), "http://localhost:9090");
        auto user = app.login(realm::App::Credentials::anonymous()).get_future().get();
        auto flx_sync_config = user.flexible_sync_configuration();
        auto tsr = realm::async_open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(
                flx_sync_config).get_future().get();
        auto synced_realm = tsr.resolve();

        auto update_success = synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet &subs) {
            subs.clear();
        }).get_future().get();
        CHECK(update_success == true);
        CHECK(synced_realm.subscriptions().size() == 0);

        update_success = synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet &subs) {
            subs.add<AllTypesObject>("foo-strings", [](auto &obj) {
                return obj.str_col == "foo";
            });
        }).get_future().get();
        CHECK(update_success == true);
        CHECK(synced_realm.subscriptions().size() == 1);

        auto sub = *synced_realm.subscriptions().find("foo-strings");
        CHECK(sub.name() == "foo-strings");
        CHECK(sub.object_class_name() == "AllTypesObject");
        CHECK(sub.query_string() == "str_col == \"foo\"");

        auto non_existent_sub = synced_realm.subscriptions().find("non-existent");
        CHECK((non_existent_sub == std::nullopt) == true);

        synced_realm.write([&synced_realm]() {
            synced_realm.add(AllTypesObject{._id=1, .str_col="foo"});
        });

        test::wait_for_sync_uploads(user).get_future().get();
        test::wait_for_sync_downloads(user).get_future().get();
        synced_realm.write([]() {}); // refresh realm
        auto objs = synced_realm.objects<AllTypesObject>();

        CHECK(objs[0]->_id == 1);

        synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet &subs) {
            subs.update_subscription<AllTypesObject>("foo-strings", [](auto &obj) {
                return obj.str_col == "bar" && obj._id == 123;
            });
        }).get_future().get();

        sub = *synced_realm.subscriptions().find("foo-strings");
        CHECK(sub.name() == "foo-strings");
        CHECK(sub.object_class_name() == "AllTypesObject");
        CHECK(sub.query_string() == "str_col == \"bar\" and _id == 123");

        synced_realm.write([&synced_realm]() {
            synced_realm.add(AllTypesObject{._id=123, .str_col="bar"});
        });

        test::wait_for_sync_uploads(user).get_future().get();
        test::wait_for_sync_downloads(user).get_future().get();

        synced_realm.write([]() {}); // refresh realm
        objs = synced_realm.objects<AllTypesObject>();
        CHECK(objs.size() == 1);

        synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet &subs) {
            subs.update_subscription<AllTypesObject>("foo-strings");
        }).get_future().get();

        sub = *synced_realm.subscriptions().find("foo-strings");
        CHECK(sub.name() == "foo-strings");
        CHECK(sub.object_class_name() == "AllTypesObject");
        CHECK(sub.query_string() == "TRUEPREDICATE");

        test::wait_for_sync_uploads(user).get_future().get();
        test::wait_for_sync_downloads(user).get_future().get();

        synced_realm.write([]() {}); // refresh realm
        objs = synced_realm.objects<AllTypesObject>();
        CHECK(objs.size() == 2);
        std::cout<<"TEST COMPLETE"<<std::endl;
    }
}

TEST_CASE("tsr") {
    realm_path path;
    auto realm = realm::open<Person, Dog>({.path=path});

    auto person = Person { .name = "John", .age = 17 };
    person.dog = Dog {.name = "Fido"};

    realm.write([&realm, &person] {
        realm.add(person);
    });

    auto tsr = realm::thread_safe_reference<Person>(person);
    std::condition_variable cv;
    std::mutex cv_m;
    bool done;
    auto t = std::thread([&cv, &tsr, &done, &path]() {
        auto realm = realm::open<Person, Dog>({.path=path});
        auto person = realm.resolve(std::move(tsr));
        CHECK(*person.age == 17);
        realm.write([&] { realm.remove(person); });
    });
    t.join();
}