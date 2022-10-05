#include "main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("mixed", "[mixed]") {
    realm_path path;
    SECTION("unmanaged_managed_mixed_get_set", "[mixed]") {
        auto obj = AllTypesObject();
        obj.mixed_col = 42;
        CHECK(obj.mixed_col == 42);
        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(
                {.path=path});
        realm.write([&realm, &obj] {
            realm.add(obj);
        });
        CHECK(obj.mixed_col == 42);
        realm.write([&obj] {
            obj.mixed_col = "hello world";
        });
        CHECK(obj.mixed_col == std::string("hello world"));
        realm.write([&obj] {
            obj.mixed_col = true;
        });
        CHECK(obj.mixed_col == true);
        auto time_point = std::chrono::time_point<std::chrono::system_clock>();
        realm.write([&obj, &time_point] {
            obj.mixed_col = time_point;
        });
        CHECK(obj.mixed_col == time_point);
        auto bin = std::vector<uint8_t>{0,1,2};
        realm.write([&obj, &bin] {
            obj.mixed_col = bin;
        });
        CHECK(obj.mixed_col == bin);
        realm.write([&obj] {
            obj.mixed_col = 42.42;
        });
        CHECK(obj.mixed_col == 42.42);
        auto u = uuid();
        realm.write([&obj, u] {
            obj.mixed_col = u;
        });
        CHECK(obj.mixed_col == u);
    }
}