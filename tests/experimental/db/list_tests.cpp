#include "../../main.hpp"
#include "test_objects.hpp"

using namespace realm;

template<typename Col, typename Value>
void test_list(Col& list, std::vector<Value> values, experimental::db& realm) {
    realm.write([&] {
        list.push_back(values[0]);
        list.push_back(values[1]);
    });

    // ensure values exist
    CHECK(list.size() == 2);

    realm.write([&] {
        list.push_back(values[0]);
    });
    CHECK(list.size() == 3);
    CHECK(list.find(values[1]) == 1);
    CHECK(list[2] == values[0]);

    realm.write([&list] {
        list.pop_back();
    });
    CHECK(list.size() == 2);

    realm.write([&list] {
        list.erase(0);
    });
    CHECK(list.size() == 1);

    realm.write([&list] {
        list.clear();
    });
    CHECK(list.size() == 0);
}

TEST_CASE("list", "[list]") {
    realm_path path;
    db_config config;
    config.set_path(path);
    SECTION("list_tests") {
        auto realm = realm::experimental::db(std::move(config));
        auto obj = realm::experimental::AllTypesObject();
        obj.list_int_col.push_back(42);
        CHECK(obj.list_int_col[0] == 42);

        realm::experimental::AllTypesObjectLink o1;
        o1.str_col = "Fido";
        obj.list_obj_col.push_back(&o1);
        CHECK(obj.list_obj_col[0]->str_col == "Fido");
        CHECK(obj.list_int_col.size() == 1);
        for (auto &i: obj.list_int_col) {
            CHECK(i == 42);
        }

        realm::experimental::AllTypesObjectEmbedded o2;
        o2.str_col = "Fido";
        obj.list_embedded_obj_col.push_back(&o2);
        CHECK(obj.list_embedded_obj_col[0]->str_col == "Fido");
        CHECK(obj.list_embedded_obj_col.size() == 1);

        auto managed_obj = realm.write([&realm, &obj]() {
            return realm.add(std::move(obj));
        });

        CHECK(managed_obj.list_int_col[0] == 42);
        CHECK(managed_obj.list_obj_col[0]->str_col == "Fido");
        CHECK(managed_obj.list_embedded_obj_col[0]->str_col == "Fido");

        realm.write([&managed_obj]() {
            managed_obj.list_int_col.push_back(84);
            realm::experimental::AllTypesObjectLink o3;
            o3._id = 1;
            o3.str_col = "Rex";
            managed_obj.list_obj_col.push_back(&o3);
            realm::experimental::AllTypesObjectEmbedded e;
            e.str_col = "Rex";
            managed_obj.list_embedded_obj_col.push_back(&e);
        });
        size_t idx = 0;
        for (auto i : managed_obj.list_int_col) {
            CHECK(i == managed_obj.list_int_col[idx]);
            ++idx;
        }
        CHECK(managed_obj.list_int_col.size() == 2);
        CHECK(managed_obj.list_int_col[0] == 42);
        CHECK(managed_obj.list_int_col[1] == 84);
        CHECK(managed_obj.list_obj_col[0]->str_col == "Fido");
        CHECK(managed_obj.list_obj_col[1]->str_col == "Rex");
        CHECK(managed_obj.list_embedded_obj_col[0]->str_col == "Fido");
        CHECK(managed_obj.list_embedded_obj_col[1]->str_col == "Rex");
    }

    SECTION("list_insert_remove_primitive") {
        auto obj = realm::experimental::AllTypesObject();

        // unmanaged
        obj.list_int_col.push_back(1);
        obj.list_int_col.push_back(2);
        obj.list_int_col.push_back(3);
        CHECK(obj.list_int_col.size() == 3);

        obj.list_int_col.pop_back();
        CHECK(obj.list_int_col.size() == 2);
        obj.list_int_col.clear();
        CHECK(obj.list_int_col.size() == 0);
        obj.list_int_col.push_back(2);
        obj.list_int_col.push_back(4);
        CHECK(obj.list_int_col[1] == 4);

        auto realm = realm::experimental::db(std::move(config));
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });

        // ensure values exist
        CHECK(managed_obj.list_int_col.size() == 2);

        CHECK_THROWS(managed_obj.list_int_col.push_back(1));

        realm.write([&realm, &managed_obj] {
            managed_obj.list_int_col.push_back(1);
        });
        CHECK(managed_obj.list_int_col.size() == 3);
        CHECK(managed_obj.list_int_col.find(1) == 2);
        CHECK(managed_obj.list_int_col[2] == 1);

        realm.write([&realm, &managed_obj] {
            managed_obj.list_int_col.pop_back();
        });
        CHECK(managed_obj.list_int_col.size() == 2);
        CHECK(managed_obj.list_int_col.find(1) == realm::npos);

        realm.write([&realm, &managed_obj] {
            managed_obj.list_int_col.erase(0);
        });
        CHECK(managed_obj.list_int_col.size() == 1);

        realm.write([&managed_obj] {
            managed_obj.list_int_col.clear();
        });
        CHECK(managed_obj.list_int_col.size() == 0);
    }

    SECTION("list_insert_remove_object") {
        auto obj = realm::experimental::AllTypesObject();

        auto o1 = realm::experimental::AllTypesObjectLink();
        o1._id = 1;
        o1.str_col = "foo";
        auto o2 = realm::experimental::AllTypesObjectLink();
        o2._id = 2;
        o2.str_col = "bar";
        auto o3 = realm::experimental::AllTypesObjectLink();
        o3._id = 3;
        o3.str_col = "baz";
        auto o4 = realm::experimental::AllTypesObjectLink();
        o4._id = 4;
        o4.str_col = "foo baz";
        auto o5 = realm::experimental::AllTypesObjectLink();
        o5._id = 5;
        o5.str_col = "foo bar";
        auto o6 = realm::experimental::AllTypesObjectLink();

        // unmanaged
        obj.list_obj_col.push_back(&o1);
        obj.list_obj_col.push_back(&o2);
        obj.list_obj_col.push_back(&o3);
        CHECK(obj.list_obj_col.size() == 3);

        obj.list_obj_col.pop_back();
        CHECK(obj.list_obj_col.size() == 2);
        obj.list_obj_col.clear();
        CHECK(obj.list_obj_col.size() == 0);
        obj.list_obj_col.push_back(&o1);
        obj.list_obj_col.push_back(&o2);
        obj.list_obj_col.push_back(&o3);
        obj.list_obj_col.push_back(&o4);

        CHECK(obj.list_obj_col[3]->str_col == o4.str_col);

        auto realm = realm::experimental::db(std::move(config));
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });

        // ensure values exist
        CHECK(managed_obj.list_obj_col.size() == 4);

        CHECK_THROWS(managed_obj.list_obj_col.push_back(&o5));

        realm.write([&managed_obj, &o5] {
            managed_obj.list_obj_col.push_back(&o5);
        });

        CHECK(managed_obj.list_obj_col.size() == 5);
        auto o = managed_obj.list_obj_col[4];
        CHECK(managed_obj.list_obj_col.find(o) == 4);
        CHECK(managed_obj.list_obj_col[4] == o);

        realm.write([&managed_obj] {
            managed_obj.list_obj_col.pop_back();
        });
        CHECK(managed_obj.list_obj_col.size() == 4);
        auto managed_o6 = realm.write([&realm, &o6] {
            return realm.add(std::move(o6));
        });
        CHECK(managed_obj.list_obj_col.find(managed_o6) == realm::npos);

        realm.write([&realm, &managed_obj] {
            managed_obj.list_obj_col.erase(0);
        });
        CHECK(managed_obj.list_obj_col.size() == 3);

        realm.write([&realm, &managed_obj] {
            managed_obj.list_obj_col.clear();
        });
        CHECK(managed_obj.list_obj_col.size() == 0);
    }

    SECTION("list_insert_remove_embedded_object") {
        auto obj = realm::experimental::AllTypesObject();

        auto o1 = realm::experimental::AllTypesObjectEmbedded();
        o1.str_col = "foo";
        auto o2 = realm::experimental::AllTypesObjectEmbedded();
        o2.str_col = "bar";
        auto o3 = realm::experimental::AllTypesObjectEmbedded();
        o3.str_col = "baz";
        auto o4 = realm::experimental::AllTypesObjectEmbedded();
        o4.str_col = "foo baz";
        auto o5 = realm::experimental::AllTypesObjectEmbedded();
        o5.str_col = "foo bar";

        // unmanaged
        obj.list_embedded_obj_col.push_back(&o1);
        obj.list_embedded_obj_col.push_back(&o2);
        obj.list_embedded_obj_col.push_back(&o3);
        CHECK(obj.list_embedded_obj_col.size() == 3);

        obj.list_embedded_obj_col.pop_back();
        CHECK(obj.list_embedded_obj_col.size() == 2);
        obj.list_embedded_obj_col.clear();
        CHECK(obj.list_embedded_obj_col.size() == 0);
        obj.list_embedded_obj_col.push_back(&o1);
        obj.list_embedded_obj_col.push_back(&o2);
        obj.list_embedded_obj_col.push_back(&o3);
        obj.list_embedded_obj_col.push_back(&o4);

        auto realm = realm::experimental::db(std::move(config));
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });

        // ensure values exist
        CHECK(managed_obj.list_embedded_obj_col.size() == 4);

        CHECK_THROWS(managed_obj.list_embedded_obj_col.push_back(&o5));

        realm.write([&managed_obj, &o5] {
            managed_obj.list_embedded_obj_col.push_back(&o5);
        });

        auto managed_o5 = managed_obj.list_embedded_obj_col[4];

        CHECK(managed_obj.list_embedded_obj_col.size() == 5);
        CHECK(managed_obj.list_embedded_obj_col.find(managed_o5) == 4);
        CHECK(managed_obj.list_embedded_obj_col[4] == managed_o5);

        realm.write([&managed_obj] {
            managed_obj.list_embedded_obj_col.pop_back();
        });
        CHECK(managed_obj.list_embedded_obj_col.size() == 4);
        CHECK(managed_obj.list_embedded_obj_col.find(managed_o5) == realm::npos);

        realm.write([&managed_obj] {
            managed_obj.list_embedded_obj_col.erase(0);
        });
        CHECK(managed_obj.list_embedded_obj_col.size() == 3);

        realm.write([&managed_obj] {
            managed_obj.list_embedded_obj_col.clear();
        });
        CHECK(managed_obj.list_embedded_obj_col.size() == 0);
    }

    SECTION("notifications_insertions", "[list]") {
        auto obj = realm::experimental::AllTypesObject();

        auto realm = realm::experimental::db(std::move(config));
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });

        int callback_count = 0;

        auto token = managed_obj.list_int_col.observe([&](auto&& c) {
            callback_count++;
            if (callback_count > 1)
                CHECK(c.insertions.size() == 1);
        });

        realm.write([&realm, &managed_obj] {
            managed_obj.list_int_col.push_back(456);
        });

        realm.refresh();


        realm.write([&realm, &managed_obj] {
            managed_obj.list_int_col.push_back(456);
        });

        realm.refresh();

        CHECK(callback_count == 3);
    }

    SECTION("notifications_deletions") {
        auto obj = realm::experimental::AllTypesObject();

        auto realm = realm::experimental::db(std::move(config));
        auto managed_obj = realm.write([&realm, &obj] {
            auto managed_obj = realm.add(std::move(obj));
            managed_obj.list_int_col.push_back(456);
            return managed_obj;
        });

        size_t deletions = 0;
        auto token = managed_obj.list_int_col.observe([&](auto&& c) {
            deletions = c.deletions.size();
        });

        realm.write([&realm, &managed_obj] {
            managed_obj.list_int_col.erase(0);
        });
        realm.refresh();
        CHECK(deletions == 1);
    }

    SECTION("notifications_modifications", "[list]") {
        auto obj = realm::experimental::AllTypesObject();

        auto realm = realm::experimental::db(std::move(config));
        auto managed_obj = realm.write([&realm, &obj] {
            auto o = realm.add(std::move(obj));
            o.list_int_col.push_back(123);
            o.list_int_col.push_back(456);
            return o;
        });

        std::vector<uint64_t> modifications;

        auto token = managed_obj.list_int_col.observe([&](auto c) {
            modifications = c.modifications;
        });

        realm.write([&realm, &managed_obj] {
            managed_obj.list_int_col.set(1, 345);
        });
        realm.refresh();

        CHECK(modifications.size() == 1);
        CHECK(modifications[0] == 1);
    }

    SECTION("list_all_primitive_types") {
        auto realm = realm::experimental::db(std::move(config));
        using Enum = realm::experimental::AllTypesObject::Enum;
        auto obj = realm.write([&]() {
            return realm.add(realm::experimental::AllTypesObject());
        });
        auto object_id1 = realm::object_id::generate();
        auto object_id2 = realm::object_id::generate();

        test_list(obj.list_int_col, std::vector<uint64_t>({1, 2}), realm);
        test_list(obj.list_bool_col, std::vector<uint8_t>({true, false}), realm);
        test_list(obj.list_double_col, std::vector<double>({1.23, 2.45}), realm);
        test_list(obj.list_str_col, std::vector<std::string>({"foo", "bar"}), realm);
        test_list(obj.list_enum_col, std::vector<Enum>({Enum::one, Enum::two}), realm);
        test_list(obj.list_uuid_col,
                  std::vector<realm::uuid>({realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120000"),
                                            realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002")}), realm);
        test_list(obj.list_binary_col, std::vector<std::vector<uint8_t>>({{0}, {1}}), realm);
        test_list(obj.list_object_id_col, std::vector<object_id>({object_id1, object_id2}), realm);
        auto date_list_obj = realm::experimental::AllTypesObject();
        auto date1 = std::chrono::system_clock::now();
        auto date2 = date1 + std::chrono::hours(24);
        test_list(obj.list_date_col,
                  std::vector<std::chrono::time_point<std::chrono::system_clock>>({date1, date2}), realm);
    }

    SECTION("list_mixed") {
        auto realm = realm::experimental::db(std::move(config));
        auto obj = realm::experimental::AllTypesObject();
        obj.list_mixed_col.push_back((int64_t)42);
        obj.list_mixed_col.push_back(true);
        obj.list_mixed_col.push_back(std::string("hello world"));
        obj.list_mixed_col.push_back(42.42);
        obj.list_mixed_col.push_back(std::vector<uint8_t>{0,1,2});
        auto ts = std::chrono::time_point<std::chrono::system_clock>();
        obj.list_mixed_col.push_back(ts);

        auto managed_obj = realm.write([&]() {
            return realm.add(std::move(obj));
        });

        CHECK(std::holds_alternative<int64_t>(managed_obj.list_mixed_col[0]));
        CHECK(std::get<int64_t>(managed_obj.list_mixed_col[0]) == 42);

        CHECK(std::holds_alternative<bool>(managed_obj.list_mixed_col[1]));
        CHECK(std::get<bool>(managed_obj.list_mixed_col[1]) == true);

        CHECK(std::holds_alternative<std::string>(managed_obj.list_mixed_col[2]));
        CHECK(std::get<std::string>(managed_obj.list_mixed_col[2]) == "hello world");

        CHECK(std::holds_alternative<double>(managed_obj.list_mixed_col[3]));
        CHECK(std::get<double>(managed_obj.list_mixed_col[3]) == 42.42);

        CHECK(std::holds_alternative<std::vector<uint8_t>>(managed_obj.list_mixed_col[4]));
        CHECK(std::get<std::vector<uint8_t>>(managed_obj.list_mixed_col[4]) == std::vector<uint8_t>{0,1,2});

        CHECK(std::holds_alternative<std::chrono::time_point<std::chrono::system_clock>>(managed_obj.list_mixed_col[5]));
        CHECK(std::get<std::chrono::time_point<std::chrono::system_clock>>(managed_obj.list_mixed_col[5]) == ts);

        realm.write([&managed_obj] {
            managed_obj.list_mixed_col.push_back(realm::uuid());
        });

        CHECK(std::holds_alternative<realm::uuid>(managed_obj.list_mixed_col[6]));
        CHECK(std::get<realm::uuid>(managed_obj.list_mixed_col[6]) == realm::uuid());
    }

    SECTION("list_value") {
        auto realm = realm::experimental::db(std::move(config));
        auto obj = realm::experimental::AllTypesObject();
        obj.list_int_col.push_back((int64_t)1);
        obj.list_int_col.push_back((int64_t)2);
        obj.list_int_col.push_back((int64_t)3);

        auto o1 = realm::experimental::AllTypesObjectLink();
        o1._id = 1;
        o1.str_col = "foo";
        auto o2 = realm::experimental::AllTypesObjectLink();
        o2._id = 2;
        o2.str_col = "bar";
        auto o3 = realm::experimental::AllTypesObjectLink();
        o3._id = 3;
        o3.str_col = "baz";
        auto o4 = realm::experimental::AllTypesObjectLink();
        o4._id = 4;
        o4.str_col = "foo baz";
        auto o5 = realm::experimental::AllTypesObjectLink();
        o5._id = 5;
        o5.str_col = "foo bar";
        auto o6 = realm::experimental::AllTypesObjectLink();

        // unmanaged
        obj.list_obj_col.push_back(&o1);
        obj.list_obj_col.push_back(&o2);
        obj.list_obj_col.push_back(&o3);

        auto managed_obj = realm.write([&]() {
            return realm.add(std::move(obj));
        });

        std::vector<int64_t> as_value = managed_obj.list_int_col.detach();
        CHECK(as_value == std::vector<int64_t>{1, 2, 3});
    }

    SECTION("iterator") {
        auto realm = realm::experimental::db(std::move(config));
        auto obj = realm::experimental::AllTypesObject();
        auto managed_obj = realm.write([&]() {
            return realm.add(std::move(obj));
        });
        realm.write([&]() {
            managed_obj.list_int_col.push_back(1);
            managed_obj.list_int_col.push_back(2);
            managed_obj.list_int_col.push_back(3);
        });
        CHECK(managed_obj.list_int_col.size() == 3);

        std::vector<int64_t> res;
        for (const auto& x : managed_obj.list_int_col) {
            res.push_back(x);
        }
        CHECK(res == std::vector<int64_t>({1, 2, 3}));
        res.clear();

        for (auto it = managed_obj.list_int_col.begin(); it != managed_obj.list_int_col.end(); ++it) {
            res.push_back(*it);
        }
        CHECK(res == std::vector<int64_t>({1, 2, 3}));
    }

    SECTION("iterator managed objects") {
        auto realm = realm::experimental::db(std::move(config));
        auto obj = realm::experimental::AllTypesObject();
        auto managed_obj = realm.write([&]() {
            return realm.add(std::move(obj));
        });

        experimental::AllTypesObjectLink link;
        link._id = 1;
        link.str_col = "foo";

        experimental::AllTypesObjectLink link2;
        link2._id = 2;
        link2.str_col = "bar";

        realm.write([&]() {
            managed_obj.list_obj_col.push_back(&link);
            managed_obj.list_obj_col.push_back(&link2);
        });

        std::set<int64_t> res;
        for (const auto& x : managed_obj.list_obj_col) {
            res.insert(x._id);
        }
        CHECK(res == std::set<int64_t>({1, 2}));
        res.clear();

        for (auto it = managed_obj.list_obj_col.begin(); it != managed_obj.list_obj_col.end(); ++it) {
            res.insert(it.operator*()._id);
        }
        CHECK(res == std::set<int64_t>({1, 2}));
    }

    SECTION("query") {
        auto realm = realm::experimental::db(std::move(config));
        auto obj = realm::experimental::AllTypesObject();
        auto managed_obj = realm.write([&]() {
            return realm.add(std::move(obj));
        });

        experimental::AllTypesObjectLink link;
        link._id = 1;
        link.str_col = "foo";

        experimental::AllTypesObjectLink link2;
        link2._id = 2;
        link2.str_col = "bar";

        auto object_id1 = realm::object_id("000123450000ffbeef91906a");
        auto object_id2 = realm::object_id("000123450000ffbeef91906b");
        auto object_id3 = realm::object_id("000123450000ffbeef91906c");

        auto date1 = std::chrono::system_clock::now();
        auto date2 = date1 + std::chrono::hours(24);
        auto date3 = date2 + std::chrono::hours(24);

        realm.write([&]() {
            managed_obj.list_obj_col.push_back(&link);
            managed_obj.list_obj_col.push_back(&link2);

            managed_obj.list_int_col.push_back(1);
            managed_obj.list_int_col.push_back(2);
            managed_obj.list_int_col.push_back(3);

            managed_obj.list_bool_col.push_back(true);
            managed_obj.list_bool_col.push_back(false);
            managed_obj.list_bool_col.push_back(false);

            managed_obj.list_double_col.push_back(1.23);
            managed_obj.list_double_col.push_back(2.34);
            managed_obj.list_double_col.push_back(4.56);

            managed_obj.list_str_col.push_back("apple");
            managed_obj.list_str_col.push_back("banana");
            managed_obj.list_str_col.push_back("coconut");

            managed_obj.list_uuid_col.push_back(realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120000"));
            managed_obj.list_uuid_col.push_back(realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120001"));
            managed_obj.list_uuid_col.push_back(realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002"));

            managed_obj.list_object_id_col.push_back(object_id1);
            managed_obj.list_object_id_col.push_back(object_id2);
            managed_obj.list_object_id_col.push_back(object_id3);

            managed_obj.list_decimal_col.push_back(decimal128(1.23));
            managed_obj.list_decimal_col.push_back(decimal128(2.34));
            managed_obj.list_decimal_col.push_back(decimal128(3.45));

            managed_obj.list_binary_col.push_back({0,0,0,0});
            managed_obj.list_binary_col.push_back({0,0,0,1});
            managed_obj.list_binary_col.push_back({0,0,0,2});

            managed_obj.list_date_col.push_back(date1);
            managed_obj.list_date_col.push_back(date2);
            managed_obj.list_date_col.push_back(date3);

            managed_obj.list_mixed_col.push_back(realm::mixed(1));
            managed_obj.list_mixed_col.push_back(realm::mixed("foo"));
            managed_obj.list_mixed_col.push_back(realm::mixed(date1));

            managed_obj.list_enum_col.push_back(experimental::AllTypesObject::Enum::one);
            managed_obj.list_enum_col.push_back(experimental::AllTypesObject::Enum::two);

            realm::experimental::AllTypesObjectEmbedded embedded1;
            embedded1.str_col = "apple";
            realm::experimental::AllTypesObjectEmbedded embedded2;
            embedded2.str_col = "banana";

            managed_obj.list_embedded_obj_col.push_back(&embedded1);
            managed_obj.list_embedded_obj_col.push_back(&embedded2);
        });

        auto object_results = managed_obj.list_obj_col.where([](auto & o) {
            return o._id == 1;
        });
        CHECK(object_results[0]._id == 1);
        CHECK(object_results.size() == 1);

        auto object_results2 = managed_obj.list_obj_col.where("_id == $0", {1});
        CHECK(object_results2[0]._id == 1);

        auto sorted_object_results1 = managed_obj.list_obj_col.sort("str_col", true);
        auto sorted_object_results2 = managed_obj.list_obj_col.sort({{"str_col", false}});
        auto sorted_object_results_embedded1 = managed_obj.list_embedded_obj_col.sort("str_col", true);
        auto sorted_object_results_embedded2 = managed_obj.list_embedded_obj_col.sort({{"str_col", false}});

        CHECK(sorted_object_results1.size() == 2);
        CHECK(sorted_object_results1[0].str_col == "bar");
        CHECK(sorted_object_results1[1].str_col == "foo");
        CHECK(sorted_object_results2.size() == 2);
        CHECK(sorted_object_results2[0].str_col == "foo");
        CHECK(sorted_object_results2[1].str_col == "bar");

        CHECK(sorted_object_results_embedded1.size() == 2);
        CHECK(sorted_object_results_embedded1[0].str_col == "apple");
        CHECK(sorted_object_results_embedded1[1].str_col == "banana");
        CHECK(sorted_object_results_embedded2.size() == 2);
        CHECK(sorted_object_results_embedded2[0].str_col == "banana");
        CHECK(sorted_object_results_embedded2[1].str_col == "apple");

        auto sorted_object_results3 = managed_obj.list_int_col.sort(true);
        CHECK(sorted_object_results3[0] == 1);
        CHECK(sorted_object_results3[1] == 2);
        CHECK(sorted_object_results3[2] == 3);

        auto sorted_object_results4 = managed_obj.list_int_col.sort(false);
        CHECK(sorted_object_results4[0] == 3);
        CHECK(sorted_object_results4[1] == 2);
        CHECK(sorted_object_results4[2] == 1);

        auto sorted_object_results5 = managed_obj.list_bool_col.sort(true);
        CHECK(sorted_object_results5[0] == false);
        CHECK(sorted_object_results5[1] == false);
        CHECK(sorted_object_results5[2] == true);

        auto sorted_object_results6 = managed_obj.list_bool_col.sort(false);
        CHECK(sorted_object_results6[0] == true);
        CHECK(sorted_object_results6[1] == false);
        CHECK(sorted_object_results6[2] == false);

        auto sorted_object_results7 = managed_obj.list_double_col.sort(true);
        CHECK(sorted_object_results7[0] == 1.23);
        CHECK(sorted_object_results7[1] == 2.34);
        CHECK(sorted_object_results7[2] == 4.56);

        auto sorted_object_results8 = managed_obj.list_double_col.sort(false);
        CHECK(sorted_object_results8[0] == 4.56);
        CHECK(sorted_object_results8[1] == 2.34);
        CHECK(sorted_object_results8[2] == 1.23);

        auto sorted_object_results9 = managed_obj.list_str_col.sort(true);
        CHECK(sorted_object_results9[0] == "apple");
        CHECK(sorted_object_results9[1] == "banana");
        CHECK(sorted_object_results9[2] == "coconut");

        auto sorted_object_results10 = managed_obj.list_str_col.sort(false);
        CHECK(sorted_object_results10[0] == "coconut");
        CHECK(sorted_object_results10[1] == "banana");
        CHECK(sorted_object_results10[2] == "apple");

        auto sorted_object_results11 = managed_obj.list_uuid_col.sort(true);
        CHECK(sorted_object_results11[0] == realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120000"));
        CHECK(sorted_object_results11[1] == realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120001"));
        CHECK(sorted_object_results11[2] == realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002"));

        auto sorted_object_results12 = managed_obj.list_uuid_col.sort(false);
        CHECK(sorted_object_results12[0] == realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002"));
        CHECK(sorted_object_results12[1] == realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120001"));
        CHECK(sorted_object_results12[2] == realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120000"));

        auto sorted_object_results13 = managed_obj.list_object_id_col.sort(true);
        CHECK(sorted_object_results13[0] == object_id1);
        CHECK(sorted_object_results13[1] == object_id2);
        CHECK(sorted_object_results13[2] == object_id3);

        auto sorted_object_results14 = managed_obj.list_object_id_col.sort(false);
        CHECK(sorted_object_results14[0] == object_id3);
        CHECK(sorted_object_results14[1] == object_id2);
        CHECK(sorted_object_results14[2] == object_id1);

        auto sorted_object_results15 = managed_obj.list_decimal_col.sort(true);
        CHECK(sorted_object_results15[0] == realm::decimal128(1.23));
        CHECK(sorted_object_results15[1] == realm::decimal128(2.34));
        CHECK(sorted_object_results15[2] == realm::decimal128(3.45));

        auto sorted_object_results16 = managed_obj.list_decimal_col.sort(false);
        CHECK(sorted_object_results16[0] == realm::decimal128(3.45));
        CHECK(sorted_object_results16[1] == realm::decimal128(2.34));
        CHECK(sorted_object_results16[2] == realm::decimal128(1.23));

        auto sorted_object_results17 = managed_obj.list_binary_col.sort(true);
        CHECK(sorted_object_results17[0] == std::vector<uint8_t>({0,0,0,0}));
        CHECK(sorted_object_results17[1] == std::vector<uint8_t>({0,0,0,1}));
        CHECK(sorted_object_results17[2] == std::vector<uint8_t>({0,0,0,2}));

        auto sorted_object_results18 = managed_obj.list_binary_col.sort(false);
        CHECK(sorted_object_results18[0] == std::vector<uint8_t>({0,0,0,2}));
        CHECK(sorted_object_results18[1] == std::vector<uint8_t>({0,0,0,1}));
        CHECK(sorted_object_results18[2] == std::vector<uint8_t>({0,0,0,0}));

        auto sorted_object_results19 = managed_obj.list_date_col.sort(true);
        CHECK(sorted_object_results19[0] == date1);
        CHECK(sorted_object_results19[1] == date2);
        CHECK(sorted_object_results19[2] == date3);

        auto sorted_object_results20 = managed_obj.list_date_col.sort(false);
        CHECK(sorted_object_results20[0] == date3);
        CHECK(sorted_object_results20[1] == date2);
        CHECK(sorted_object_results20[2] == date1);

        auto sorted_object_results21 = managed_obj.list_mixed_col.sort(true);
        CHECK(sorted_object_results21[0] == realm::mixed(1));
        CHECK(sorted_object_results21[1] == realm::mixed("foo"));
        CHECK(sorted_object_results21[2] == realm::mixed(date1));

        auto sorted_object_results22 = managed_obj.list_mixed_col.sort(false);
        CHECK(sorted_object_results22[0] == realm::mixed(date1));
        CHECK(sorted_object_results22[1] == realm::mixed("foo"));
        CHECK(sorted_object_results22[2] == realm::mixed(1));

        auto sorted_object_results23 = managed_obj.list_enum_col.sort(true);
        CHECK(sorted_object_results23[0] == experimental::AllTypesObject::Enum::one);
        CHECK(sorted_object_results23[1] == experimental::AllTypesObject::Enum::two);

        auto sorted_object_results24 = managed_obj.list_enum_col.sort(false);
        CHECK(sorted_object_results24[0] == experimental::AllTypesObject::Enum::two);
        CHECK(sorted_object_results24[1] == experimental::AllTypesObject::Enum::one);
    }
}
