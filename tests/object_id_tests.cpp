#include "test_objects.hpp"
#include "main.hpp"

using namespace realm;

TEST_CASE("object_id", "[object_id]") {
    realm_path path;
    SECTION("unmanaged_managed_object_id", "[object_id]") {
        auto realm = open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({path});
        auto object_id1 = realm::object_id::generate();
        auto object_id2 = realm::object_id::generate();
        auto object_id3 = realm::object_id::generate();
        auto object_id4 = realm::object_id::generate();
        auto object = AllTypesObject();
        object.object_id_col = object_id1;
        object.opt_object_id_col = object_id2;
        object.list_object_id_col.push_back(object_id3);
        object.map_object_id_col = {
            {"a", object_id4}
        };

        CHECK(object_id1 == object.object_id_col);
        CHECK(object_id2 == *object.opt_object_id_col);
        CHECK(object_id3 == object.list_object_id_col[0]);
        CHECK(object.map_object_id_col["a"] == object_id4);

        realm.write([&] {
            realm.add(object);
        });

        CHECK(object.is_managed());
        CHECK(object_id1 == object.object_id_col);
        CHECK(object_id2 == *object.opt_object_id_col);
        CHECK(object_id3 == object.list_object_id_col[0]);
        CHECK(object.map_object_id_col["a"] == object_id4);

        const std::string val = "000123450000ffbeef91906c";
        auto object_id5 = realm::object_id(val);
        CHECK(object_id5.to_string() == val);
    }
}