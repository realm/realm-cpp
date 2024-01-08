#include "../main.hpp"
#include "test_objects.hpp"

namespace realm {
    struct results_wrapper {
        explicit results_wrapper(const db_config &c) {
            m_realm = std::make_unique<db>(c);
        }
        auto get_results() const {
            return m_realm->objects<AllTypesObject>()
                    .where([&](auto &data) { return data.double_col != 5 && data.str_col == "foo"; });
        }

        void observe() {
            m_token = get_results().observe([&](auto &&c) {
                run_count++;
                realm::results<realm::AllTypesObject> *res = c.collection;
                CHECK_FALSE(res == nullptr);
            });
        }

        void unregister() {
            m_token.unregister();
        }

        size_t run_count = 0;
    private:
        std::unique_ptr<db> m_realm;
        realm::notification_token m_token;
    };

    TEST_CASE("results", "[results]") {
        realm_path path;
        realm::db_config config;
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

            realm::results<AllTypesObject>::results_change change;

            auto require_change = [&] {
                auto token = results.observe([&](auto&& c) {
                    did_run = true;
                    change = std::move(c);
                    CHECK_FALSE(c.collection == nullptr);
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
                    CHECK(c.collection == &results);
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
                    CHECK(c.collection == &results);
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

        managed<AllTypesObject> test_obj;

        SECTION("results_subscript") {
            auto realm = db(std::move(config));

            AllTypesObject obj;
            obj.str_col = "John";
            obj._id = 1;
            obj.double_col = 1;
            auto managed_obj = realm.write([&realm, &obj]() {
                return realm.add(std::move(obj));
            });
            auto results = realm.objects<AllTypesObject>();
            {
                auto results1 = realm.objects<AllTypesObject>();
                test_obj = results1[0];
            }
            auto o = results[0];
            CHECK(o.str_col == "John");
            CHECK(o.list_obj_col.size() == 0);
            auto copy_o = o;
            CHECK(copy_o.str_col == "John");
            CHECK(copy_o.list_obj_col.size() == 0);
            auto moved_o = std::move(o);
            CHECK(moved_o.str_col == "John");
            CHECK(moved_o.list_obj_col.size() == 0);

            realm.write([&](){
                test_obj.double_col = 123.456;
            });
            CHECK(copy_o.double_col == 123.456);
            CHECK(moved_o.double_col == 123.456);
        }

        SECTION("results_iterator") {
            auto realm = db(std::move(config));

            AllTypesObject obj;
            obj.str_col = "foo";
            obj._id = 1;

            AllTypesObject obj2;
            obj2.str_col = "bar";
            obj2._id = 2;

            realm.write([&realm, &obj, &obj2]() {
                realm.add(std::move(obj));
                realm.add(std::move(obj2));

            });
            auto results = realm.objects<AllTypesObject>();

            size_t count = 0;
            for (auto& o : results) {
                count++;
                if (count == 1) {
                    CHECK(o._id == 1);
                    CHECK(o.str_col == "foo");
                } else {
                    CHECK(o._id == 2);
                    CHECK(o.str_col == "bar");
                }
            }
            CHECK(count == 2);
        }

        SECTION("results_iterator") {
            auto realm = db(std::move(config));

            AllTypesObject obj;
            obj.str_col = "foo";
            obj._id = 1;

            AllTypesObject obj2;
            obj2.str_col = "bar";
            obj2._id = 2;

            realm.write([&realm, &obj, &obj2]() {
                realm.add(std::move(obj));
                realm.add(std::move(obj2));

            });
            auto results = realm.objects<AllTypesObject>();

            size_t count = 0;
            for (auto& o : results) {
                count++;
                if (count == 1) {
                    CHECK(o._id == 1);
                    CHECK(o.str_col == "foo");
                } else {
                    CHECK(o._id == 2);
                    CHECK(o.str_col == "bar");
                }
            }
            CHECK(count == 2);
        }

        SECTION("results_query") {
            auto realm = db(std::move(config));

            AllTypesObject obj;
            obj.str_col = "foo";
            obj._id = 1;

            AllTypesObject obj2;
            obj2.str_col = "bar";
            obj2._id = 2;

            realm.write([&realm, &obj, &obj2]() {
                realm.add(std::move(obj));
                realm.add(std::move(obj2));

            });
            auto results = realm.objects<AllTypesObject>();
            auto queried_results = results.where([](auto& o) {
                return o._id == 1;
            });
            CHECK(queried_results.size() == 1);
            CHECK(queried_results[0]._id == 1);

            auto queried_results_string = results.where("_id == $0", {(int64_t)2});
            CHECK(queried_results_string.size() == 1);
            CHECK(queried_results_string[0]._id == 2);

            queried_results_string = results.where("_id == $0", {(int64_t)3});
            CHECK(queried_results_string.size() == 0);
        }

        SECTION("results_sort") {
            auto realm = db(std::move(config));

            AllTypesObject obj;
            obj.str_col = "foo";
            obj._id = 1;

            AllTypesObject obj2;
            obj2.str_col = "bar";
            obj2._id = 2;

            realm.write([&realm, &obj, &obj2]() {
                realm.add(std::move(obj));
                realm.add(std::move(obj2));
            });
            auto sorted_results_ascending = realm.objects<AllTypesObject>().sort("str_col", true);
            CHECK(sorted_results_ascending[0].str_col == "bar");
            CHECK(sorted_results_ascending[1].str_col == "foo");
            auto sorted_results_descending = realm.objects<AllTypesObject>().sort("str_col", false);
            CHECK(sorted_results_descending[0].str_col == "foo");
            CHECK(sorted_results_descending[1].str_col == "bar");

            auto sorted_results_with_descriptors_ascending = realm.objects<AllTypesObject>().sort({{"str_col", true}, {"_id", true}});
            CHECK(sorted_results_with_descriptors_ascending[0].str_col == "bar");
            CHECK(sorted_results_with_descriptors_ascending[1].str_col == "foo");
            auto sorted_results_with_descriptors_descending = realm.objects<AllTypesObject>().sort({{"str_col", false}, {"_id", false}});
            CHECK(sorted_results_with_descriptors_descending[0].str_col == "foo");
            CHECK(sorted_results_with_descriptors_descending[1].str_col == "bar");
        }

        SECTION("observe_results_derived_from_list") {
            auto realm = db(std::move(config));

            AllTypesObjectLink link;
            link.str_col = "bar";
            link._id = 1;

            AllTypesObject obj;
            obj.str_col = "foo";
            obj._id = 1;
            obj.list_obj_col.push_back(&link);

            realm.write([&realm, &obj]() {
                return realm.add(std::move(obj));
            });

            auto res = realm.objects<AllTypesObject>()[0].list_obj_col.where([](auto&& o) {
                return o._id == 1;
            });

            int notification_count = 0;
            auto token = res.observe([&notification_count](auto&& change) {
                notification_count++;
            });

            realm.write([&realm, &res]() {
                res[0].str_col = "foo";
            });
            realm.refresh();

            realm.write([&realm, &res]() {
                res[0].str_col = "bar";
            });
            realm.refresh();

            CHECK(notification_count == 3);
            token.unregister();
            realm.write([&realm, &res]() {
                res[0].str_col = "abc";
            });
            realm.refresh();
            CHECK(notification_count == 3);
        }

        SECTION("observe_results_in_wrapper") {
            auto realm = db(config);

            AllTypesObjectLink link;
            link.str_col = "bar";
            link._id = 1;

            AllTypesObject obj;
            obj.str_col = "foo";
            obj._id = 1;
            obj.list_obj_col.push_back(&link);

            realm.write([&realm, &obj]() {
                return realm.add(std::move(obj));
            });

            results_wrapper wrapper(config);
            wrapper.observe();
            auto res = wrapper.get_results();

            realm.write([&realm, &res]() {
                res[0].str_col = "foo";
            });
            realm.refresh();

            realm.write([&realm, &res]() {
                res[0].str_col = "bar";
            });
            realm.refresh();

            CHECK(wrapper.run_count == 3);
            wrapper.unregister();
            realm.write([&realm, &res]() {
                AllTypesObject o;
                o.str_col = "foo";
                o._id = 1;
                realm.add(std::move(o));
            });
            realm.refresh();
            CHECK(wrapper.run_count == 3);
        }
    }
}