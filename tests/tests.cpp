#include "test_utils.hpp"
#include "test_objects.hpp"
#include "sync_test_utils.hpp"
#include "admin_utils.hpp"

#include <realm/object-store/impl/realm_coordinator.hpp>


TEST(all) {
    auto realm = realm::open<Person, Dog>({.path=path});

    auto person = Person();
    person.name = "John";
    person.age = 17;
    person.dog = Dog{.name = "Fido"};

    realm.write([&realm, &person] {
        realm.add(person);
    });

    CHECK_EQUALS(*person.name, "John");
    CHECK_EQUALS(*person.age, 17);
    auto dog = **person.dog;
    CHECK_EQUALS(*dog.name, "Fido");

    bool did_run = false;
    auto token = person.observe<Person>([&did_run](auto&& change) {
        CHECK_EQUALS(change.property.name, "age")
        CHECK_EQUALS(std::any_cast<int>(*change.property.new_value), 19);
        did_run = true;
    });

    realm.write([&person] {
        person.age += 2;
    });

    CHECK_EQUALS(*person.age, 19);

    auto persons = realm.objects<Person>();
    CHECK_EQUALS(persons.size(), 1)

    std::vector<Person> people;
    std::copy(persons.begin(), persons.end(), std::back_inserter(people));
    for (auto& person:people) {
        realm.write([&person, &realm]{
            realm.remove(person);
        });
    }
    CHECK_EQUALS(did_run, true)

    CHECK_EQUALS(persons.size(), 0);
    auto app = realm::App("car-wsney");
    auto user = co_await app.login(realm::App::Credentials::anonymous());

    auto tsr = co_await user.realm<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>("foo");
    auto synced_realm = tsr.resolve();
    synced_realm.write([&synced_realm]() {
        synced_realm.add(AllTypesObject{._id=1});
    });

    CHECK_EQUALS(*synced_realm.object<AllTypesObject>(1)._id, 1);

    co_return;
}

TEST(flx_sync) {
    auto app = realm::App(Admin::Session::shared.create_app<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({"str_col", "_id"}), "http://localhost:9090");
    auto user = co_await app.login(realm::App::Credentials::anonymous());
    auto flx_sync_config = user.flexible_sync_configuration();

    try {
        auto tsr = co_await realm::async_open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(flx_sync_config);
        auto synced_realm = tsr.resolve();

        auto update_success = co_await synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet &subs) {
            subs.clear();
        });
        CHECK_EQUALS(update_success, true);
        CHECK_EQUALS(synced_realm.subscriptions().size(), 0);

        update_success = co_await synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet &subs) {
            subs.add<AllTypesObject>("foo-strings", [](auto &obj) {
                return obj.str_col == "foo";
            });
        });
        CHECK_EQUALS(update_success, true);
        CHECK_EQUALS(synced_realm.subscriptions().size(), 1);

        auto sub = *synced_realm.subscriptions().find("foo-strings");
        CHECK_EQUALS(sub.name(), "foo-strings")
        CHECK_EQUALS(sub.object_class_name(), "AllTypesObject")
        CHECK_EQUALS(sub.query_string(), "str_col == \"foo\"")

        auto non_existent_sub = synced_realm.subscriptions().find("non-existent");
        CHECK_EQUALS((non_existent_sub == std::nullopt), true)

        synced_realm.write([&synced_realm]() {
            synced_realm.add(AllTypesObject{._id=1, .str_col="foo"});
        });

        co_await test::wait_for_sync_uploads(user);
        co_await test::wait_for_sync_downloads(user);
        synced_realm.write([]() {}); // refresh realm
        auto objs = synced_realm.objects<AllTypesObject>();

        CHECK_EQUALS(objs[0]->_id, 1)

        update_success = co_await synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet &subs) {
            subs.update_subscription<AllTypesObject>("foo-strings", [](auto &obj) {
                return obj.str_col == "bar" && obj._id == 123;
            });
        });

        sub = *synced_realm.subscriptions().find("foo-strings");
        CHECK_EQUALS(sub.name(), "foo-strings");
        CHECK_EQUALS(sub.object_class_name(), "AllTypesObject");
        CHECK_EQUALS(sub.query_string(), "str_col == \"bar\" and _id == 123");

        synced_realm.write([&synced_realm]() {
            synced_realm.add(AllTypesObject{._id=123, .str_col="bar"});
        });

        co_await test::wait_for_sync_uploads(user);
        co_await test::wait_for_sync_downloads(user);

        synced_realm.write([]() {}); // refresh realm
        objs = synced_realm.objects<AllTypesObject>();
        CHECK_EQUALS(objs.size(), 1);

        update_success = co_await synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet &subs) {
            subs.update_subscription<AllTypesObject>("foo-strings");
        });

        sub = *synced_realm.subscriptions().find("foo-strings");
        CHECK_EQUALS(sub.name(), "foo-strings");
        CHECK_EQUALS(sub.object_class_name(), "AllTypesObject");
        CHECK_EQUALS(sub.query_string(), "TRUEPREDICATE");

        co_await test::wait_for_sync_uploads(user);
        co_await test::wait_for_sync_downloads(user);

        synced_realm.write([]() {}); // refresh realm
        objs = synced_realm.objects<AllTypesObject>();
        CHECK_EQUALS(objs.size(), 2);
    } catch (const std::exception& err) {
    }
    co_return;
}

