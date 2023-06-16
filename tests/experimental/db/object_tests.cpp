#include "test_objects.hpp"
#include "../../main.hpp"

namespace realm::experimental {

    enum class PrimaryKeyEnum {
        one,
        two
    };

    struct PK1 {
        primary_key<int64_t> _id;
        std::string str_col;
    };
    REALM_SCHEMA(PK1, _id, str_col)
    struct PK2 {
        primary_key<realm::object_id> _id;
        std::string str_col;
    };
    REALM_SCHEMA(PK2, _id, str_col)
    struct PK3 {
        primary_key<std::string> _id;
        std::string str_col;
    };
    REALM_SCHEMA(PK3, _id, str_col)
    struct PK4 {
        primary_key<realm::uuid> _id;
        std::string str_col;
    };
    REALM_SCHEMA(PK4, _id, str_col)
    struct PK5 {
        primary_key<PrimaryKeyEnum> _id;
        std::string str_col;
    };
    REALM_SCHEMA(PK5, _id, str_col)

    struct PK1_opt {
        primary_key<std::optional<int64_t>> _id;
        std::string str_col;
    };
    REALM_SCHEMA(PK1_opt, _id, str_col)
    struct PK2_opt {
        primary_key<std::optional<realm::object_id>> _id;
        std::string str_col;
    };
    REALM_SCHEMA(PK2_opt, _id, str_col)
    struct PK3_opt {
        primary_key<std::optional<std::string>> _id;
        std::string str_col;
    };
    REALM_SCHEMA(PK3_opt, _id, str_col)
    struct PK4_opt {
        primary_key<std::optional<realm::uuid>> _id;
        std::string str_col;
    };
    REALM_SCHEMA(PK4_opt, _id, str_col)

