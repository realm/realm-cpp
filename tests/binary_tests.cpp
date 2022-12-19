#include "test_objects.hpp"
#include "main.hpp"

TEST_CASE("binary", "[binary]") {
    realm_path path;
    SECTION("push_back") {
        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(path);
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
        CHECK(obj.binary_col == obj.binary_col);
    }

    SECTION("list_binary") {
        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(path);
        auto obj = AllTypesObject();
        obj.list_binary_col.push_back({0});
        obj.list_binary_col.push_back({1});
        obj.list_binary_col.push_back({2});
        realm.write([&realm, &obj] {
            realm.add(obj);
        });
        realm.write([&obj] {
            obj.list_binary_col.push_back({3});
        });
        for (size_t i = 0; i < obj.list_binary_col.size(); i++) {
            CHECK(obj.list_binary_col[i] == std::vector<uint8_t>({ static_cast<uint8_t>(i) }));
        }
    }
}
