#include "test_utils.hpp"
#include "test_objects.hpp"

using namespace realm;

auto get_realm(const std::string& path) {
    return open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({.path=path});
}

TEST(unmanaged_str_get_set) {
    auto obj = AllTypesObject();
    obj.str_col = "foo";
    CHECK_EQUALS(obj.str_col, "foo");
    obj.str_col = "bar";
    CHECK_EQUALS(obj.str_col, "bar");
}

TEST(managed_str_get_set) {
    auto obj = AllTypesObject();
    auto realm = get_realm(path);
    realm.write([&realm, &obj] {
        realm.add(obj);
    });
    CHECK_THROWS([&obj] { obj.str_col = "foo"; });
    CHECK_EQUALS(obj.str_col, "");
    realm.write([&obj] { obj.str_col = "bar"; });
    CHECK_EQUALS(obj.str_col, "bar");
}

TEST(unmanaged_str_contains) {
    auto obj = AllTypesObject();
    obj.str_col = "foo";
    CHECK(obj.str_col.contains("oo"));
}

TEST(managed_str_contains) {
    auto obj = AllTypesObject();
    auto realm = get_realm(path);
    realm.write([&realm, &obj] {
        realm.add(obj);
        obj.str_col = "foo";
    });
    CHECK(obj.str_col.contains("oo"));
}
TEST(custom_str) {
    auto obj = CustomStringObject();
    auto realm = realm::open<CustomStringObject>({path});
    realm.write([&realm, &obj] {
        realm.add(obj);
        obj.str_col = "foo";
    });
    CHECK_EQUALS(obj.str_col, "FOO");
    CHECK(obj.str_col.contains("OO"))
}