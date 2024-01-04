#include "../main.hpp"
#include "test_objects.hpp"

namespace realm {

    TEST_CASE("string", "[str]") {
        realm_path path;
        realm::db_config config;
        config.set_path(path);

        SECTION("managed_str_get_set", "[str]") {
            auto obj = AllTypesObject();
            auto realm = db(std::move(config));
            auto managed_obj = realm.write([&realm, &obj] {
                return realm.add(std::move(obj));
            });
            CHECK_THROWS(managed_obj.str_col = "foo");
            CHECK(managed_obj.str_col == "");
            realm.write([&managed_obj] { managed_obj.str_col = "bar"; });
            CHECK(managed_obj.str_col == "bar");
        }

        SECTION("managed_str_contains", "[str]") {
            auto obj = AllTypesObject();
            auto realm = db(std::move(config));
            auto managed_obj = realm.write([&realm, &obj] {
                obj.str_col = "foo";
                return realm.add(std::move(obj));
            });
            CHECK(managed_obj.str_col.contains("oo"));
        }
    }
}