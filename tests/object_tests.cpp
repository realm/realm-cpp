#include "test_objects.hpp"
#include "main.hpp"

enum class PrimaryKeyEnum {
    one, two
};
template<typename PrimaryKey>
struct TestPrimaryKeyObject : realm::object<TestPrimaryKeyObject<PrimaryKey>> {
    realm::persisted<PrimaryKey> _id;
    realm::persisted<std::string> firstName;
    realm::persisted<std::string> lastName;
    static constexpr auto schema = realm::schema("TestPrimaryKeyObject",
        realm::property<&TestPrimaryKeyObject::_id, true>("_id"),
        realm::property<&TestPrimaryKeyObject::firstName>("firstName"),
        realm::property<&TestPrimaryKeyObject::lastName>("lastName"));
};

TEST_CASE("all_primary_key_types") {
    SECTION("primary_keys") {
        std::tuple<int64_t, realm::object_id, std::string, realm::uuid, PrimaryKeyEnum> primary_keys
            = {123, realm::object_id::generate(), "primary_key", realm::uuid("68b696d7-320b-4402-a412-d9cee10fc6a3"), PrimaryKeyEnum::one};
        auto n = sizeof(primary_keys);
        for (size_t i = 0; i < n; i++) {
            {
                realm_path path;
                using ObjectType = TestPrimaryKeyObject<std::remove_reference<decltype(std::get<0>(
                        primary_keys))>::type>;
                auto realm = realm::open<ObjectType>({path});
                auto obj = ObjectType();
                obj._id = std::get<decltype(*obj._id)>(primary_keys);
                realm.write([&realm, &obj] {
                    realm.add(obj);
                });
                auto res = realm.objects<ObjectType>()[0];
                CHECK(res._id == std::get<decltype(*obj._id)>(primary_keys));
            }
        }
    }

    SECTION("optional_primary_keys") {
        std::tuple<std::optional<int64_t>, std::optional<realm::object_id>, std::optional<std::string>, std::optional<realm::uuid>, std::optional<PrimaryKeyEnum>> primary_keys
            = {123, realm::object_id::generate(), "primary_key", realm::uuid("68b696d7-320b-4402-a412-d9cee10fc6a3"), PrimaryKeyEnum::one};
        auto n = sizeof(primary_keys);
        for (size_t i = 0; i < n; i++) {
            {
                realm_path path;
                using ObjectType = TestPrimaryKeyObject<std::remove_reference<decltype(std::get<0>(
                        primary_keys))>::type>;
                auto realm = realm::open<ObjectType>({path});
                auto obj = ObjectType();
                obj._id = std::get<decltype(*obj._id)>(primary_keys);
                realm.write([&realm, &obj] {
                    realm.add(obj);
                });
                auto res = realm.objects<ObjectType>()[0];
                CHECK(*res._id == *std::get<decltype(*obj._id)>(primary_keys));
            }
        }
    }

    SECTION("optional_primary_keys_null_values") {
        std::tuple<std::optional<int64_t>, std::optional<realm::object_id>, std::optional<std::string>, std::optional<realm::uuid>, std::optional<PrimaryKeyEnum>> primary_keys;
        auto n = sizeof(primary_keys);
        for (size_t i = 0; i < n; i++) {
            {
                realm_path path;
                using ObjectType = TestPrimaryKeyObject<std::remove_reference<decltype(std::get<0>(
                        primary_keys))>::type>;
                auto realm = realm::open<ObjectType>({path});
                auto obj = ObjectType();
                obj._id = std::nullopt;
                realm.write([&realm, &obj] {
                    realm.add(obj);
                });
                auto res = realm.objects<ObjectType>()[0];
                CHECK(!res._id);
            }
        }
    }
}

TEST_CASE("object_notifications") {
    realm_path path;
    SECTION("observe") {
        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({path});

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
                    } else if (prop_change.name == "opt_embedded_obj_col" && prop_change.new_value) {
                        auto obj = std::get<std::optional<AllTypesObjectEmbedded>>(*prop_change.new_value);
                        CHECK(*(obj->str_col) == "embedded obj");
                    } else if (prop_change.name == "list_int_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<int64_t>>(*prop_change.new_value);
                        // Cocoa does not populate collection changes for an object and neither should we for perforamnce reasons.
                        CHECK(obj.size() == 0);
                    } else if (prop_change.name == "list_bool_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<bool>>(*prop_change.new_value);
                        CHECK(obj.size() == 0);
                    } else if (prop_change.name == "list_str_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<std::string>>(*prop_change.new_value);
                        CHECK(obj.size() == 0);
                    } else if (prop_change.name == "list_uuid_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<realm::uuid>>(*prop_change.new_value);
                        CHECK(obj.size() == 0);
                    } else if (prop_change.name == "list_binary_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<std::vector<uint8_t>>>(*prop_change.new_value);
                        CHECK(obj.size() == 0);
                    } else if (prop_change.name == "list_date_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<std::chrono::time_point<std::chrono::system_clock>>>(*prop_change.new_value);
                        CHECK(obj.size() == 0);
                    } else if (prop_change.name == "list_mixed_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<realm::mixed>>(*prop_change.new_value);
                        CHECK(obj.size() == 0);
                    } else if (prop_change.name == "list_obj_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<AllTypesObjectLink>>(*prop_change.new_value);
                        CHECK(obj.size() == 0);
                    } else if (prop_change.name == "list_embedded_obj_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<AllTypesObjectEmbedded>>(*prop_change.new_value);
                        CHECK(obj.size() == 0);
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
            o._id = 1;
            o.str_col = "link object";
            foo.opt_obj_col = o;
            o3.str_col = "embedded obj";
            foo.opt_embedded_obj_col = o3;

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

        CHECK(foo.opt_obj_col->is_managed());
        CHECK(*(foo.opt_obj_col->str_col) == "link object");
        CHECK(foo.list_obj_col[0].is_managed());
        CHECK(foo.list_obj_col[0].str_col == "link object 2");
        CHECK(foo.opt_embedded_obj_col->is_managed());
        CHECK(foo.opt_embedded_obj_col->str_col == "embedded obj");
        CHECK(foo.list_embedded_obj_col[0].is_managed());
        CHECK(foo.list_embedded_obj_col[0].str_col == "embedded obj 2");

        realm.write([&foo, &realm] {
            realm.remove(foo);
        });
        realm.refresh();

        CHECK(run_count == 2);
    }

    SECTION("optional objects") {
        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({path});

        auto foo = AllTypesObject();
        auto o = AllTypesObjectLink();
        o.str_col = "bar";
        AllTypesObjectEmbedded o2;
        o2.str_col = "embedded bar";
        foo.opt_obj_col = o;
        foo.opt_embedded_obj_col = o2;

        realm.write([&foo, &realm] {
            realm.add(foo);
        });

        realm.write([&] {
            foo.opt_obj_col = std::nullopt;
            foo.opt_embedded_obj_col = std::nullopt;
        });

        CHECK(*(foo.opt_obj_col) == std::nullopt);
        CHECK(*(foo.opt_embedded_obj_col) == std::nullopt);
    }
}