    TEST_CASE("all_primary_key_types") {
        realm_path path;
        realm::db_config config;
        config.set_path(path);


        SECTION("primary_keys") {
            auto realm = db(std::move(config));

            PK1 pk1;
            pk1._id = 123;
            pk1.str_col = "pk1";
            auto managed_pk1 = realm.write([&] {
                return realm.add(std::move(pk1));
            });

            auto obj_id = realm::object_id::generate();
            PK2 pk2;
            pk2._id = obj_id;
            pk2.str_col = "pk2";
            auto managed_pk2 = realm.write([&] {
                return realm.add(std::move(pk2));
            });

            PK3 pk3;
            pk3._id = "primary_key";
            pk3.str_col = "pk3";
            auto managed_pk3 = realm.write([&] {
                return realm.add(std::move(pk3));
            });

            PK4 pk4;
            pk4._id = realm::uuid("68b696d7-320b-4402-a412-d9cee10fc6a3");
            pk4.str_col = "pk4";
            auto managed_pk4 = realm.write([&] {
                return realm.add(std::move(pk4));
            });

            CHECK(managed_pk1._id == 123);
            CHECK(managed_pk1._id != 321);
            CHECK(managed_pk1._id > 100);
            CHECK(managed_pk1._id >= 123);
            CHECK(managed_pk1._id < 500);
            CHECK(managed_pk1._id <= 123);

            CHECK(managed_pk1._id == (int64_t)123);
            CHECK(managed_pk1._id != (int64_t)321);
            CHECK(managed_pk1._id > (int64_t)100);
            CHECK(managed_pk1._id >= (int64_t)123);
            CHECK(managed_pk1._id < (int64_t)500);
            CHECK(managed_pk1._id <= (int64_t)123);

            CHECK(managed_pk2._id == obj_id);
            CHECK(managed_pk2._id != realm::object_id::generate());

            CHECK(managed_pk3._id == "primary_key");
            CHECK(managed_pk3._id != "pk3");

            CHECK(managed_pk4._id == realm::uuid("68b696d7-320b-4402-a412-d9cee10fc6a3"));
            CHECK(managed_pk4._id != realm::uuid("68b696d7-320b-4402-a412-d9cee10fc6a4"));

            CHECK(realm.objects<PK1>().where([](auto& o) { return o._id == 123; }).size() == 1);
            CHECK(realm.objects<PK1>().where([](auto& o) { return o._id != 123; }).size() == 0);
            CHECK(realm.objects<PK2>().where([&](auto& o) { return o._id == obj_id; }).size() == 1);
            CHECK(realm.objects<PK2>().where([&](auto& o) { return o._id != obj_id; }).size() == 0);
            CHECK(realm.objects<PK3>().where([](auto& o) { return o._id == "primary_key"; }).size() == 1);
            CHECK(realm.objects<PK3>().where([](auto& o) { return o._id != "primary_key"; }).size() == 0);
            CHECK(realm.objects<PK4>().where([](auto& o) { return o._id == realm::uuid("68b696d7-320b-4402-a412-d9cee10fc6a3"); }).size() == 1);
            CHECK(realm.objects<PK4>().where([](auto& o) { return o._id != realm::uuid("68b696d7-320b-4402-a412-d9cee10fc6a3"); }).size() == 0);
        }

        SECTION("optional_primary_keys") {
            auto realm = db(std::move(config));

            PK1_opt pk1;
            pk1._id = 123;
            pk1.str_col = "pk1";
            auto managed_pk1 = realm.write([&] {
                return realm.add(std::move(pk1));
            });

            auto obj_id = realm::object_id::generate();
            PK2_opt pk2;
            pk2._id = obj_id;
            pk2.str_col = "pk2";
            auto managed_pk2 = realm.write([&] {
                return realm.add(std::move(pk2));
            });

            PK3_opt pk3;
            pk3._id = "primary_key";
            pk3.str_col = "pk3";
            auto managed_pk3 = realm.write([&] {
                return realm.add(std::move(pk3));
            });

            PK4_opt pk4;
            pk4._id = realm::uuid("68b696d7-320b-4402-a412-d9cee10fc6a3");
            pk4.str_col = "pk4";
            auto managed_pk4 = realm.write([&] {
                return realm.add(std::move(pk4));
            });

            CHECK(managed_pk1._id == 123);
            CHECK(managed_pk1._id != 321);
            CHECK(managed_pk1._id > 100);
            CHECK(managed_pk1._id >= 123);
            CHECK(managed_pk1._id < 500);
            CHECK(managed_pk1._id <= 123);

            CHECK(managed_pk1._id == (int64_t)123);
            CHECK(managed_pk1._id != (int64_t)321);
            CHECK(managed_pk1._id > (int64_t)100);
            CHECK(managed_pk1._id >= (int64_t)123);
            CHECK(managed_pk1._id < (int64_t)500);
            CHECK(managed_pk1._id <= (int64_t)123);

            CHECK(managed_pk2._id == obj_id);
            CHECK(managed_pk2._id != realm::object_id::generate());

            CHECK(managed_pk3._id == "primary_key");
            CHECK(managed_pk3._id != "pk3");

            CHECK(managed_pk4._id == realm::uuid("68b696d7-320b-4402-a412-d9cee10fc6a3"));
            CHECK(managed_pk4._id != realm::uuid("68b696d7-320b-4402-a412-d9cee10fc6a4"));

            CHECK(realm.objects<PK1_opt>().where([](auto& o) { return o._id == std::nullopt; }).size() == 0);
            CHECK(realm.objects<PK1_opt>().where([](auto& o) { return o._id != std::nullopt; }).size() == 1);
            CHECK(realm.objects<PK2_opt>().where([](auto& o) { return o._id == std::nullopt; }).size() == 0);
            CHECK(realm.objects<PK2_opt>().where([](auto& o) { return o._id != std::nullopt; }).size() == 1);
            CHECK(realm.objects<PK3_opt>().where([](auto& o) { return o._id == std::nullopt; }).size() == 0);
            CHECK(realm.objects<PK3_opt>().where([](auto& o) { return o._id != std::nullopt; }).size() == 1);
            CHECK(realm.objects<PK4_opt>().where([](auto& o) { return o._id == std::nullopt; }).size() == 0);
            CHECK(realm.objects<PK4_opt>().where([](auto& o) { return o._id != std::nullopt; }).size() == 1);
        }

        SECTION("optional_primary_keys_null_initialized") {
            auto realm = db(std::move(config));

            PK1_opt pk1;
            pk1._id = std::nullopt;
            pk1.str_col = "pk1";
            auto managed_pk1 = realm.write([&] {
                return realm.add(std::move(pk1));
            });

            auto obj_id = realm::object_id::generate();
            PK2_opt pk2;
            pk2._id = std::nullopt;
            pk2.str_col = "pk2";
            auto managed_pk2 = realm.write([&] {
                return realm.add(std::move(pk2));
            });

            PK3_opt pk3;
            pk3._id = std::nullopt;
            pk3.str_col = "pk3";
            auto managed_pk3 = realm.write([&] {
                return realm.add(std::move(pk3));
            });

            PK4_opt pk4;
            pk4._id = std::nullopt;
            pk4.str_col = "pk4";
            auto managed_pk4 = realm.write([&] {
                return realm.add(std::move(pk4));
            });

            CHECK(managed_pk1._id == std::nullopt);
            CHECK(!(managed_pk1._id != std::nullopt));

            CHECK(managed_pk2._id == std::nullopt);
            CHECK(!(managed_pk2._id != std::nullopt));

            CHECK(managed_pk3._id == std::nullopt);
            CHECK(!(managed_pk3._id != std::nullopt));

            CHECK(managed_pk4._id == std::nullopt);
            CHECK(!(managed_pk4._id != std::nullopt));

            CHECK(realm.objects<PK1_opt>().where([](auto& o) { return o._id == std::nullopt; }).size() == 1);
            CHECK(realm.objects<PK1_opt>().where([](auto& o) { return o._id != std::nullopt; }).size() == 0);
            CHECK(realm.objects<PK2_opt>().where([](auto& o) { return o._id == std::nullopt; }).size() == 1);
            CHECK(realm.objects<PK2_opt>().where([](auto& o) { return o._id != std::nullopt; }).size() == 0);
            CHECK(realm.objects<PK3_opt>().where([](auto& o) { return o._id == std::nullopt; }).size() == 1);
            CHECK(realm.objects<PK3_opt>().where([](auto& o) { return o._id != std::nullopt; }).size() == 0);
            CHECK(realm.objects<PK4_opt>().where([](auto& o) { return o._id == std::nullopt; }).size() == 1);
            CHECK(realm.objects<PK4_opt>().where([](auto& o) { return o._id != std::nullopt; }).size() == 0);
        }

        SECTION("object_initialization_beta") {
            auto date = std::chrono::time_point<std::chrono::system_clock>();
            auto uuid = realm::uuid();
            auto link1 = AllTypesObjectLink();
            link1._id = 1;
            link1.str_col = "link object";

            auto link2 = AllTypesObjectLink();
            link2._id = 2;
            link2.str_col = "link object 2";

            auto link3 = AllTypesObjectLink();
            link3._id = 3;
            link3.str_col = "link object 3";

            AllTypesObjectEmbedded embedded_obj1;
            embedded_obj1.str_col = "embedded obj1";
            AllTypesObjectEmbedded embedded_obj2;
            embedded_obj2.str_col = "embedded obj2";
            AllTypesObjectEmbedded embedded_obj3;
            embedded_obj3.str_col = "embedded obj3";

            auto object_id = realm::object_id::generate();

            AllTypesObject obj;
            obj._id = 123;
            obj.double_col = 12.34;
            obj.bool_col = true;
            obj.str_col = std::string("foo");
            obj.enum_col = AllTypesObject::Enum::two;
            obj.date_col = date;
            obj.uuid_col = uuid;
            obj.object_id_col = object_id;
            obj.binary_col = std::vector<uint8_t>({1});
            obj.mixed_col = AllTypesObject::my_mixed("mixed");

            obj.opt_int_col = 2;
            obj.opt_double_col = 2.34;
            obj.opt_str_col = "opt string";
            obj.opt_bool_col = true;
            obj.opt_enum_col = AllTypesObject::Enum::two;
            obj.opt_date_col = date;
            obj.opt_uuid_col = uuid;
            obj.opt_object_id_col = object_id;
            obj.opt_binary_col = std::vector<uint8_t>({1});

            obj.opt_obj_col = &link1,
            obj.opt_embedded_obj_col = &embedded_obj1,

            obj.list_int_col = std::vector<int64_t>({1});
            obj.list_double_col = std::vector<double>({1.23});
            obj.list_bool_col = std::vector<bool>({true});
            obj.list_str_col = std::vector<std::string>({"bar"});
            obj.list_uuid_col = std::vector<realm::uuid>({uuid});
            obj.list_object_id_col = std::vector<realm::object_id>({object_id});
            obj.list_binary_col = std::vector<std::vector<uint8_t>>({{1}});
            obj.list_date_col = std::vector<std::chrono::time_point<std::chrono::system_clock>>({date});
            obj.list_mixed_col = std::vector<realm::mixed>({realm::mixed("mixed str")});
            obj.list_obj_col = std::vector<AllTypesObjectLink*>({&link2});
            obj.list_embedded_obj_col = std::vector<AllTypesObjectEmbedded*>({&embedded_obj2});

            obj.map_int_col = std::map<std::string, int64_t>({{"foo", 1}});
            obj.map_double_col = std::map<std::string, double>({{"foo", 1.23}});
            obj.map_bool_col = std::map<std::string, bool>({{"foo", true}});
            obj.map_str_col = std::map<std::string, std::string>({{"foo", "bar"}});
            obj.map_uuid_col = std::map<std::string, realm::uuid>({{"foo", uuid}});
            obj.map_object_id_col = std::map<std::string, realm::object_id>({{"foo", object_id}});
            obj.map_binary_col = std::map<std::string, std::vector<std::uint8_t>>({{"foo", {1}}});
            obj.map_date_col = std::map<std::string, std::chrono::time_point<std::chrono::system_clock>>({{"foo", date}});
            obj.map_enum_col = std::map<std::string, AllTypesObject::Enum>({{"foo", AllTypesObject::Enum::one}});
            obj.map_mixed_col = std::map<std::string, realm::mixed>({{"foo", realm::mixed("bar")}});
            obj.map_link_col = std::map<std::string, AllTypesObjectLink*>({{"foo", &link3}});
            obj.map_embedded_col = std::map<std::string, AllTypesObjectEmbedded*>({{"foo", &embedded_obj3}});

            experimental::db db = experimental::open(path);
            auto managed_obj = db.write([&obj, &db]() {
                return db.add(std::move(obj));
            });

            CHECK(managed_obj._id == (int64_t)123);
            CHECK(managed_obj.double_col == 12.34);
            CHECK(managed_obj.str_col == "foo");
            CHECK(managed_obj.bool_col == true);
            CHECK(managed_obj.enum_col == AllTypesObject::Enum::two);
            CHECK(managed_obj.date_col == date);
            CHECK(managed_obj.uuid_col.value() == uuid);
            CHECK(managed_obj.binary_col.value() == std::vector<uint8_t>({1}));
            CHECK(managed_obj.mixed_col.value() == AllTypesObject::my_mixed("mixed"));

            CHECK(managed_obj.object_id_col.value() == object_id);

            CHECK(managed_obj.opt_int_col == 2);
            CHECK(managed_obj.opt_double_col == 2.34);
            CHECK(managed_obj.opt_str_col == "opt string");
            CHECK(managed_obj.opt_bool_col == true);
            CHECK(managed_obj.opt_enum_col == AllTypesObject::Enum::two);
            CHECK(managed_obj.opt_date_col == date);
            CHECK(managed_obj.opt_uuid_col == uuid);
            CHECK(managed_obj.opt_binary_col.value() == std::vector<uint8_t>({1}));
            CHECK(managed_obj.opt_object_id_col == object_id);

            CHECK(managed_obj.list_int_col[0] == 1);
            CHECK(managed_obj.list_double_col[0] == 1.23);
            CHECK(managed_obj.list_bool_col[0] == true);
            CHECK(managed_obj.list_str_col[0] == "bar");
            CHECK(managed_obj.list_uuid_col[0] == uuid);
            CHECK(managed_obj.list_date_col[0] == date);
            CHECK(managed_obj.list_uuid_col[0] == uuid);
            CHECK(managed_obj.list_mixed_col[0] == realm::mixed("mixed str"));
            auto other_obj = db.objects<realm::experimental::AllTypesObjectLink>()[1];
            CHECK(managed_obj.opt_obj_col->str_col == "link object");
            auto a = managed_obj.list_obj_col[0]->str_col.value();
            CHECK(managed_obj.list_obj_col[0]->str_col == "link object 2");
            CHECK(managed_obj.list_obj_col[0] == other_obj);
            auto b = managed_obj.list_embedded_obj_col[0]->str_col.value();
            auto c = managed_obj.opt_embedded_obj_col->str_col.value();

            CHECK(managed_obj.list_embedded_obj_col[0]->str_col == "embedded obj2");
            CHECK(managed_obj.opt_embedded_obj_col->str_col == "embedded obj1");
            CHECK(managed_obj.list_embedded_obj_col[0] == managed_obj.list_embedded_obj_col[0]);

            CHECK(managed_obj.map_int_col["foo"] == 1);
            CHECK(managed_obj.map_double_col["foo"] == 1.23);
            CHECK(managed_obj.map_bool_col["foo"] == true);
            CHECK(managed_obj.map_str_col["foo"] == "bar");
            CHECK(managed_obj.map_uuid_col["foo"] == uuid);
            CHECK(managed_obj.map_date_col["foo"] == date);
            CHECK(managed_obj.map_uuid_col["foo"] == uuid);
            CHECK(managed_obj.map_mixed_col["foo"] == "bar");
            CHECK(managed_obj.map_link_col["foo"]->str_col == "link object 3");
            auto other_obj2 = db.objects<realm::experimental::AllTypesObjectLink>()[2];
            auto other_obj3 = db.objects<realm::experimental::AllTypesObject>()[0];
            CHECK(managed_obj.map_link_col["foo"] == other_obj2);
            auto map_embedded_col = other_obj3.map_embedded_col["foo"];
            CHECK(managed_obj.map_embedded_col["foo"] == map_embedded_col);

            auto allTypeObjects = db.objects<AllTypesObject>();
            managed<AllTypesObjectLink> allTypeObjectLink = db.objects<AllTypesObjectLink>().where([](auto& o) {
                return o._id == 1;
            })[0];

            auto results_obj = allTypeObjects[0];
            CHECK(results_obj._id == (int64_t)123);
            CHECK(results_obj.double_col == 12.34);
            CHECK(results_obj.str_col == "foo");
            CHECK(results_obj.bool_col == true);
            CHECK(results_obj.enum_col == AllTypesObject::Enum::two);
            CHECK(results_obj.date_col == date);
            CHECK(results_obj.uuid_col == uuid);
            CHECK(results_obj.binary_col == std::vector<uint8_t>({1}));
            CHECK(results_obj.mixed_col == "mixed");
            CHECK(results_obj.object_id_col == object_id);

            CHECK(results_obj.opt_int_col == 2);
            CHECK(results_obj.opt_double_col == 2.34);
            CHECK(results_obj.opt_str_col == "opt string");
            CHECK(results_obj.opt_bool_col == true);
            CHECK(results_obj.opt_enum_col == AllTypesObject::Enum::two);
            CHECK(results_obj.opt_date_col == date);
            CHECK(results_obj.opt_uuid_col == uuid);
            CHECK(results_obj.opt_binary_col == std::vector<uint8_t>({1}));
            CHECK(results_obj.opt_object_id_col == object_id);

//            CHECK(results_obj.opt_obj_col == allTypeObjectLink);
//            CHECK(*results_obj.opt_embedded_obj_col == embedded_obj);

            CHECK(results_obj.list_int_col[0] == 1);
            CHECK(results_obj.list_double_col[0] == 1.23);
            CHECK(results_obj.list_bool_col[0] == true);
            CHECK(results_obj.list_str_col[0] == "bar");
            CHECK(results_obj.list_uuid_col[0] == uuid);
            CHECK(results_obj.list_date_col[0] == date);
            CHECK(results_obj.list_uuid_col[0] == uuid);
            CHECK(results_obj.list_mixed_col[0] == realm::mixed("mixed str"));
//            CHECK(results_obj.list_obj_col[0] == allTypeObjectLink);
//            CHECK(results_obj.list_embedded_obj_col[0] == embedded_obj);

            CHECK(results_obj.map_int_col["foo"] == 1);
            CHECK(results_obj.map_double_col["foo"] == 1.23);
            CHECK(results_obj.map_bool_col["foo"] == true);
            CHECK(results_obj.map_str_col["foo"] == "bar");
            CHECK(results_obj.map_uuid_col["foo"] == uuid);
            CHECK(results_obj.map_date_col["foo"] == date);
            CHECK(results_obj.map_uuid_col["foo"] == uuid);
            CHECK(results_obj.map_mixed_col["foo"] == realm::mixed("bar"));
//            CHECK(results_obj.map_link_col["foo"] == allTypeObjectLink);
//            CHECK(results_obj.map_embedded_col["foo"] == embedded_obj);
        }

        SECTION("object_notifications_beta") {
            experimental::db realm = experimental::open(path);

            auto date = std::chrono::time_point<std::chrono::system_clock>();
            auto uuid = realm::uuid();
            auto foo = AllTypesObject();
            auto o = AllTypesObjectLink();
            AllTypesObjectLink o2;
            AllTypesObjectEmbedded o3;
            AllTypesObjectEmbedded o4;

            auto managed_foo = realm.write([&foo, &realm]() {
                return realm.add(std::move(foo));
            });

            int run_count = 0;
            auto token = managed_foo.observe([&](auto change) {
                if (run_count == 1) {
                    CHECK(change.is_deleted);
                } else {
                    CHECK(change.property_changes.size() == 26);
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
                            auto val = std::get<AllTypesObject::my_mixed>(*prop_change.new_value);
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
                            auto obj = std::get<managed<AllTypesObjectLink*>>(*prop_change.new_value);
                            CHECK((obj->str_col) == "link object");
                        } else if (prop_change.name == "opt_embedded_obj_col" && prop_change.new_value) {
                            auto obj = std::get<managed<AllTypesObjectEmbedded*>>(*prop_change.new_value);
                            CHECK((obj->str_col) == "embedded obj");
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
                            auto obj = std::get<std::vector<AllTypesObjectLink*>>(*prop_change.new_value);
                            CHECK(obj.size() == 0);
                        } else if (prop_change.name == "list_embedded_obj_col" && prop_change.new_value) {
                            auto obj = std::get<std::vector<AllTypesObjectEmbedded*>>(*prop_change.new_value);
                            CHECK(obj.size() == 0);
                        }
                    }
                }

                run_count++;
            });

