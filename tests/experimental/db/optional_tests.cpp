#include "../../main.hpp"
#include "test_objects.hpp"

namespace realm::experimental {
    TEST_CASE("optional") {
        realm_path path;
        db_config config;
        config.set_path(path);
        SECTION("unmanaged_managed_optional_get_set") {
            AllTypesObject obj;
            obj.mixed_col = AllTypesObject::my_mixed();
            obj.opt_int_col = std::nullopt;
            obj.opt_double_col = std::nullopt;
            obj.opt_str_col = std::nullopt;
            obj.opt_bool_col = std::nullopt;
            obj.opt_enum_col = std::nullopt;
            obj.opt_date_col = std::nullopt;
            obj.opt_uuid_col = std::nullopt;
            obj.opt_object_id_col = std::nullopt;
            obj.opt_binary_col = std::nullopt;

            obj.list_mixed_col = std::vector<realm::mixed>({realm::mixed()});
            obj.map_mixed_col = std::map<std::string, realm::mixed>({{"foo", realm::mixed()}});

            experimental::db db = experimental::open(path);
            auto managed_obj = db.write([&obj, &db]() {
                return db.add(std::move(obj));
            });

            CHECK(managed_obj.opt_int_col == std::nullopt);
            CHECK(managed_obj.opt_double_col == std::nullopt);
            CHECK(managed_obj.opt_str_col == std::nullopt);
            CHECK(managed_obj.opt_bool_col == std::nullopt);
            CHECK(managed_obj.opt_enum_col == std::nullopt);
            CHECK(managed_obj.opt_date_col == std::nullopt);
            CHECK(managed_obj.opt_uuid_col == std::nullopt);
            CHECK(managed_obj.opt_binary_col.detach() == std::nullopt);
            CHECK(managed_obj.opt_object_id_col == std::nullopt);
            CHECK(managed_obj.mixed_col == AllTypesObject::my_mixed());
            CHECK(managed_obj.list_mixed_col[0] == realm::mixed());
            CHECK(managed_obj.map_mixed_col["foo"] == realm::mixed());

            auto allTypeObjects = db.objects<AllTypesObject>();
            auto results_obj = allTypeObjects[0];

            CHECK(results_obj.opt_int_col == std::nullopt);
            CHECK(results_obj.opt_double_col == std::nullopt);
            CHECK(results_obj.opt_str_col == std::nullopt);
            CHECK(results_obj.opt_bool_col == std::nullopt);
            CHECK(results_obj.opt_enum_col == std::nullopt);
            CHECK(results_obj.opt_date_col == std::nullopt);
            CHECK(results_obj.opt_uuid_col == std::nullopt);
            CHECK(results_obj.opt_binary_col == std::nullopt);
            CHECK(results_obj.opt_object_id_col == std::nullopt);
            CHECK(results_obj.list_mixed_col[0] == realm::mixed());
            CHECK(results_obj.map_mixed_col["foo"] == realm::mixed());
        }
    }
}