TEST(tsr) {
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
        CHECK_EQUALS(*person.age, 17);
        realm.write([&] { realm.remove(person); });
    });
    t.join();
    co_return;
}

TEST(query_) {
    auto realm = realm::open<Person, Dog>({.path=path});

    auto person = Person { .name = "John", .age = 42 };
    realm.write([&realm, &person](){
        realm.add(person);
    });

    auto results = realm.objects<Person>().where("age > $0", {42});
    CHECK_EQUALS(results.size(), 0);
    results = realm.objects<Person>().where("age = $0", {42});
    CHECK_EQUALS(results.size(), 1);
    std::unique_ptr<Person> john = results[0];
    CHECK_EQUALS(john->age, 42);
    CHECK_EQUALS(john->name, "John");

    co_return;
}

TEST(results_notifications) {
    auto realm = realm::open<Person, Dog>({.path=path});

    auto person = Person { .name = "John", .age = 42 };
    realm.write([&realm, &person](){
        realm.add(person);
    });
    auto results = realm.objects<Person>();

    bool did_run = false;

    realm::results_change<Person> change;

    auto require_change = [&] {
        auto token = results.observe([&](realm::results_change<Person> c) {
            did_run = true;
            change = std::move(c);
        });
        return token;
    };

    auto token = require_change();
    auto person2 = Person { .name = "Jane", .age = 24 };
    realm.write([&realm, &person2](){
        realm.add(person2);
    });
    realm.write([] { });

    CHECK_EQUALS(change.insertions.size(), 1);
    CHECK_EQUALS(change.collection->size(), 2);
    CHECK_EQUALS(did_run, true);

    co_return;
}


TEST(results_notifications_insertions) {
    auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded, Dog>({.path=path});
    realm.write([&realm] {
        realm.add(AllTypesObject { ._id = 1 });
    });

    bool did_run = false;

    realm::results_change<AllTypesObject> change;

    int callback_count = 0;
    auto results = realm.objects<AllTypesObject>();
    auto require_change = [&] {
        auto token = results.observe([&](realm::results_change<AllTypesObject> c) {
            CHECK_EQUALS(c.collection , &results);
            callback_count++;
            change = std::move(c);
        });
        return token;
    };

    auto token = require_change();
    realm.write([&realm] {
        realm.add(AllTypesObject { ._id = 2 });
    });

    realm.write([] { });

    CHECK_EQUALS(change.insertions.size(), 1);
    CHECK_EQUALS(change.deletions.size(), 0);
    CHECK_EQUALS(change.modifications.size(), 0);

    realm.write([&realm] {
        realm.add(AllTypesObject { ._id = 3 });
        realm.add(AllTypesObject { ._id = 4 });
    });

    realm.write([] { });

    CHECK_EQUALS(change.insertions.size(), 2);
    CHECK_EQUALS(change.deletions.size(), 0);
    CHECK_EQUALS(change.modifications.size(), 0);
    CHECK_EQUALS(callback_count, 3);

    co_return;
}

