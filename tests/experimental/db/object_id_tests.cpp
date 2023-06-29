#include "test_objects.hpp"
#include "../../main.hpp"

using namespace realm;

TEST_CASE("object_id", "[object_id]") {
    realm_path path;
    db_config config;
    config.set_path(path);
    SECTION("unmanaged_managed_object_id", "[object_id]") {
        auto realm = experimental::db(std::move(config));
        auto object_id1 = realm::object_id::generate();
        auto object_id2 = realm::object_id::generate();
        auto object_id3 = realm::object_id::generate();
        auto object_id4 = realm::object_id::generate();
        auto object = experimental::AllTypesObject();
        object.object_id_col = object_id1;
        object.opt_object_id_col = object_id2;
        object.list_object_id_col.push_back(object_id3);
        object.map_object_id_col = {
                {"a", object_id4}
        };

        CHECK(object.object_id_col == object_id1);
        CHECK(*object.opt_object_id_col == object_id2);
        CHECK(object.list_object_id_col[0] == object_id3);
        CHECK(object_id4 == object.map_object_id_col["a"]);

        auto managed_obj = realm.write([&] {
            return realm.add(std::move(object));
        });

        CHECK(managed_obj.object_id_col == object_id1);
        CHECK(*managed_obj.opt_object_id_col == object_id2);
        CHECK(managed_obj.list_object_id_col[0] == object_id3);
        CHECK(managed_obj.map_object_id_col["a"] == object_id4);

        const std::string val = "000123450000ffbeef91906c";
        auto object_id5 = realm::object_id(val);
        CHECK(object_id5.to_string() == val);
    }
}