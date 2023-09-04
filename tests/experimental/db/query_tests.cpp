#include "../../main.hpp"
#include "test_objects.hpp"

namespace realm::experimental {

#define query_results_size(Cls, fn)  \
    realm.template objects<Cls>().where(fn).size()

    TEST_CASE("query") {
        realm_path path;
        db_config config;
        config.set_path(path);
        SECTION("tsq_basic_comparison", "[query]") {
            auto realm = db(std::move(config));
            auto date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            auto object_id = realm::object_id::generate();

            auto create_obj = [&]() {
                AllTypesObject obj;
                AllTypesObjectLink link;
                link._id = 1;
                link.str_col = "foo";

                obj._id = 123;
                obj.opt_obj_col = &link;
                obj.str_col = "foo bar";
                obj.enum_col = AllTypesObject::Enum::two;
                obj.date_col = std::chrono::system_clock::from_time_t(date);
                obj.uuid_col = realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002");
                obj.object_id_col = object_id;

                obj.binary_col.push_back(0);
                obj.binary_col.push_back(1);
                obj.binary_col.push_back(2);
                obj.binary_col.push_back(3);

                auto managed_obj = realm.write([&]() {
                    return realm.add(std::move(obj));
                });

                return managed_obj;
            };

            auto obj = create_obj();
            auto obj2 = create_obj();

            // With literal as RHS.
            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id == 123; }) == 1);
            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id != 123; }) == 0);
            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.str_col == "foo bar"; }) == 1);
            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.str_col != "foo bar"; }) == 0);
            CHECK(query_results_size(AllTypesObject, [](auto &o) {
                      return o.binary_col == std::vector<uint8_t>({0, 1, 2, 3});
                  }) == 1);
            CHECK(query_results_size(AllTypesObject,
                                     [](auto &o) { return o.binary_col != std::vector<uint8_t>({0, 1, 2, 3}); }) == 0);
            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col == AllTypesObject::Enum::two; }) == 1);
            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col != AllTypesObject::Enum::two; }) == 0);
            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col == std::chrono::system_clock::from_time_t(date);
                  }) == 1);
            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col != std::chrono::system_clock::from_time_t(date);
                  }) == 0);
            CHECK(query_results_size(AllTypesObject, [](auto &o) {
                      return o.uuid_col == realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002");
                  }) == 1);
            CHECK(query_results_size(AllTypesObject, [](auto &o) {
                      return o.uuid_col != realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002");
                  }) == 0);

            auto x = realm.objects<AllTypesObject>().where([](auto& o) { return o.opt_obj_col->str_col == "foo"; }).size();
            CHECK(x == 1);
        }

        SECTION("tsq_greater_less_than", "[query]") {
            auto realm = db(std::move(config));
            auto date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

            auto create_obj = [&]() {
                AllTypesObject obj;
                obj._id = 123;
                obj.enum_col = AllTypesObject::Enum::two;
                obj.date_col = std::chrono::system_clock::from_time_t(date);

                return realm.write([&]() {
                    return realm.add(std::move(obj));
                });
            };

            auto obj = create_obj();
            auto obj2 = create_obj();

            // With literal as RHS.
            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id > 123; }) == 0);
            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id >= 123; }) == 1);
            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id < 123; }) == 0);
            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id <= 123; }) == 1);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col > AllTypesObject::Enum::two; }) == 0);
            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col >= AllTypesObject::Enum::two; }) == 1);
            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col < AllTypesObject::Enum::two; }) == 0);
            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col <= AllTypesObject::Enum::two; }) == 1);


            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col > std::chrono::system_clock::from_time_t(date);
                  }) == 0);
            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col >= std::chrono::system_clock::from_time_t(date);
                  }) == 1);
            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col < std::chrono::system_clock::from_time_t(date);
                  }) == 0);
            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col <= std::chrono::system_clock::from_time_t(date);
                  }) == 1);
        }

        SECTION("tsq_compound", "[query]") {
            auto realm = db(std::move(config));

            AllTypesObject obj;
            obj._id = 123;
            obj.str_col = "John";
            obj.bool_col = true;

            realm.write([&]() {
                realm.add(std::move(obj));
            });

            auto results = realm.objects<AllTypesObject>().where([](auto &o) {
                return o._id == 123 && o.str_col != "John";
            });
            CHECK(results.size() == 0);
            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return o._id == 123 || o.str_col != "John";
            });
            CHECK(results.size() == 1);
            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return o._id == 123 && o.str_col == "John";
            });
            CHECK(results.size() == 1);
            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return o._id == 123 && o.str_col == "John" || o.bool_col == true;
            });
            CHECK(results.size() == 1);
            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return o._id == 123 && o.str_col.contains("Jo") || o.bool_col == true;
            });
            CHECK(results.size() == 1);
            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return o.str_col.empty();
            });
            CHECK(results.size() == 0);
            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return !o.str_col.empty();
            });
            CHECK(results.size() == 1);
        }

        SECTION("optionals") {
            auto realm = db(std::move(config));

            auto obj = AllTypesObject();
            auto managed_obj = realm.write([&]() {
                return realm.add(std::move(obj));
            });
            auto res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_str_col == std::nullopt;
            });
            CHECK(res.size() == 1);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_str_col != std::nullopt;
            });
            CHECK(res.size() == 0);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.mixed_col == std::nullopt;
            });
            CHECK(res.size() == 1);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.mixed_col != std::nullopt;
            });
            CHECK(res.size() == 0);
        }

        SECTION("link column") {
            auto realm = db(std::move(config));

            auto obj = AllTypesObject();
            auto obj_link = AllTypesObjectLink();
            obj_link.str_col = "foo";
            auto obj_link2 = StringObject();
            obj_link2.str_col = "bar";

            obj.opt_obj_col = &obj_link;
            obj_link.str_link_col = &obj_link2;

            auto managed_obj = realm.write([&]() {
                return realm.add(std::move(obj));
            });

            auto res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_obj_col->str_col == "foo";
            });
            CHECK(res.size() == 1);
            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_obj_col->str_col == "bar";
            });
            CHECK(res.size() == 0);
            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_obj_col->str_col != "bar";
            });
            CHECK(res.size() == 1);
            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_obj_col->str_link_col->str_col == "bar";
            });
            CHECK(res.size() == 1);
            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_obj_col->str_link_col->str_col != "bar";
            });
            CHECK(res.size() == 0);
        }
    }
}