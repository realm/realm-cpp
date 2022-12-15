#include "test_objects.hpp"
#include "main.hpp"

TEST_CASE("object_notifications") {
    realm_path path;
    SECTION("observe") {
        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({.path=path});
        auto date = std::chrono::time_point<std::chrono::system_clock>();
        auto uuid = realm::uuid();
        auto foo = AllTypesObject();
        auto o = AllTypesObjectLink();
        o._id = 1;

        realm.write([&foo, &realm]() {
            realm.add(foo);
        });

        int run_count = 0;
        auto token = foo.observe<AllTypesObject>([&](auto change) {
            if (run_count == 1) {
                CHECK(change.is_deleted);
            } else {
                for (auto& prop_change : change.property_changes) {
                    if (prop_change.name == "str_col" && prop_change.new_value) {
                        CHECK(std::get<std::string>(*prop_change.new_value) == "foo");
                    } else if (prop_change.name == "bool_col" && prop_change.new_value) {
                        CHECK(std::get<bool>(*prop_change.new_value) == true);
                    } else if (prop_change.name == "enum_col" && prop_change.new_value) {
                        CHECK(std::get<AllTypesObject::Enum>(*prop_change.new_value) == AllTypesObject::Enum::two);
                    } else if (prop_change.name == "date_col" && prop_change.new_value) {
                        CHECK(std::get<std::chrono::time_point<std::chrono::system_clock>>(*prop_change.new_value) == date);
                    } else if (prop_change.name == "uuid_col" && prop_change.new_value) {
                        CHECK(std::get<realm::uuid>(*prop_change.new_value) == uuid);
                    } else if (prop_change.name == "binary_col" && prop_change.new_value) {
                        CHECK(std::get<std::vector<uint8_t>>(*prop_change.new_value) == std::vector<uint8_t>({1}));
                    } else if (prop_change.name == "mixed_col" && prop_change.new_value) {
                        auto val = std::get<realm::mixed>(*prop_change.new_value);
                        CHECK(std::get<std::string>(val) == "mixed");
                    } else if (prop_change.name == "opt_str_col" && prop_change.new_value) {
                        CHECK(std::get<std::optional<std::string>>(*prop_change.new_value) == "opt string");
                    } else if (prop_change.name == "opt_bool_col" && prop_change.new_value) {
                        CHECK(std::get<std::optional<bool>>(*prop_change.new_value) == true);
                    } else if (prop_change.name == "opt_enum_col" && prop_change.new_value) {
                        CHECK(std::get<std::optional<AllTypesObject::Enum>>(*prop_change.new_value) == AllTypesObject::Enum::two);
                    } else if (prop_change.name == "opt_date_col" && prop_change.new_value) {
                        CHECK(std::get<std::optional<std::chrono::time_point<std::chrono::system_clock>>>(*prop_change.new_value) == date);
                    } else if (prop_change.name == "opt_uuid_col" && prop_change.new_value) {
                        CHECK(std::get<std::optional<realm::uuid>>(*prop_change.new_value) == uuid);
                    } else if (prop_change.name == "opt_binary_col" && prop_change.new_value) {
                        CHECK(std::get<std::optional<std::vector<uint8_t>>>(*prop_change.new_value) == std::vector<uint8_t>({1}));
                    } else if (prop_change.name == "opt_obj_col" && prop_change.new_value) {
                        auto obj = std::get<std::optional<AllTypesObjectLink>>(*prop_change.new_value);
                        CHECK(*(obj->str_col) == "link object");
                    }
                }
            }

            run_count++;
        });

        realm.write([&]() {
            foo.str_col = "foo";
            foo.bool_col = true;
            foo.enum_col = AllTypesObject::Enum::two;
            foo.date_col = date;
            foo.uuid_col = realm::uuid();
            foo.binary_col.push_back(1);
            foo.mixed_col = "mixed";

            foo.opt_int_col = 2;
            foo.opt_str_col = "opt string";
            foo.opt_bool_col = true;
            foo.opt_enum_col = AllTypesObject::Enum::two;
            foo.opt_date_col = date;
            foo.opt_uuid_col = uuid;
            foo.opt_binary_col = std::vector<uint8_t>({1});
            o._id = 123;
            o.str_col = "link object";
            foo.opt_obj_col = o;

            foo.list_int_col.push_back(1);
            foo.list_bool_col.push_back(true);
            foo.list_str_col.push_back("bar");
            foo.list_uuid_col.push_back(realm::uuid());
            foo.list_binary_col.push_back({1});
            foo.list_date_col.push_back(date);
            foo.list_mixed_col.push_back("mixed str");
            AllTypesObjectLink o2;
            o2._id = 2;
            o2.str_col = "link object 2";
            foo.list_obj_col.push_back(o2);
            AllTypesObjectEmbedded o3;
            o3.str_col = "embedded obj";

            foo.list_embedded_obj_col.push_back(o3);
        });
        realm.refresh();

        auto x = *o.str_col;
        auto xx = o.is_managed();

        realm.write([&foo, &realm] {
            realm.remove(foo);
        });
        realm.refresh();
        CHECK(run_count == 2);
    }

    SECTION("scope") {

    }
}
