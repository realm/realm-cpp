#include "test_objects.hpp"
#include "main.hpp"

TEST_CASE("binary", "[binary]") {
    realm_path path;
    SECTION("push_back") {
        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({.path=path});
        auto obj = AllTypesObject();
        obj.binary_col.push_back(1);
        obj.binary_col.push_back(2);
        obj.binary_col.push_back(3);
        realm.write([&realm, &obj] {
            realm.add(obj);
        });
        realm.write([&obj] {
            obj.binary_col.push_back(4);
        });
        CHECK(obj.binary_col[0] == 1);
        CHECK(obj.binary_col[1] == 2);
        CHECK(obj.binary_col[2] == 3);
        CHECK(obj.binary_col[3] == 4);
    }
}