#include "../../main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("mixed", "[mixed]") {
    realm_path path;
    db_config config;
    config.set_path(path);
    SECTION("unmanaged_managed_mixed_get_set", "[mixed]") {
        auto obj = experimental::AllTypesObject();
        obj.mixed_col = 42;
        CHECK(obj.mixed_col == experimental::AllTypesObject::my_mixed(42));
        auto realm = experimental::db(std::move(config));
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        CHECK(managed_obj.mixed_col == static_cast<int64_t>(42));
        realm.write([&managed_obj] {
            managed_obj.mixed_col = "hello world";
        });
        CHECK(managed_obj.mixed_col == std::string("hello world"));
        realm.write([&managed_obj] {
            managed_obj.mixed_col = true;
        });
        CHECK(managed_obj.mixed_col == true);
        auto time_point = std::chrono::time_point<std::chrono::system_clock>();
        realm.write([&managed_obj, &time_point] {
            managed_obj.mixed_col = time_point;
        });
        CHECK(managed_obj.mixed_col == time_point);
        auto bin = std::vector<uint8_t>{0,1,2};
        realm.write([&managed_obj, &bin] {
            managed_obj.mixed_col = bin;
        });
        CHECK(managed_obj.mixed_col == bin);
        realm.write([&managed_obj] {
            managed_obj.mixed_col = 42.42;
        });
        CHECK(managed_obj.mixed_col == 42.42);
        auto u = uuid();
        realm.write([&managed_obj, u] {
            managed_obj.mixed_col = u;
        });
        CHECK(managed_obj.mixed_col == u);
    }
}