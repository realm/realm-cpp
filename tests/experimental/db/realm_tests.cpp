#include "../../main.hpp"
#include <cpprealm/experimental/sdk.hpp>
#include "test_objects.hpp"

using namespace realm;

namespace realm::experimental {
    TEST_CASE("object_initialization_beta") {
        realm_path path;
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
        //            .my_mixed_col = AllTypesObject::foo_mixed(o2),

        obj.opt_int_col = 2;
        obj.opt_double_col = 2.34;
        obj.opt_str_col = "opt string";
        obj.opt_bool_col = true;
        obj.opt_enum_col = AllTypesObject::Enum::two;
        obj.opt_date_col = date;
        obj.opt_uuid_col = uuid;
        obj.opt_object_id_col = object_id;
        obj.opt_binary_col = std::vector<uint8_t>({1});

        obj.opt_obj_col = link1,
        obj.opt_embedded_obj_col = embedded_obj1,

        obj.list_int_col = std::vector<int64_t>({1});
        obj.list_double_col = std::vector<double>({1.23});
        obj.list_bool_col = std::vector<bool>({true});
        obj.list_str_col = std::vector<std::string>({"bar"});
        obj.list_uuid_col = std::vector<realm::uuid>({uuid});
        obj.list_object_id_col = std::vector<realm::object_id>({object_id});
        obj.list_binary_col = std::vector<std::vector<uint8_t>>({{1}});
        obj.list_date_col = std::vector<std::chrono::time_point<std::chrono::system_clock>>({date});
        obj.list_mixed_col = std::vector<realm::mixed>({realm::mixed("mixed str")});
        obj.list_obj_col = std::vector<link<AllTypesObjectLink>>({std::move(link2)});
        obj.list_embedded_obj_col = std::vector<link<AllTypesObjectEmbedded>>({embedded_obj2});

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
        obj.map_link_col = std::map<std::string, std::optional<link<AllTypesObjectLink>>>({{"foo", link3}});
        obj.map_embedded_col = std::map<std::string, std::optional<link<AllTypesObjectEmbedded>>>({{"foo", embedded_obj3}});

        experimental::db db = experimental::open(path);

        auto managed_obj = db.write([&obj, &db]() {
            return db.add(std::move(obj));
        });

        CHECK(managed_obj._id == 123);
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
        auto a = managed_obj.list_obj_col[0].str_col.value();
        CHECK(managed_obj.list_obj_col[0].str_col == "link object 2");
        CHECK(managed_obj.list_obj_col[0] == other_obj);
        auto b = managed_obj.list_embedded_obj_col[0].str_col.value();
        auto c = managed_obj.opt_embedded_obj_col->str_col.value();

        CHECK(managed_obj.list_embedded_obj_col[0].str_col == "embedded obj2");
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
        CHECK((*managed_obj.map_link_col["foo"]).value()->str_col == "link object 3");
        auto other_obj2 = db.objects<realm::experimental::AllTypesObjectLink>()[2];
        auto other_obj3 = db.objects<realm::experimental::AllTypesObject>()[0];
        CHECK(*managed_obj.map_link_col["foo"] == other_obj2);
        std::optional<link<AllTypesObjectEmbedded>> map_embedded_col = *other_obj3.map_embedded_col["foo"];
        CHECK(*managed_obj.map_embedded_col["foo"] == map_embedded_col);

        auto allTypeObjects = db.objects<AllTypesObject>();

        auto results_obj = allTypeObjects[0];
        CHECK(results_obj._id == 123);
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

        //    CHECK(*results_obj.opt_obj_col == o);
        //    CHECK(*results_obj.opt_embedded_obj_col == embedded_obj);

        CHECK(results_obj.list_int_col[0] == 1);
        CHECK(results_obj.list_double_col[0] == 1.23);
        CHECK(results_obj.list_bool_col[0] == true);
        CHECK(results_obj.list_str_col[0] == "bar");
        CHECK(results_obj.list_uuid_col[0] == uuid);
        CHECK(results_obj.list_date_col[0] == date);
        CHECK(results_obj.list_uuid_col[0] == uuid);
        CHECK(results_obj.list_mixed_col[0] == realm::mixed("mixed str"));
        //    CHECK(results_obj.list_obj_col[0] == o);
        //    CHECK(results_obj.list_embedded_obj_col[0] == embedded_obj);

        CHECK(results_obj.map_int_col["foo"] == 1);
        CHECK(results_obj.map_double_col["foo"] == 1.23);
        CHECK(results_obj.map_bool_col["foo"] == true);
        CHECK(results_obj.map_str_col["foo"] == "bar");
        CHECK(results_obj.map_uuid_col["foo"] == uuid);
        CHECK(results_obj.map_date_col["foo"] == date);
        CHECK(results_obj.map_uuid_col["foo"] == uuid);
        CHECK(results_obj.map_mixed_col["foo"] == realm::mixed("bar"));
        //    CHECK(results_obj.map_link_col["foo"] == o);
        //    CHECK(results_obj.map_embedded_col["foo"] == embedded_obj);
    }
}

