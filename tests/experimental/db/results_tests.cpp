#include "../../main.hpp"
#include "test_objects.hpp"

namespace realm::experimental {

    TEST_CASE("results", "[results]") {
        realm_path path;
        db_config config;
        config.set_path(path);
        SECTION("results_notifications") {
            auto realm = db(std::move(config));

            AllTypesObject obj;
            obj.str_col = "John";
            obj._id = 1;
            realm.write([&realm, &obj]() {
                realm.add(std::move(obj));
            });
            auto results = realm.objects<AllTypesObject>();

            bool did_run = false;

            realm::experimental::results<AllTypesObject>::results_change change;

            auto require_change = [&] {
                auto token = results.observe([&](auto&& c) {
                    did_run = true;
                    change = std::move(c);
                });
                return token;
            };

            auto token = require_change();
            AllTypesObject obj2;
            obj2.str_col = "Jane";
            obj2._id = 2;
            realm.write([&realm, &obj2]() {
                realm.add(std::move(obj2));
            });
            realm.refresh();

            CHECK(change.insertions.size() == 1);
            CHECK(change.collection->size() == 2);
            CHECK(did_run);
        }

        SECTION("results_notifications_insertions") {
            auto realm = db(std::move(config));
            realm.write([&realm] {
                AllTypesObject o;
                o._id = 1;
                realm.add(std::move(o));
            });

            bool did_run = false;

            results<AllTypesObject>::results_change change;

            int callback_count = 0;
            auto results = realm.objects<AllTypesObject>();
            auto require_change = [&] {
                auto token = results.observe([&](auto&& c) {
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

            auto realm = db(std::move(config));
            auto managed_obj = realm.write([&realm, &obj] {
                return realm.add(std::move(obj));
            });

            bool did_run = false;
            results<AllTypesObject>::results_change change;
            auto results = realm.objects<AllTypesObject>();

            auto require_change = [&] {
                auto token = results.observe([&](auto&& c) {
                    did_run = true;
                    change = std::move(c);
                });
                return token;
            };

            auto token = require_change();
            realm.write([&realm, &managed_obj] {
                realm.remove(managed_obj);
            });
            realm.refresh();
            CHECK(change.deletions.size() == 1);
            CHECK(change.insertions.size() == 0);
            CHECK(change.modifications.size() == 0);
            CHECK(did_run);
        }

        SECTION("results_notifications_modifications") {
            auto obj = AllTypesObject();

            auto realm = db(std::move(config));
            auto managed_obj = realm.write([&realm, &obj] {
                return realm.add(std::move(obj));
            });

            bool did_run = false;

            results<AllTypesObject>::results_change change;
            auto results = realm.objects<AllTypesObject>();

            auto require_change = [&] {
                auto token = results.observe([&](auto&& c) {
                    did_run = true;
                    change = std::move(c);
                });
                return token;
            };

            auto token = require_change();
            realm.write([&realm, &managed_obj] {
                managed_obj.str_col = "foobar";
            });
            realm.refresh();
            CHECK(change.modifications.size() == 1);
            CHECK(change.insertions.size() == 0);
            CHECK(change.deletions.size() == 0);
            CHECK(did_run);
        }
    }
}