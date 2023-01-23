#include "main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("string", "[str]") {
    realm_path path;
    SECTION("unmanaged_str_get_set", "[str]") {
        auto obj = AllTypesObject();
        obj.str_col = "foo";
        CHECK(obj.str_col == "foo");
        obj.str_col = "bar";
        CHECK(obj.str_col == "bar");
    }

    SECTION("managed_str_get_set", "[str]") {
        auto obj = AllTypesObject();
        auto realm = open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({.path=path});
        realm.write([&realm, &obj] {
            realm.add(obj);
        });
        CHECK_THROWS(obj.str_col = "foo");
        CHECK(obj.str_col == "");
        realm.write([&obj] { obj.str_col = "bar"; });
        CHECK(obj.str_col == "bar");
    }

    SECTION("unmanaged_str_contains", "[str]") {
        auto obj = AllTypesObject();
        obj.str_col = "foo";
        CHECK(obj.str_col.contains("oo"));
    }

    SECTION("managed_str_contains", "[str]") {
        auto obj = AllTypesObject();
        auto realm = open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({.path=path});
        realm.write([&realm, &obj] {
            realm.add(obj);
            obj.str_col = "foo";
        });
        CHECK(obj.str_col.contains("oo"));
    }
    SECTION("custom_str", "[str]") {
        auto obj = CustomStringObject();
        auto realm = realm::open<CustomStringObject>({path});
        realm.write([&realm, &obj] {
            realm.add(obj);
            obj.str_col = "foo";
        });
        CHECK(obj.str_col == "FOO");
        CHECK(obj.str_col.contains("OO"));
    }
}