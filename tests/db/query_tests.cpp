#include "../main.hpp"
#include "test_objects.hpp"

namespace realm {

#define query_results_size(Cls, fn)  \
    realm.template objects<Cls>().where(fn).size()

#define query_string_results_size(Cls, str, args)  \
    realm.template objects<Cls>().where(str, args).size()

    TEST_CASE("query") {
        realm_path path;
        realm::db_config config;
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
            CHECK(query_string_results_size(AllTypesObject, "_id == $0", {(int64_t)123}) == 1);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id != 123; }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "_id != $0", {(int64_t)123}) == 0);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.str_col == "foo bar"; }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "str_col == $0", {std::string("foo bar")}) == 1);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.str_col != "foo bar"; }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "str_col != $0", {std::string("foo bar")}) == 0);

            CHECK(query_results_size(AllTypesObject, [](auto &o) {
                      return o.binary_col == std::vector<uint8_t>({0, 1, 2, 3});
                  }) == 1);

            CHECK(query_string_results_size(AllTypesObject, "binary_col == $0", {std::vector<uint8_t>({0, 1, 2, 3})}) == 1);

            CHECK(query_results_size(AllTypesObject,
                                     [](auto &o) { return o.binary_col != std::vector<uint8_t>({0, 1, 2, 3}); }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "binary_col != $0", {std::vector<uint8_t>({0, 1, 2, 3})}) == 0);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col == AllTypesObject::Enum::two; }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "enum_col == $0", {static_cast<int64_t>(AllTypesObject::Enum::two)}) == 1);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col != AllTypesObject::Enum::two; }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "enum_col != $0", {static_cast<int64_t>(AllTypesObject::Enum::two)}) == 0);

            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col == std::chrono::system_clock::from_time_t(date);
                  }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "date_col == $0", {std::chrono::system_clock::from_time_t(date)}) == 1);

            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col != std::chrono::system_clock::from_time_t(date);
                  }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "date_col != $0", {std::chrono::system_clock::from_time_t(date)}) == 0);

            CHECK(query_results_size(AllTypesObject, [](auto &o) {
                      return o.uuid_col == realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002");
                  }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "uuid_col == $0", {realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002")}) == 1);

            CHECK(query_results_size(AllTypesObject, [](auto &o) {
                      return o.uuid_col != realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002");
                  }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "uuid_col != $0", {realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002")}) == 0);

            auto objs = realm.objects<AllTypesObject>().where([](auto& o) { return o.opt_obj_col->str_col == "foo"; }).size();
            CHECK(objs == 1);
            objs = realm.objects<AllTypesObject>().where("opt_obj_col.str_col == $0", {std::string("foo")}).size();
            CHECK(objs == 1);
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
            CHECK(query_string_results_size(AllTypesObject, "_id > $0", {(int64_t)123}) == 0);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id >= 123; }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "_id >= $0", {(int64_t) 123}) == 1);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id < 123; }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "_id < $0", {(int64_t) 123}) == 0);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id <= 123; }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "_id <= $0", {(int64_t) 123}) == 1);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col > AllTypesObject::Enum::two; }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "enum_col > $0", {static_cast<int64_t>(AllTypesObject::Enum::two)}) == 0);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col >= AllTypesObject::Enum::two; }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "enum_col >= $0", {static_cast<int64_t>(AllTypesObject::Enum::two)}) == 1);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col < AllTypesObject::Enum::two; }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "enum_col < $0", {static_cast<int64_t>(AllTypesObject::Enum::two)}) == 0);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col <= AllTypesObject::Enum::two; }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "enum_col <= $0", {static_cast<int64_t>(AllTypesObject::Enum::two)}) == 1);

            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col > std::chrono::system_clock::from_time_t(date);
                  }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "date_col > $0", {std::chrono::system_clock::from_time_t(date)}) == 0);

            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col >= std::chrono::system_clock::from_time_t(date);
                  }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "date_col >= $0", {std::chrono::system_clock::from_time_t(date)}) == 1);

            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col < std::chrono::system_clock::from_time_t(date);
                  }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "date_col < $0", {std::chrono::system_clock::from_time_t(date)}) == 0);

            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col <= std::chrono::system_clock::from_time_t(date);
                  }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "date_col <= $0", {std::chrono::system_clock::from_time_t(date)}) == 1);
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
            CHECK(query_string_results_size(AllTypesObject, "_id == $0 && str_col != $1", std::vector<realm::mixed>({(int64_t)123, std::string("John")})) == 0);

            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return o._id == 123 || o.str_col != "John";
            });
            CHECK(results.size() == 1);
            CHECK(query_string_results_size(AllTypesObject, "_id == $0 || str_col != $1", std::vector<realm::mixed>({(int64_t)123, std::string("John")})) == 1);

            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return o._id == 123 && o.str_col == "John";
            });
            CHECK(results.size() == 1);
            CHECK(query_string_results_size(AllTypesObject, "_id == $0 && str_col == $1", std::vector<realm::mixed>({(int64_t)123, std::string("John")})) == 1);

            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return o._id == 123 && o.str_col == "John" || o.bool_col == true;
            });
            CHECK(results.size() == 1);
            CHECK(query_string_results_size(AllTypesObject, "_id == $0 && str_col == $1  || bool_col == $2", std::vector<realm::mixed>({(int64_t)123, std::string("John"), true})) == 1);

            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return o._id == 123 && o.str_col.contains("Jo") || o.bool_col == true;
            });
            CHECK(results.size() == 1);
            CHECK(query_string_results_size(AllTypesObject, "_id == $0 && str_col CONTAINS $1  || bool_col == $2", std::vector<realm::mixed>({(int64_t)123, std::string("John"), true})) == 1);

            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return o.str_col.empty();
            });
            CHECK(results.size() == 0);
            CHECK(query_string_results_size(AllTypesObject, "str_col == ''", {}) == 0);

            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return !o.str_col.empty();
            });
            CHECK(results.size() == 1);
            CHECK(query_string_results_size(AllTypesObject, "NOT str_col == ''", {}) == 1);
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
            CHECK(query_string_results_size(AllTypesObject, "opt_str_col == NULL", {}) == 1);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_str_col != std::nullopt;
            });
            CHECK(res.size() == 0);
            CHECK(query_string_results_size(AllTypesObject, "opt_str_col != NULL", {}) == 0);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.mixed_col == std::nullopt;
            });
            CHECK(res.size() == 1);
            CHECK(query_string_results_size(AllTypesObject, "mixed_col == NULL", {}) == 1);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.mixed_col != std::nullopt;
            });
            CHECK(res.size() == 0);
            CHECK(query_string_results_size(AllTypesObject, "mixed_col != NULL", {}) == 0);
        }

        SECTION("link column") {
            auto realm = db(std::move(config));

            auto create_obj = [&](int64_t pk) {
                auto obj = AllTypesObject();
                obj._id = pk;
                obj.str_col = "root obj";
                auto obj_link = AllTypesObjectLink();
                obj_link._id = pk;
                obj_link.str_col = "foo";
                auto obj_link2 = StringObject();
                obj_link2._id = pk;
                obj_link2.str_col = "bar";

                obj.opt_obj_col = &obj_link;
                obj_link.str_link_col = &obj_link2;

                return realm.write([&]() {
                    return realm.add(std::move(obj));
                });
            };

            auto managed_obj = create_obj(0);
            auto managed_obj2 = create_obj(1);
            auto managed_obj3 = create_obj(2);

            auto res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_obj_col->str_col == "foo";
            });
            CHECK(res.size() == 3);
            CHECK(query_string_results_size(AllTypesObject, "opt_obj_col.str_col == $0", {std::string("foo")}) == 3);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_obj_col->str_col == "bar";
            });
            CHECK(res.size() == 0);
            CHECK(query_string_results_size(AllTypesObject, "opt_obj_col.str_col == $0", {std::string("bar")}) == 0);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_obj_col->str_col != "bar";
            });
            CHECK(res.size() == 3);
            CHECK(query_string_results_size(AllTypesObject, "str_col != $0", {std::string("bar")}) == 3);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_obj_col->str_link_col->str_col == "bar";
            });
            CHECK(res.size() == 3);
            CHECK(query_string_results_size(AllTypesObject, "opt_obj_col.str_link_col.str_col == $0", {std::string("bar")}) == 3);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_obj_col->str_link_col->str_col != "bar";
            });
            CHECK(res.size() == 0);
            CHECK(query_string_results_size(AllTypesObject, "opt_obj_col.str_link_col.str_col != $0", {std::string("bar")}) == 0);

            res = realm.objects<AllTypesObject>().where([&](auto &obj) {
                return obj.opt_obj_col == managed_obj.opt_obj_col;
            });
            CHECK(res.size() == 1);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_obj_col == std::nullptr_t();
            });
            CHECK(res.size() == 0);

            res = realm.objects<AllTypesObject>().where([&](auto &obj) {
                return obj.opt_obj_col != managed_obj.opt_obj_col;
            });
            CHECK(res.size() == 2);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_obj_col != std::nullptr_t();
            });
            CHECK(res.size() == 3);
        }

        SECTION("TRUEPREDICATE_FALSEPREDICATE") {
            auto realm = db(std::move(config));

            auto create_obj = [&](int64_t pk) {
                auto obj = AllTypesObject();
                obj._id = pk;
                obj.str_col = "root obj";
                auto obj_link = AllTypesObjectLink();
                obj_link._id = pk;
                obj_link.str_col = "foo";
                auto obj_link2 = StringObject();
                obj_link2._id = pk;
                obj_link2.str_col = "bar";

                obj.opt_obj_col = &obj_link;
                obj_link.str_link_col = &obj_link2;

                return realm.write([&]() {
                    return realm.add(std::move(obj));
                });
            };

            auto managed_obj = create_obj(0);
            auto managed_obj2 = create_obj(1);
            auto managed_obj3 = create_obj(2);

            auto res = realm.objects<AllTypesObject>().where([](auto& o) {
                return truepredicate(o);
            });
            CHECK(res.size() == 3);

            res = realm.objects<AllTypesObject>().where([](auto& o) {
                return falsepredicate(o);
            });
            CHECK(res.size() == 0);
        }

        SECTION("sub results") {
            auto realm = db(std::move(config));

            auto create_obj = [&](int64_t pk) {
                auto obj = AllTypesObject();
                obj._id = pk;
                obj.str_col = "root obj";
                auto obj_link = AllTypesObjectLink();
                obj_link._id = pk;
                obj_link.str_col = "foo";
                auto obj_link2 = StringObject();
                obj_link2._id = pk;
                obj_link2.str_col = "bar";

                obj.opt_obj_col = &obj_link;
                obj_link.str_link_col = &obj_link2;

                return realm.write([&]() {
                    return realm.add(std::move(obj));
                });
            };

            auto managed_obj = create_obj(0);
            auto managed_obj2 = create_obj(1);
            auto managed_obj3 = create_obj(2);

            auto res = realm.objects<AllTypesObject>().where([](auto& o) {
                return o._id > 0;
            });
            CHECK(res.size() == 2);

            res = res.where([](auto& o) {
                return o._id > 1;
            });
            CHECK(res.size() == 1);

            res = res.where([](auto& o) {
                return truepredicate(o);
            });
            CHECK(res.size() == 3);
        }

        SECTION("map") {
            auto realm = db(std::move(config));
            auto date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            auto obj_id = realm::object_id::generate();

            auto create_obj = [&](int64_t pk) {
                auto obj = AllTypesObject();
                obj._id = pk;
                obj.str_col = "root obj";
                auto obj_link = AllTypesObjectLink();
                obj_link._id = pk;
                obj_link.str_col = "foo";
                auto obj_link2 = StringObject();
                obj_link2._id = pk;
                obj_link2.str_col = "bar";
                obj_link.str_link_col = &obj_link2;

                auto embedded_obj = AllTypesObjectEmbedded();
                embedded_obj.str_col = "foo_embedded";

                obj.map_int_col["one"] = 1;
                obj.map_int_col["two"] = 2;

                obj.map_bool_col["is_true"] = true;

                obj.map_double_col["my_double"] = 1.234;
                obj.map_double_col["my_double2"] = 2.234;

                obj.map_str_col["foo"] = "bar";
                obj.map_str_col["name"] = "Fido";

                obj.map_uuid_col["my_uuid"] = realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002");
                obj.map_object_id_col["my_object_id"] = obj_id;

                obj.map_decimal_col["my_decimal"] = realm::decimal128(1.234);
                obj.map_decimal_col["my_decimal2"] = realm::decimal128(2.234);

                obj.map_binary_col["my_binary"] = std::vector<uint8_t>({0,0,0,0,1,1,1,1});

                obj.map_date_col["my_date"] = std::chrono::system_clock::from_time_t(date);
                obj.map_date_col["my_date2"] = std::chrono::system_clock::from_time_t(date + 10);

                obj.map_enum_col["my_enum"] = AllTypesObject::Enum::two;

                obj.map_mixed_col["my_mixed"] = realm::mixed("foo_value");
                obj.map_mixed_col["my_mixed_numeric"] = realm::mixed((int64_t)1);
                obj.map_mixed_col["my_mixed_numeric2"] = realm::mixed((int64_t)2);

                obj.map_link_col["my_link"] = &obj_link;
                obj.map_embedded_col["my_embedded_link"] = &embedded_obj;

                return realm.write([&]() {
                    return realm.add(std::move(obj));
                });
            };

            auto managed_obj = create_obj(0);
            auto managed_obj2 = create_obj(1);
            auto managed_obj3 = create_obj(2);

            auto do_query = [&](std::function<rbool(realm::managed<AllTypesObject>&)>&& fn) -> size_t {
                auto res = realm.objects<AllTypesObject>().where([fn = std::move(fn)](realm::managed<AllTypesObject>& o) {
                    return fn(o);
                });
                return res.size();
            };

            // Int
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_int_col["one"] == 1; }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_int_col["one"] == 2; }) == 0);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_int_col["one"] != 2; }) == 3);

            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_int_col["two"] > 1; }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_int_col["two"] >= 1; }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_int_col["two"] < 2; }) == 0);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_int_col["two"] <= 2; }) == 3);

            // Bool

            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_bool_col["is_true"] == true; }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_bool_col["is_true"] == false; }) == 0);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_bool_col["is_true"] != false; }) == 3);

            // Double
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_double_col["my_double"] == 1.234; }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_double_col["my_double"] == 2.234; }) == 0);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_double_col["my_double"] != 2.234; }) == 3);

            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_double_col["my_double"] > 1; }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_double_col["my_double"] >= 1; }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_double_col["my_double2"] < 2; }) == 0);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_double_col["my_double2"] <= 2.234; }) == 3);

            // String
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_str_col["name"] == "Fido"; }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_str_col["name"] == "Bill"; }) == 0);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_str_col["name"] != "NA"; }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_str_col["name"].contains("ido"); }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_str_col["name"].contains("NA"); }) == 0);

            // UUID
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_uuid_col["my_uuid"] == realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002"); }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_uuid_col["my_uuid"] == realm::uuid("20de7916-7f84-11ec-a8a3-0242ac120002"); }) == 0);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_uuid_col["my_uuid"] != realm::uuid("20de7916-7f84-11ec-a8a3-0242ac120002"); }) == 3);

            // Object ID
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_object_id_col["my_object_id"] == obj_id; }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_object_id_col["my_object_id"] == realm::object_id::generate(); }) == 0);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_object_id_col["my_object_id"] != realm::object_id::generate(); }) == 3);

            // Decimal
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_decimal_col["my_decimal"] == realm::decimal128(1.234); }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_decimal_col["my_decimal"] == realm::decimal128(2.234); }) == 0);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_decimal_col["my_decimal"] != realm::decimal128(2.234); }) == 3);

            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_decimal_col["my_decimal"] > 1; }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_decimal_col["my_decimal"] >= 1; }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_decimal_col["my_decimal2"] < 2; }) == 0);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_decimal_col["my_decimal2"] <= 2.234; }) == 3);

            // Binary
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_binary_col["my_binary"] == std::vector<uint8_t>({0,0,0,0,1,1,1,1}); }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_binary_col["my_binary"] == std::vector<uint8_t>({1,0,0,0,1,1,1,1}); }) == 0);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_binary_col["my_binary"] != std::vector<uint8_t>({1,0,0,0,1,1,1,1}); }) == 3);

            // Date
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_date_col["my_date"] == std::chrono::system_clock::from_time_t(date); }) == 3);
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_date_col["my_date"] == std::chrono::system_clock::from_time_t(date + 10); }) == 0);
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_date_col["my_date"] != std::chrono::system_clock::from_time_t(date + 10); }) == 3);

            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_date_col["my_date"] > std::chrono::system_clock::from_time_t(date - 1); }) == 3);
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_date_col["my_date"] >= std::chrono::system_clock::from_time_t(date); }) == 3);
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_date_col["my_date2"] < std::chrono::system_clock::from_time_t(date + 10); }) == 0);
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_date_col["my_date2"] <= std::chrono::system_clock::from_time_t(date + 20); }) == 3);

            // Enum
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_enum_col["my_enum"] == AllTypesObject::Enum::two; }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_enum_col["my_enum"] == AllTypesObject::Enum::one; }) == 0);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_enum_col["my_enum"] != AllTypesObject::Enum::one; }) == 3);

            // Mixed
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_mixed_col["my_mixed"] == realm::mixed("foo_value"); }) == 3);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_mixed_col["my_mixed"] == realm::mixed("bar_value"); }) == 0);
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_mixed_col["my_mixed"] != realm::mixed("bar_value"); }) == 3);

            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_mixed_col["my_mixed_numeric"] > (int64_t)1; }) == 0);
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_mixed_col["my_mixed_numeric"] >= (int64_t)1; }) == 3);
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_mixed_col["my_mixed_numeric2"] < (int64_t)2; }) == 0);
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_mixed_col["my_mixed_numeric2"] <= (int64_t)2; }) == 3);

            // Link
            auto links = realm.objects<AllTypesObjectLink>().where([](auto& o) { return o._id == 0; });
            CHECK(links.size() == 1);
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_link_col["my_link"] == links[0]; }) == 1);
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_link_col["my_link"] != links[0]; }) == 2);

            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_link_col["my_link"]->str_col == "foo"; }) == 3);
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_link_col["my_link"]->str_col == "bar"; }) == 0);
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_link_col["my_link"]->str_col != "bar"; }) == 3);

            // RHS is null
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_link_col["my_link"] == managed_obj.opt_obj_col; }) == 0);
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_link_col["my_link"] != managed_obj.opt_obj_col; }) == 3);

            auto managed_link = realm.write([&]() {
                AllTypesObjectLink link;
                link._id = 1234;
                return realm.add(std::move(link));
            });
            realm.write([&]() {
                managed_obj.map_link_col["my_link"] = managed_link;
            });
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_link_col["my_link"] == managed_link; }) == 1);
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_link_col["my_link"] == managed_link; }) != 2);

            // Embedded
            CHECK(do_query([&](realm::managed<AllTypesObject>& o) -> rbool { return o.map_embedded_col["my_embedded_link"]->str_col == "foo_embedded"; }) == 3);

            // Test non existent key
            CHECK(do_query([](realm::managed<AllTypesObject>& o) -> rbool { return o.map_int_col["NA"] == 1; }) == 0);
        }
    }
}
