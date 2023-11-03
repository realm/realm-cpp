#include "test_objects.hpp"
#include "../../main.hpp"

namespace realm::experimental {
    TEST_CASE("frozen_realm") {
        realm_path path;
        realm::db_config config;
        config.set_path(path);

        SECTION("is_frozen") {
            auto realm = db(std::move(config));
            auto frozen_realm = realm.freeze();
            CHECK(frozen_realm.is_frozen());

            auto frozen_realm_copy = frozen_realm;
            CHECK(frozen_realm_copy.is_frozen());
        }

        SECTION("refresh_frozen") {
            auto realm = db(std::move(config));
            auto frozen_realm = db(realm.freeze());
            CHECK(frozen_realm.is_frozen());

            realm.write([&realm]() {
                realm.add(AllTypesObject());
            });

            CHECK_FALSE(frozen_realm.refresh());
            CHECK(frozen_realm.objects<AllTypesObject>().size() == 0);
        }

        SECTION("forbidden_methods") {
            auto realm = db(std::move(config));
            auto frozen_realm = db(realm.freeze());
            CHECK(frozen_realm.is_frozen());

            CHECK_THROWS_WITH(frozen_realm.begin_write(), "Can't perform transactions on a frozen Realm");
            CHECK_THROWS_WITH(frozen_realm.objects<AllTypesObject>().observe([](auto) { }), "Notifications are not available on frozen collections since they do not change.");
        }

        SECTION("object_is_frozen_from_results") {
            auto realm = db(std::move(config));
            realm.write([&realm]() {
                realm.add(AllTypesObject());
            });
            auto frozen_realm = db(realm.freeze());
            frozen_realm.invalidate();
            auto managed_obj = frozen_realm.objects<AllTypesObject>()[0];
            CHECK(managed_obj.is_frozen());
        }

        SECTION("thaw") {
            auto realm = db(std::move(config));
            realm.write([&realm]() {
                realm.add(AllTypesObject());
            });
            auto frozen_realm = realm.freeze();
            CHECK(frozen_realm.is_frozen());

            auto thawed = frozen_realm.thaw();
            CHECK_FALSE(thawed.is_frozen());
            thawed.write([&thawed]() {
                auto o = AllTypesObject();
                o._id = 2;
                thawed.add(std::move(o));
            });
            thawed.write([&thawed]() {
                auto o = AllTypesObject();
                o._id = 3;
                thawed.add(std::move(o));
            });
            CHECK_FALSE(thawed.refresh());
            CHECK(thawed.objects<AllTypesObject>().size() == 3);

            CHECK(frozen_realm.is_frozen());
            CHECK_FALSE(frozen_realm.refresh());
            CHECK(frozen_realm.objects<AllTypesObject>().size() == 1);
        }

        SECTION("thaw_different_thread") {
            auto realm = db(std::move(config));
            realm.write([&realm]() {
                realm.add(AllTypesObject());
            });
            auto frozen_realm = realm.freeze();
            CHECK(frozen_realm.is_frozen());

            auto async = std::async(std::launch::async, [&frozen_realm](){
                auto thawed = frozen_realm.thaw();
                CHECK_FALSE(thawed.is_frozen());
                thawed.write([&thawed]() {
                    auto o = AllTypesObject();
                    o._id = 2;
                    thawed.add(std::move(o));
                });
                thawed.write([&thawed]() {
                    auto o = AllTypesObject();
                    o._id = 3;
                    thawed.add(std::move(o));
                });
                CHECK_FALSE(thawed.refresh());
                CHECK(thawed.objects<AllTypesObject>().size() == 3);

                CHECK(frozen_realm.is_frozen());
                CHECK_FALSE(frozen_realm.refresh());
                CHECK(frozen_realm.objects<AllTypesObject>().size() == 1);
                return true;
            });
            CHECK(async.get());
        }

        SECTION("thaw_previous_version") {
            auto realm = db(std::move(config));
            auto frozen_realm = realm.freeze();
            CHECK(frozen_realm.is_frozen());
            realm.write([&realm]() {
                realm.add(AllTypesObject());
            });
            CHECK(frozen_realm.objects<AllTypesObject>().size() == 0);

            auto thawed = frozen_realm.thaw();
            CHECK_FALSE(thawed.is_frozen());
            CHECK(thawed.refresh());
            CHECK(thawed.objects<AllTypesObject>().size() == 1);
        }

        SECTION("freeze_same_realm_returns_self") {
            db realm = db(std::move(config));
            db frozen_realm1 = realm.freeze();
            db frozen_realm2 = realm.freeze();
            db frozen_realm3 = frozen_realm2.freeze();

            CHECK(frozen_realm1 == frozen_realm2);
            CHECK(frozen_realm2 == frozen_realm3);
            CHECK_FALSE(realm == frozen_realm1);
        }

        SECTION("frozen_object") {
            auto realm = db(std::move(config));
            auto object = realm.write([&realm]() {
                return realm.add(AllTypesObject());
            });

            auto frozen_object = object.freeze();
            CHECK(frozen_object.is_frozen());
            auto same_frozen_object = frozen_object.freeze();
            CHECK(frozen_object == same_frozen_object);
            CHECK_FALSE(frozen_object == object);
        }

        SECTION("frozen_object_different_thread") {
            auto realm = db(std::move(config));
            auto object = realm.write([&realm]() {
                auto o = AllTypesObject();
                o.str_col = "bar";
                o._id = 1;

                auto link = AllTypesObjectLink();
                link._id = 1;
                link.str_col = "foobar";

                o.opt_obj_col = &link;
                o.list_obj_col.push_back(&link);
                o.set_obj_col.insert(&link);
                o.map_link_col["foo"] = &link;

                return realm.add(std::move(o));
            });

            auto frozen_object = object.freeze();
            CHECK(frozen_object.is_frozen());

            auto async = std::async(std::launch::async, [&frozen_object]() {
                CHECK(frozen_object.is_frozen());
                CHECK(frozen_object._id == 1);
                CHECK(frozen_object.str_col == "bar");
                CHECK(frozen_object.opt_obj_col->_id == 1);
                CHECK(frozen_object.opt_obj_col->str_col == "foobar");
                CHECK(frozen_object.list_obj_col[0]->str_col == "foobar");
                CHECK(frozen_object.map_link_col["foo"]->str_col == "foobar");
                CHECK((*frozen_object.set_obj_col.find(frozen_object.opt_obj_col)).str_col == "foobar");

                return true;
            });
            CHECK(async.get());
        }

        SECTION("frozen_object_observe") {
            auto realm = db(std::move(config));
            auto object = realm.write([&realm]() {
                auto o = AllTypesObject();
                o._id = 1;
                return realm.add(std::move(o));
            });

            auto frozen_object = object.freeze();
            CHECK(frozen_object.is_frozen());

            CHECK_THROWS_WITH(frozen_object.observe([](auto) { }), "Notifications are not available on frozen collections since they do not change.");
            CHECK_THROWS_WITH(frozen_object.list_int_col.observe([](auto) { }), "Notifications are not available on frozen collections since they do not change.");
            CHECK_THROWS_WITH(frozen_object.list_obj_col.observe([](auto) { }), "Notifications are not available on frozen collections since they do not change.");
            CHECK_THROWS_WITH(frozen_object.set_int_col.observe([](auto) { }), "Notifications are not available on frozen collections since they do not change.");
            CHECK_THROWS_WITH(frozen_object.set_obj_col.observe([](auto) { }), "Notifications are not available on frozen collections since they do not change.");
            CHECK_THROWS_WITH(frozen_object.map_int_col.observe([](auto) { }), "Notifications are not available on frozen collections since they do not change.");
            CHECK_THROWS_WITH(frozen_object.map_link_col.observe([](auto) { }), "Notifications are not available on frozen collections since they do not change.");
        }

        SECTION("frozen_object_comparison") {
            auto realm = db(std::move(config));
            auto object = realm.write([&realm]() {
                auto o = AllTypesObject();
                o._id = 1;
                return realm.add(std::move(o));
            });

            auto frozen_object = object.freeze();
            CHECK(frozen_object.is_frozen());
            auto frozen_object2 = object.freeze();
            CHECK(frozen_object2.is_frozen());

            CHECK(frozen_object == frozen_object2);
            CHECK_FALSE(frozen_object != frozen_object2);
            CHECK(object != frozen_object2);
            CHECK_FALSE(object == frozen_object2);
        }

        SECTION("frozen_object_mutate") {
            auto realm = db(std::move(config));
            auto object = realm.write([&realm]() {
                auto o = AllTypesObject();
                o._id = 1;
                return realm.add(std::move(o));
            });

            auto frozen_object = object.freeze();
            realm.write([&realm, &object]() {
                object.str_col = "doesn't throw exception";
            });

            CHECK_THROWS_WITH(
                realm.write([&realm, &frozen_object]() {
                    frozen_object.str_col = "throws exception";
                }), "Trying to modify database while in read transaction"
            );
        }

        SECTION("frozen_object_linking_objects") {
            auto realm = db(std::move(config));

            realm.write([&realm]() {
                Dog dog;
                dog._id = 1;
                dog.name = "Spot";

                Person person;
                person.name = "John";
                person._id = 1;
                person.dog = &dog;
                realm.add(std::move(person));
            });

            auto dog = realm.objects<Dog>().freeze()[0];
            auto frozen_object = dog;
            auto owner = frozen_object.owners[0];
            CHECK(owner.is_frozen());
            CHECK(owner.name == "John");
        }

        SECTION("frozen_thawed_results") {
            auto realm = db(std::move(config));

            realm.write([&realm]() {
                Dog dog;
                dog._id = 1;
                dog.name = "Spot";

                Person person;
                person.name = "John";
                person._id = 1;
                person.dog = &dog;
                realm.add(std::move(person));
            });

            auto frozen_results = realm.objects<Dog>().freeze();
            auto dog = frozen_results[0];
            CHECK(dog.is_frozen());
            CHECK(frozen_results.is_frozen());
            CHECK_THROWS_WITH(frozen_results.observe([](auto) { }), "Notifications are not available on frozen collections since they do not change.");
            auto thawed_results = frozen_results.thaw();
            CHECK_FALSE(thawed_results.is_frozen());
            dog = thawed_results[0];
            CHECK_FALSE(dog.is_frozen());
        }
    }
}