            realm.write([&]() {
                managed_foo.str_col = "foo";
                managed_foo.bool_col = true;
                managed_foo.enum_col = AllTypesObject::Enum::two;
                managed_foo.date_col = date;
                managed_foo.uuid_col = realm::uuid();
                managed_foo.binary_col.push_back(1);
                managed_foo.mixed_col = std::string("mixed");

                managed_foo.opt_int_col = 2;
                managed_foo.opt_double_col = 1.23;
                managed_foo.opt_str_col = "opt string";
                managed_foo.opt_bool_col = true;
                managed_foo.opt_enum_col = AllTypesObject::Enum::two;
                managed_foo.opt_date_col = date;
                managed_foo.opt_uuid_col = uuid;
                managed_foo.opt_binary_col = std::vector<uint8_t>({1});
                o._id = 1;
                o.str_col = "link object";
                managed_foo.opt_obj_col = &o;
                o3.str_col = "embedded obj";
                managed_foo.opt_embedded_obj_col = &o3;

                managed_foo.list_int_col.push_back(1);
                managed_foo.list_bool_col.push_back(true);
                managed_foo.list_str_col.push_back("bar");
                managed_foo.list_uuid_col.push_back(uuid);
                managed_foo.list_binary_col.push_back({1});
                managed_foo.list_date_col.push_back(date);
                managed_foo.list_mixed_col.push_back("mixed str");
                o2._id = 2;
                o2.str_col = "link object 2";
                managed_foo.list_obj_col.push_back(&o2);
                o4.str_col = "embedded obj 2";
                managed_foo.list_embedded_obj_col.push_back(&o4);
            });
            realm.refresh();
            realm.write([&managed_foo, &realm] {
                realm.remove(managed_foo);
            });
            realm.refresh();

            CHECK(run_count == 2);
        }

        SECTION("optional objects") {
            auto realm = db(std::move(config));

            auto foo = AllTypesObject();
            auto o = AllTypesObjectLink();
            o.str_col = "bar";
            AllTypesObjectEmbedded o2;
            o2.str_col = "embedded bar";
            foo.opt_obj_col = &o;
            foo.opt_embedded_obj_col = &o2;

            auto managed_obj = realm.write([&foo, &realm] {
                return realm.add(std::move(foo));
            });

//            realm.write([&] {
//                foo.opt_obj_col = std::nullopt;
//                foo.opt_embedded_obj_col = std::nullopt;
//            });
//
//            CHECK(foo.opt_obj_col == std::nullopt);
//            CHECK(foo.opt_embedded_obj_col == std::nullopt);
        }
    }
}