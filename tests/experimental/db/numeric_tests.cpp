#include "../../main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("numerics", "[numerics]") {
    realm_path path;
    db_config config;
    config.set_path(path);
    SECTION("unmanaged_managed_mixed_get_set", "[mixed]") {
        auto obj = experimental::AllTypesObject();
        obj.int_col = 1;
        obj.double_col = 1.0;
        obj.bool_col = true;
        obj.enum_col = experimental::AllTypesObject::Enum::one;

        obj.opt_int_col = 1;
        obj.opt_double_col = 1.0;
        obj.opt_bool_col = true;
        obj.opt_enum_col = experimental::AllTypesObject::Enum::one;

        obj.list_int_col.push_back(1);
        obj.map_int_col["foo"] = 1;
        obj.list_double_col.push_back(1.0);
        obj.map_double_col["foo"] = 1.0;
        obj.list_bool_col.push_back(true);
        obj.map_bool_col["foo"] = true;

        auto realm = experimental::db(std::move(config));
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });

        // Int
        CHECK(managed_obj.int_col == 1);
        CHECK((managed_obj.int_col != 1) == false);
        CHECK((managed_obj.int_col > 1) == false);
        CHECK((managed_obj.int_col < 1) == false);
        CHECK(((managed_obj.int_col <= 1) && (managed_obj.int_col <= 100)));
        CHECK(((managed_obj.int_col >= 1) && (managed_obj.int_col >= 0)));

        auto res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.int_col == 1;
        });
        CHECK(res.size() == 1);
        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.int_col != 1;
        });
        CHECK(res.size() == 0);
        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.int_col > 1;
        });
        CHECK(res.size() == 0);
        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.int_col < 1;
        });
        CHECK(res.size() == 0);
        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return (o.int_col <= 1) && (o.int_col <= 100);
        });
        CHECK(res.size() == 1);
        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return (o.int_col >= 1) && (o.int_col >= 0);
        });
        CHECK(res.size() == 1);

        realm.write([&realm, &managed_obj] {
            managed_obj.int_col = managed_obj.int_col + 1;
            CHECK(managed_obj.int_col == 2);

            managed_obj.int_col += 1;
            CHECK(managed_obj.int_col == 3);

            managed_obj.int_col -= 1;
            CHECK(managed_obj.int_col == 2);

            managed_obj.int_col *= 2;
            CHECK(managed_obj.int_col == 4);

            managed_obj.int_col++;
            CHECK(managed_obj.int_col == 5);

            managed_obj.int_col--;
            CHECK(managed_obj.int_col == 4);
        });

        // Double
        CHECK(managed_obj.double_col == 1.0);
        CHECK((managed_obj.double_col != 1.0) == false);
        CHECK((managed_obj.double_col > 1.0) == false);
        CHECK((managed_obj.double_col < 1.0) == false);
        CHECK(((managed_obj.double_col <= 1.0) && (managed_obj.double_col <= 100.0)));
        CHECK(((managed_obj.double_col >= 1.0) && (managed_obj.double_col >= 0.0)));

        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.double_col == 1.0;
        });
        CHECK(res.size() == 1);
        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.double_col != 1.0;
        });
        CHECK(res.size() == 0);
        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.double_col > 1.0;
        });
        CHECK(res.size() == 0);
        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.double_col < 1.0;
        });
        CHECK(res.size() == 0);
        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return (o.double_col <= 1.0) && (o.double_col <= 100.0);
        });
        CHECK(res.size() == 1);
        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return (o.double_col >= 1.0) && (o.double_col >= 0.0);
        });
        CHECK(res.size() == 1);

        realm.write([&realm, &managed_obj] {
            managed_obj.double_col = managed_obj.double_col + 1.0;
            CHECK(managed_obj.double_col == 2.0);

            managed_obj.double_col += 1;
            CHECK(managed_obj.double_col == 3.0);

            managed_obj.double_col -= 1;
            CHECK(managed_obj.double_col == 2.0);

            managed_obj.double_col *= 2;
            CHECK(managed_obj.double_col == 4.0);

            managed_obj.double_col++;
            CHECK(managed_obj.double_col == 5.0);

            managed_obj.double_col--;
            CHECK(managed_obj.double_col == 4.0);
        });

        // Opt Int
        CHECK(managed_obj.opt_int_col == 1);
        CHECK((managed_obj.opt_int_col != 1) == false);

        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.opt_int_col == 1;
        });
        CHECK(res.size() == 1);
        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.opt_int_col != 1;
        });
        CHECK(res.size() == 0);

        realm.write([&realm, &managed_obj] {
            managed_obj.opt_int_col = *managed_obj.opt_int_col.value() + 1;
            CHECK(managed_obj.opt_int_col == 2);

            managed_obj.opt_int_col += 1;
            CHECK(managed_obj.opt_int_col == 3);

            managed_obj.opt_int_col -= 1;
            CHECK(managed_obj.opt_int_col == 2);

            managed_obj.opt_int_col *= 2;
            CHECK(managed_obj.opt_int_col == 4);

            managed_obj.opt_int_col++;
            CHECK(managed_obj.opt_int_col == 5);

            managed_obj.opt_int_col--;
            CHECK(managed_obj.opt_int_col == 4);
        });

        // Opt Double
        CHECK(managed_obj.opt_double_col == 1.0);
        CHECK((managed_obj.opt_double_col != 1.0) == false);

        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.opt_double_col == 1.0;
        });
        CHECK(res.size() == 1);
        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.opt_double_col != 1.0;
        });
        CHECK(res.size() == 0);

        realm.write([&realm, &managed_obj] {
            managed_obj.opt_double_col = *managed_obj.opt_double_col.value() + 1.0;
            CHECK(managed_obj.opt_double_col == 2.0);

            managed_obj.opt_double_col += 1.0;
            CHECK(managed_obj.opt_double_col == 3.0);

            managed_obj.opt_double_col -= 1.0;
            CHECK(managed_obj.opt_double_col == 2.0);

            managed_obj.opt_double_col *= 2.0;
            CHECK(managed_obj.opt_double_col == 4.0);

            managed_obj.opt_double_col++;
            CHECK(managed_obj.opt_double_col == 5.0);

            managed_obj.opt_double_col--;
            CHECK(managed_obj.opt_double_col == 4.0);
        });

        // Enum

        CHECK(managed_obj.enum_col == experimental::AllTypesObject::Enum::one);
        CHECK(managed_obj.enum_col != experimental::AllTypesObject::Enum::two);

        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.enum_col == experimental::AllTypesObject::Enum::one;
        });
        CHECK(res.size() == 1);
        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.enum_col != experimental::AllTypesObject::Enum::one;
        });
        CHECK(res.size() == 0);

        realm.write([&realm, &managed_obj] {
            managed_obj.enum_col = experimental::AllTypesObject::Enum::two;
            CHECK(managed_obj.enum_col == experimental::AllTypesObject::Enum::two);
        });

        // Opt Enum
        CHECK(managed_obj.opt_enum_col == experimental::AllTypesObject::Enum::one);
        CHECK(managed_obj.opt_enum_col != experimental::AllTypesObject::Enum::two);

        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.opt_enum_col == experimental::AllTypesObject::Enum::one;
        });
        CHECK(res.size() == 1);
        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.opt_enum_col != experimental::AllTypesObject::Enum::one;
        });
        CHECK(res.size() == 0);

        realm.write([&realm, &managed_obj] {
            managed_obj.opt_enum_col = experimental::AllTypesObject::Enum::two;
            CHECK(managed_obj.opt_enum_col == experimental::AllTypesObject::Enum::two);
        });

        // Bool
        CHECK(managed_obj.bool_col == true);
        CHECK(managed_obj.bool_col != false);

        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.bool_col == true;
        });
        CHECK(res.size() == 1);
        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.bool_col != true;
        });
        CHECK(res.size() == 0);

        realm.write([&realm, &managed_obj] {
            managed_obj.bool_col = false;
            CHECK(managed_obj.bool_col == false);
        });

        // Opt Bool
        CHECK(managed_obj.opt_bool_col == true);
        CHECK(managed_obj.opt_bool_col != false);

        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.opt_bool_col == true;
        });
        CHECK(res.size() == 1);
        res = realm.objects<experimental::AllTypesObject>().where([](auto&& o) {
            return o.opt_bool_col != true;
        });
        CHECK(res.size() == 0);

        realm.write([&realm, &managed_obj] {
            managed_obj.opt_bool_col = false;
            CHECK(managed_obj.opt_bool_col == false);
        });

        // List
        CHECK(managed_obj.list_int_col[0] == 1);
        CHECK(managed_obj.list_double_col[0] == 1.0);
        CHECK(managed_obj.list_bool_col[0] == true);

        realm.write([&realm, &managed_obj] {
            // TODO: Allow subscript assignment
            // managed_obj.list_int_col[0] = 2;
            managed_obj.list_int_col.set(0, 2);
            CHECK(managed_obj.list_int_col[0] == 2);
            managed_obj.list_double_col.set(0, 2.0);
            CHECK(managed_obj.list_double_col[0] == 2.0);
            managed_obj.list_bool_col.set(0, false);
            CHECK(managed_obj.list_bool_col[0] == false);
        });

        // Map
        CHECK(managed_obj.map_int_col["foo"] == 1);
        CHECK(managed_obj.map_double_col["foo"] == 1.0);
        CHECK(managed_obj.map_bool_col["foo"] == true);

        realm.write([&realm, &managed_obj] {
            managed_obj.map_int_col["foo"] = 2;
            CHECK(managed_obj.map_int_col["foo"] == 2);
            managed_obj.map_double_col["foo"] = 2.0;
            CHECK(managed_obj.map_double_col["foo"] == 2);
            managed_obj.map_bool_col["foo"] = false;
            CHECK(managed_obj.map_bool_col["foo"] == false);
        });
    }
}