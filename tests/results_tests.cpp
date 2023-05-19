#include "main.hpp"
#include "test_objects.hpp"
#include <cpprealm/notifications.hpp>

using namespace realm;

TEST_CASE("results", "[results]") {
    realm_path path;
    db_config config;
    config.set_path(path);
    SECTION("results_notifications") {
        auto realm = realm::open<Person, Dog>(std::move(config));

        Person person;
        person.name = "John";
        person.age = 42;
        realm.write([&realm, &person]() {
            realm.add(person);
        });
        auto results = realm.objects<Person>();

        bool did_run = false;

        realm::results_change<Person> change;

        auto require_change = [&] {
            auto token = results.observe([&](results_change<Person> c) {
                did_run = true;
                change = std::move(c);
            });
            return token;
        };

        auto token = require_change();
        Person person2;
        person2.name = "Jane";
        person2.age = 24;
        realm.write([&realm, &person2]() {
            realm.add(person2);
        });
        realm.write([] {});

        CHECK(change.insertions.size() == 1);
        CHECK(change.collection->size() == 2);
        CHECK(did_run);
    }

    SECTION("results_notifications_insertions") {
        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded, Dog>(std::move(config));
        realm.write([&realm] {
            AllTypesObject o;
            o._id = 1;
            realm.add(std::move(o));
        });

        bool did_run = false;

        realm::results_change<AllTypesObject> change;

        int callback_count = 0;
        auto results = realm.objects<AllTypesObject>();
        auto require_change = [&] {
            auto token = results.observe([&](realm::results_change<AllTypesObject> c) {
                CHECK(c.collection == &results);
                callback_count++;
                change = std::move(c);
            });
            return token;
        };

        auto token = require_change();
        realm.write([&realm] {
            AllTypesObject o;
            o._id = 2;
            realm.add(std::move(o));
        });

        realm.write([] {});

        CHECK(change.insertions.size() == 1);
        CHECK(change.deletions.size() == 0);
        CHECK(change.modifications.size() == 0);

        realm.write([&realm] {
            AllTypesObject o1;
            o1._id = 3;
            realm.add(std::move(o1));
            AllTypesObject o2;
            o2._id = 4;
            realm.add(std::move(o2));
        });

        realm.write([] {});

        CHECK(change.insertions.size() == 2);
        CHECK(change.deletions.empty());
        CHECK(change.modifications.empty());
        CHECK(callback_count == 3);
    }

    SECTION("results_notifications_deletions") {
        auto obj = AllTypesObject();

        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded, Dog>(std::move(config));
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
        realm.write([] {});
        CHECK(change.deletions.size() == 1);
        CHECK(change.insertions.size() == 0);
        CHECK(change.modifications.size() == 0);
        CHECK(did_run);
    }

    SECTION("results_notifications_modifications") {
        auto obj = AllTypesObject();

        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded, Dog>(std::move(config));
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
        realm.write([] {});
        CHECK(change.modifications.size() == 1);
        CHECK(change.insertions.size() == 0);
        CHECK(change.deletions.size() == 0);
        CHECK(did_run);
    }
}