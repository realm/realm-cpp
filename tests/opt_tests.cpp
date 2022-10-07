#include "main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("optional") {
    realm_path path;
    SECTION("unmanaged_managed_optional_get_set") {
        auto obj = AllTypesObject();
        CHECK(obj.opt_str_col == std::nullopt);
        obj.opt_str_col = "hello world";
        CHECK(obj.opt_str_col == "hello world");

        CHECK(obj.opt_int_col == std::nullopt);
        obj.opt_int_col = 42;
        CHECK(obj.opt_int_col == 42);

        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({.path=path});
        realm.write([&realm, &obj] {
            realm.add(obj);
        });

        CHECK(obj.opt_int_col == 42);
        CHECK(obj.opt_str_col == "hello world");
        realm.write([&obj] {
            obj.opt_str_col = std::nullopt;
        });
        CHECK(obj.opt_str_col == std::nullopt);
    }
}