#include "test_utils.hpp"
#include "test_objects.hpp"

using namespace realm;

auto get_realm(const std::string& path) {
    return open<AllTypesObject, AllTypesObjectLink>({.path=path});
}

TEST(unmanaged_str_get_set) {
    auto obj = AllTypesObject();
    obj.str_col = "foo";
    CHECK_EQUALS(obj.str_col, "foo");
    obj.str_col = "bar";
    CHECK_EQUALS(obj.str_col, "bar");
    co_return;
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
    co_return;
}

TEST(unmanaged_str_contains) {
    auto obj = AllTypesObject();
    obj.str_col = "foo";
    CHECK(obj.str_col.contains("oo"));
    co_return;
}

TEST(managed_str_contains) {
    auto obj = AllTypesObject();
    auto realm = get_realm(path);
    realm.write([&realm, &obj] {
        realm.add(obj);
        obj.str_col = "foo";
    });
    CHECK(obj.str_col.contains("oo"));
    co_return;
}