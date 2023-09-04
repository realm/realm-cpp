#include "test_objects.hpp"
#include "../../main.hpp"

TEST_CASE("binary", "[binary]") {
    realm_path path;
    realm::db_config config;
    config.set_path(path);
    SECTION("push_back") {
        auto realm = realm::experimental::db(std::move(config));
        auto obj = realm::experimental::AllTypesObject();
        obj.binary_col.push_back(1);
        obj.binary_col.push_back(2);
        obj.binary_col.push_back(3);
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        CHECK(managed_obj.binary_col.size() == 3);
        realm.write([&managed_obj] {
            managed_obj.binary_col.push_back(4);
        });
        CHECK(managed_obj.binary_col.size() == 4);
        CHECK(managed_obj.binary_col[0] == 1);
        CHECK(managed_obj.binary_col[1] == 2);
        CHECK(managed_obj.binary_col[2] == 3);
        CHECK(managed_obj.binary_col[3] == 4);
        CHECK(managed_obj.binary_col == std::vector<uint8_t>({1, 2, 3, 4}));
        CHECK(managed_obj.binary_col != std::vector<uint8_t>({1, 2, 3}));
    }

    SECTION("assign") {
        auto realm = realm::experimental::db(std::move(config));
        auto obj = realm::experimental::AllTypesObject();
        obj.binary_col = std::vector<uint8_t>({1, 2, 3, 4});
        CHECK(obj.binary_col[0] == 1);
        CHECK(obj.binary_col[1] == 2);
        CHECK(obj.binary_col[2] == 3);
        CHECK(obj.binary_col[3] == 4);
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        CHECK(managed_obj.binary_col.size() == 4);
        CHECK(managed_obj.binary_col[0] == 1);
        CHECK(managed_obj.binary_col[1] == 2);
        CHECK(managed_obj.binary_col[2] == 3);
        CHECK(managed_obj.binary_col[3] == 4);
        realm.write([&managed_obj] {
            managed_obj.binary_col = std::vector<uint8_t>({5, 6, 7, 8});
        });
        CHECK(managed_obj.binary_col.size() == 4);
        CHECK(managed_obj.binary_col[0] == 5);
        CHECK(managed_obj.binary_col[1] == 6);
        CHECK(managed_obj.binary_col[2] == 7);
        CHECK(managed_obj.binary_col[3] == 8);
        CHECK(managed_obj.binary_col == std::vector<uint8_t>({5, 6, 7, 8}));
        CHECK(managed_obj.binary_col != std::vector<uint8_t>({5, 6, 7}));
    }

    SECTION("push_back_optional") {
        auto realm = realm::experimental::db(std::move(config));
        auto obj = realm::experimental::AllTypesObject();
        CHECK(obj.opt_binary_col == std::nullopt);
        obj.opt_binary_col = std::vector<uint8_t>();
        obj.opt_binary_col->push_back(1);
        obj.opt_binary_col->push_back(2);
        obj.opt_binary_col->push_back(3);
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        CHECK(managed_obj.opt_binary_col->size() == 3);
        realm.write([&managed_obj] {
            managed_obj.opt_binary_col->push_back(4);
        });
        CHECK(managed_obj.opt_binary_col->size() == 4);
        CHECK((*managed_obj.opt_binary_col)[0] == 1);
        CHECK((*managed_obj.opt_binary_col)[1] == 2);
        CHECK((*managed_obj.opt_binary_col)[2] == 3);
        CHECK((*managed_obj.opt_binary_col)[3] == 4);
        CHECK(managed_obj.opt_binary_col == std::vector<uint8_t>({1, 2, 3, 4}));
        CHECK(managed_obj.opt_binary_col != std::vector<uint8_t>({1, 2, 3}));
    }

    SECTION("assign_optional") {
        auto realm = realm::experimental::db(std::move(config));
        auto obj = realm::experimental::AllTypesObject();
        obj.opt_binary_col = std::vector<uint8_t>({1, 2, 3, 4});
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        CHECK(managed_obj.opt_binary_col->size() == 4);
        CHECK((*managed_obj.opt_binary_col)[0] == 1);
        CHECK((*managed_obj.opt_binary_col)[1] == 2);
        CHECK((*managed_obj.opt_binary_col)[2] == 3);
        CHECK((*managed_obj.opt_binary_col)[3] == 4);
        realm.write([&managed_obj] {
            managed_obj.opt_binary_col = std::vector<uint8_t>({5, 6, 7, 8});
        });
        CHECK(managed_obj.opt_binary_col->size() == 4);
        CHECK((*managed_obj.opt_binary_col)[0] == 5);
        CHECK((*managed_obj.opt_binary_col)[1] == 6);
        CHECK((*managed_obj.opt_binary_col)[2] == 7);
        CHECK((*managed_obj.opt_binary_col)[3] == 8);
        CHECK(managed_obj.opt_binary_col == std::vector<uint8_t>({5, 6, 7, 8}));
        CHECK(managed_obj.opt_binary_col != std::vector<uint8_t>({5, 6, 7}));

        realm.write([&managed_obj] {
            managed_obj.opt_binary_col = std::nullopt;
        });
        CHECK(managed_obj.opt_binary_col == std::nullopt);
        CHECK(managed_obj.opt_binary_col != std::vector<uint8_t>({5, 6, 7}));
    }

    SECTION("rbool_comparison") {
        auto realm = realm::experimental::db(std::move(config));
        auto obj = realm::experimental::AllTypesObject();
        obj.binary_col = std::vector<uint8_t>({1, 2, 3, 4});

        realm.write([&obj, &realm] {
            realm.add(std::move(obj));
        });

        auto results = realm.objects<realm::experimental::AllTypesObject>().where([](auto& o) {
            return o.binary_col == std::vector<uint8_t>({1, 2, 3, 4});
        });
        CHECK(results.size() == 1);
        CHECK(results[0].binary_col == std::vector<uint8_t>({1, 2, 3, 4}));
        CHECK(results[0].binary_col != std::vector<uint8_t>({1, 2, 3}));

        results = realm.objects<realm::experimental::AllTypesObject>().where([](auto& o) {
            return o.binary_col != std::vector<uint8_t>({1, 2, 3, 4});
        });
        CHECK(results.size() == 0);
    }

    SECTION("rbool_comparison_optional") {
        auto realm = realm::experimental::db(std::move(config));
        auto obj = realm::experimental::AllTypesObject();
        obj.opt_binary_col = std::vector<uint8_t>({1, 2, 3, 4});

        realm.write([&obj, &realm] {
            realm.add(std::move(obj));
        });

        auto results = realm.objects<realm::experimental::AllTypesObject>().where([](auto& o) {
            return o.opt_binary_col == std::vector<uint8_t>({1, 2, 3, 4});
        });
        CHECK(results.size() == 1);
        CHECK(results[0].opt_binary_col == std::vector<uint8_t>({1, 2, 3, 4}));
        CHECK(results[0].opt_binary_col != std::vector<uint8_t>({1, 2, 3}));

        results = realm.objects<realm::experimental::AllTypesObject>().where([](auto& o) {
            return o.opt_binary_col != std::nullopt;
        });
        CHECK(results.size() == 1);
    }

    SECTION("list_binary") {
        auto realm = realm::experimental::db(std::move(config));
        auto obj = realm::experimental::AllTypesObject();
        obj.list_binary_col.push_back({0});
        obj.list_binary_col.push_back({1});
        obj.list_binary_col.push_back({2});
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        realm.write([&managed_obj] {
            managed_obj.list_binary_col.push_back({3});
        });

        CHECK(managed_obj.list_binary_col.size() == 4);
        CHECK(managed_obj.list_binary_col[0] == std::vector<uint8_t>({ static_cast<uint8_t>(0) }));
        CHECK(managed_obj.list_binary_col[1] == std::vector<uint8_t>({ static_cast<uint8_t>(1) }));
        CHECK(managed_obj.list_binary_col[2] == std::vector<uint8_t>({ static_cast<uint8_t>(2) }));
        CHECK(managed_obj.list_binary_col[3] == std::vector<uint8_t>({ static_cast<uint8_t>(3) }));
    }

    SECTION("operator") {
        auto realm = realm::experimental::db(std::move(config));
        auto obj = realm::experimental::AllTypesObject();
        obj.binary_col.push_back(1);
        obj.binary_col.push_back(2);
        obj.binary_col.push_back(3);
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        CHECK(managed_obj.binary_col.size() == 3);
        realm.write([&managed_obj] {
            managed_obj.binary_col.push_back(4);
        });
        std::vector<uint8_t> vector = managed_obj.binary_col;
        CHECK(vector.size() == 4);
        CHECK(vector[0] == 1);
        CHECK(vector[1] == 2);
        CHECK(vector[2] == 3);
        CHECK(vector[3] == 4);
        CHECK(vector == std::vector<uint8_t>({1, 2, 3, 4}));
        CHECK(vector != std::vector<uint8_t>({1, 2, 3}));
    }

    SECTION("optional operator") {
        auto realm = realm::experimental::db(std::move(config));
        auto obj = realm::experimental::AllTypesObject();
        CHECK(obj.opt_binary_col == std::nullopt);
        obj.opt_binary_col = std::vector<uint8_t>();
        obj.opt_binary_col->push_back(1);
        obj.opt_binary_col->push_back(2);
        obj.opt_binary_col->push_back(3);
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        CHECK(managed_obj.opt_binary_col->size() == 3);
        realm.write([&managed_obj] {
            managed_obj.opt_binary_col->push_back(4);
        });

        std::optional<std::vector<uint8_t>> vector = managed_obj.opt_binary_col;
        CHECK(vector->size() == 4);
        CHECK((*vector)[0] == 1);
        CHECK((*vector)[1] == 2);
        CHECK((*vector)[2] == 3);
        CHECK((*vector)[3] == 4);
        CHECK(vector == std::vector<uint8_t>({1, 2, 3, 4}));
        CHECK(vector != std::vector<uint8_t>({1, 2, 3}));
    }
}