TEST(results_notifications_deletions) {
    auto obj = AllTypesObject();

    auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded, Dog>({.path=path});
    realm.write([&realm, &obj] {
        realm.add(obj);
    });

    bool did_run = false;
    realm::results_change<AllTypesObject> change;
    auto results = realm.objects<AllTypesObject>();

    auto require_change = [&] {
        auto token = results.observe([&](realm::results_change<AllTypesObject> c) {
            did_run = true;
            change = std::move(c);
        });
        return token;
    };

    auto token = require_change();
    realm.write([&realm, &obj] {
        realm.remove(obj);
    });
    realm.write([] { });
    CHECK_EQUALS(change.deletions.size(), 1);
    CHECK_EQUALS(change.insertions.size(), 0);
    CHECK_EQUALS(change.modifications.size(), 0);
    CHECK_EQUALS(did_run, true);

    co_return;
}

TEST(results_notifications_modifications) {
    auto obj = AllTypesObject();

    auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded, Dog>({.path=path});
    realm.write([&realm, &obj] {
        realm.add(obj);
    });

    bool did_run = false;

    realm::results_change<AllTypesObject> change;
    auto results = realm.objects<AllTypesObject>();

    auto require_change = [&] {
        auto token = results.observe([&](realm::results_change<AllTypesObject> c) {
            did_run = true;
            change = std::move(c);
        });
        return token;
    };

    auto token = require_change();
    realm.write([&realm, &obj] {
        obj.str_col = "foobar";
    });
    realm.write([] { });
    CHECK_EQUALS(change.modifications.size(), 1);
    CHECK_EQUALS(change.insertions.size(), 0);
    CHECK_EQUALS(change.deletions.size(), 0);
    CHECK_EQUALS(did_run, true);

    co_return;
}

TEST(binary) {
    auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({.path=path});
    auto obj = AllTypesObject();
    obj.binary_col.push_back(1);
    obj.binary_col.push_back(2);
    obj.binary_col.push_back(3);
    realm.write([&realm, &obj] {
        realm.add(obj);
    });
    realm.write([&realm, &obj] {
        obj.binary_col.push_back(4);
    });
    CHECK_EQUALS(obj.binary_col[0], 1);
    CHECK_EQUALS(obj.binary_col[1], 2);
    CHECK_EQUALS(obj.binary_col[2], 3);
    CHECK_EQUALS(obj.binary_col[3], 4);
    co_return;
}

TEST(date) {
    auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({.path=path});
    auto obj = AllTypesObject();
    CHECK_EQUALS(obj.date_col, std::chrono::time_point<std::chrono::system_clock>{});
    auto now = std::chrono::system_clock::now();
    obj.date_col = now;
    CHECK_EQUALS(obj.date_col, now);
    realm.write([&realm, &obj] {
        realm.add(obj);
    });
    CHECK_EQUALS(obj.date_col, now);
    realm.write([&realm, &obj] {
        obj.date_col += std::chrono::seconds(42);
    });
    CHECK_EQUALS(obj.date_col, now + std::chrono::seconds(42));
    co_return;
}

TEST(login_username_password) {
    auto app_id = Admin::Session::shared.create_app();
    auto app = realm::App(app_id, "http://localhost:9090");
    try {
        co_await app.login(realm::App::Credentials::apple("id_token"));
    } catch (const std::exception& err) {
    }

    co_await app.register_user("foo@mongodb.com", "foobar");
    auto user = co_await app.login(realm::App::Credentials::username_password("foo@mongodb.com", "foobar"));
    CHECK(!user.access_token().empty())
    co_return;
}

TEST(embedded) {
    auto realm = realm::open<Foo, EmbeddedFoo>({.path=path});

    auto foo = Foo();
    foo.foo = EmbeddedFoo{.bar=42};

    realm.write([&foo, &realm](){
        realm.add(foo);
    });

    CHECK_EQUALS((*foo.foo).bar, 42)
    bool did_run;
    EmbeddedFoo e_foo = (*foo.foo);
    auto token = e_foo.observe<EmbeddedFoo>([&did_run](auto change){
        CHECK_EQUALS(change.object->bar, 84)
        did_run = true;
    });
    realm.write([&foo](){
        (*foo.foo).bar = 84;
    });
    CHECK_EQUALS((*foo.foo).bar, 84)
    realm.write([&foo, &realm]{
        realm.remove(foo);
    });
    CHECK(did_run)
    co_return;
}

//@end
