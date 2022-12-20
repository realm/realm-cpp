#include "test_objects.hpp"
#include "main.hpp"

TEST_CASE("object_notifications") {
    realm_path path;
    SECTION("observe") {
        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(path);

        auto date = std::chrono::time_point<std::chrono::system_clock>();
        auto uuid = realm::uuid();
        auto foo = AllTypesObject();
        auto o = AllTypesObjectLink();
        AllTypesObjectLink o2;
        AllTypesObjectEmbedded o3;
        AllTypesObjectEmbedded o4;

        realm.write([&foo, &realm]() {
            realm.add(foo);
        });

        int run_count = 0;
        auto token = foo.observe([&](auto change) {
            if (run_count == 1) {
                CHECK(change.is_deleted);
            } else {
                CHECK(change.property_changes.size() == 25);
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
//                        auto val = std::get<realm::mixed>(*prop_change.new_value);
//                        CHECK(std::get<std::string>(val) == "mixed");
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
//                        auto obj = std::get<std::optional<AllTypesObjectLink>>(*prop_change.new_value);
//                        CHECK(*(obj->str_col) == "link object");
                    } else if (prop_change.name == "opt_embedded_obj_col" && prop_change.new_value) {
//                        auto obj = std::get<std::optional<AllTypesObjectEmbedded>>(*prop_change.new_value);
//                        CHECK(*(obj->str_col) == "embedded obj");
                    } else if (prop_change.name == "list_int_col" && prop_change.new_value) {
//                        auto obj = std::get<std::vector<int>>(*prop_change.new_value);
//                        CHECK(obj.size() == 1);
//                        CHECK(obj[0] == 1);
                    } else if (prop_change.name == "list_bool_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<bool>>(*prop_change.new_value);
                        CHECK(obj.size() == 1);
                        CHECK(obj[0] == true);
                    } else if (prop_change.name == "list_str_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<std::string>>(*prop_change.new_value);
                        CHECK(obj.size() == 1);
                        CHECK(obj[0] == "bar");
                    } else if (prop_change.name == "list_str_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<std::string>>(*prop_change.new_value);
                        CHECK(obj.size() == 1);
                        CHECK(obj[0] == "bar");
                    } else if (prop_change.name == "list_uuid_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<realm::uuid>>(*prop_change.new_value);
                        CHECK(obj.size() == 1);
                        CHECK(obj[0] == uuid);
                    } else if (prop_change.name == "list_binary_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<std::vector<uint8_t>>>(*prop_change.new_value);
                        CHECK(obj.size() == 1);
                        CHECK(obj[0] == std::vector<uint8_t>({1}));
                    } else if (prop_change.name == "list_date_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<std::chrono::time_point<std::chrono::system_clock>>>(*prop_change.new_value);
                        CHECK(obj.size() == 1);
                        CHECK(obj[0] == date);
                    } else if (prop_change.name == "list_mixed_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<realm::mixed>>(*prop_change.new_value);
                        CHECK(obj.size() == 1);
                        CHECK(std::get<std::string>(obj[0]) == "mixed str");
                    } else if (prop_change.name == "list_obj_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<AllTypesObjectLink>>(*prop_change.new_value);
                        CHECK(obj.size() == 1);
                        CHECK(*obj[0].str_col == "link object 2");
                    } else if (prop_change.name == "list_embedded_obj_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<AllTypesObjectEmbedded>>(*prop_change.new_value);
                        CHECK(obj.size() == 1);
                        CHECK(*obj[0].str_col == "embedded obj 2");
                    }
                }
            }

            run_count++;
        });

        realm.write([&]() {
            foo.str_col = "foo";
//            foo.bool_col = true;
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
            o._id = 1;
            o.str_col = "link object";
            foo.opt_obj_col = o;
            o3.str_col = "embedded obj";
//            foo.opt_embedded_obj_col = o3;

            foo.list_int_col.push_back(1);
            foo.list_bool_col.push_back(true);
            foo.list_str_col.push_back("bar");
            foo.list_uuid_col.push_back(uuid);
            foo.list_binary_col.push_back({1});
            foo.list_date_col.push_back(date);
            foo.list_mixed_col.push_back("mixed str");
            o2._id = 2;
            o2.str_col = "link object 2";
            foo.list_obj_col.push_back(o2);
            o4.str_col = "embedded obj 2";
            foo.list_embedded_obj_col.push_back(o4);
        });
        realm.refresh();

        CHECK(o.is_managed());
        CHECK(o.str_col == "link object");
        CHECK(o2.is_managed());
        CHECK(o2.str_col == "link object 2");
        CHECK(o3.is_managed());
        CHECK(o3.str_col == "embedded obj");
        CHECK(o4.is_managed());
        CHECK(o4.str_col == "embedded obj 2");

//        CHECK(foo.opt_obj_col.operator*()->is_managed());
//        CHECK(foo.opt_obj_col.operator*()->str_col == "link object");
//        CHECK(foo.list_obj_col.operator*()[0].is_managed());
//        CHECK(foo.list_obj_col.operator*()[0].str_col == "link object 2");
//        CHECK(foo.opt_embedded_obj_col.operator*()->is_managed());
//        CHECK(foo.opt_embedded_obj_col.operator*()->str_col == "embedded obj");
//        CHECK(foo.list_embedded_obj_col.operator*()[0].is_managed());
//        CHECK(foo.list_embedded_obj_col.operator*()[0].str_col == "embedded obj 2");

        realm.write([&foo, &realm] {
            realm.remove(foo);
        });
        realm.refresh();

        CHECK(run_count == 2);
    }

    SECTION("optional objects") {
        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(path);

        auto foo = AllTypesObject();
        auto o = AllTypesObjectLink();
        o.str_col = "bar";
        AllTypesObjectEmbedded o2;
        o2.str_col = "embedded bar";

        auto opt_obj = std::optional<AllTypesObjectLink>(o);
        auto opt_embedded_obj = std::optional<AllTypesObjectEmbedded>(o2);

        realm.write([&foo, &realm] {
            realm.add(foo);
        });

        realm.write([&] {
//            foo.opt_obj_col = opt_obj;
//            foo.opt_embedded_obj_col = opt_embedded_obj;
        });

//        CHECK(foo.opt_obj_col.operator*()->is_managed());
//        CHECK(foo.opt_obj_col.operator*()->str_col == "bar");
//
//        CHECK(foo.opt_embedded_obj_col.operator*()->is_managed());
//        CHECK(foo.opt_embedded_obj_col.operator*()->str_col == "embedded bar");

    }

//    SECTION("observation scope") {
//        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(path);
//
//        realm::notification_token token;
//        // Keep object outside of scope with token;
//        AllTypesObject foo;
//        int run_count = 0;
//
//        {
//            realm.write([&foo, &realm]() {
//                realm.add(foo);
//            });
//
//            token = foo.observe<AllTypesObject>([&](auto change) {
//                run_count++;
//            });
//        }
//
//        AllTypesObject o = *realm.objects<AllTypesObject>()[0];
//
//        realm.write([&o, &realm]() {
//            o.str_col = "foo";
//        });
//
//        realm.refresh();
//        CHECK(run_count == 1);
//    }
}