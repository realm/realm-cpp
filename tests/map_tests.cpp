#include "main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("map", "[map]") {
    realm_path path;
    db_config config;
    config.set_path(path);
    SECTION("unmanaged_managed_map_get_set", "[mixed]") {
        auto obj = AllTypesObject();
        auto link = AllTypesObjectLink();
        link.str_col = "foo";
        auto embedded = AllTypesObjectEmbedded();
        embedded.str_col = "bar";
        obj.map_int_col = {
            {"a", 42}
        };
        obj.map_str_col = {
            {"a", std::string("foo")}
        };
        obj.map_bool_col = {
            {"a", true}
        };
        obj.map_enum_col = {
                {"a", AllTypesObject::Enum::one}
        };
        obj.map_uuid_col = {
                {"a", realm::uuid()}
        };
        obj.map_binary_col = {
                {"a", {0, 1, 2}}
        };
        obj.map_date_col = {
                {"a", std::chrono::system_clock::now()}
        };
        obj.map_link_col = {
                {"a", link}
        };
        obj.map_embedded_col = {
                {"a", embedded}
        };
        CHECK(obj.map_int_col["a"] == 42);
        CHECK(obj.map_str_col["a"] == "foo");
        CHECK(obj.map_bool_col["a"] == true);
        CHECK(obj.map_enum_col["a"] == AllTypesObject::Enum::one);
        CHECK(obj.map_uuid_col["a"] == realm::uuid());
        CHECK(obj.map_binary_col["a"] == std::vector<uint8_t>{0, 1, 2});
        CHECK(obj.map_link_col["a"] == AllTypesObjectLink());
        CHECK(obj.map_embedded_col["a"] == AllTypesObjectEmbedded());
        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(std::move(config));
        realm.write([&realm, &obj] {
            realm.add(obj);
        });
        CHECK(obj.map_int_col["a"] == 42);
        CHECK(obj.map_str_col["a"] == "foo");
        CHECK(obj.map_bool_col["a"] == true);
        CHECK(obj.map_enum_col["a"] == AllTypesObject::Enum::one);
        CHECK(obj.map_uuid_col["a"] == realm::uuid());
        CHECK(obj.map_binary_col["a"] == std::vector<uint8_t>{0, 1, 2});
        CHECK(obj.map_link_col["a"] == link);
        CHECK(obj.map_embedded_col["a"] == embedded);
        CHECK((*obj.map_link_col["a"])->str_col == "foo");
        CHECK((*obj.map_embedded_col["a"])->str_col == "bar");

        auto link2 = AllTypesObjectLink();
        link2.str_col = "foo";
        auto embedded2 = AllTypesObjectEmbedded();
        embedded2.str_col = "bar";

        realm.write([&obj, &link2, &embedded2] {
            obj.map_int_col["b"] = 84;
            obj.map_str_col["b"] = "bar";
            obj.map_bool_col["b"] = true;
            obj.map_enum_col["b"] = AllTypesObject::Enum::two;
            obj.map_uuid_col["b"] = realm::uuid();
            obj.map_binary_col["b"] = std::vector<uint8_t>{3,4,5};
            obj.map_link_col["b"] = link2;
            obj.map_embedded_col["b"] = embedded2;
        });
        CHECK(obj.map_int_col["a"] == 42);
        CHECK(obj.map_int_col["b"] == 84);
        CHECK(obj.map_str_col["a"] == "foo");
        CHECK(obj.map_str_col["b"] == "bar");
        CHECK(obj.map_bool_col["a"] == true);
        CHECK(obj.map_bool_col["b"] == true);
        CHECK(obj.map_enum_col["a"] == AllTypesObject::Enum::one);
        CHECK(obj.map_enum_col["b"] == AllTypesObject::Enum::two);
        CHECK(obj.map_uuid_col["a"] == realm::uuid());
        CHECK(obj.map_uuid_col["b"] == realm::uuid());
        CHECK(obj.map_binary_col["a"] == std::vector<uint8_t>{0, 1, 2});
        CHECK(obj.map_binary_col["b"] == std::vector<uint8_t>{3, 4, 5});
        CHECK(obj.map_link_col["a"] == link);
        CHECK(obj.map_embedded_col["a"] == embedded);
        CHECK(obj.map_link_col["b"] == link2);
        CHECK(obj.map_embedded_col["b"] == embedded2);
        CHECK((*obj.map_link_col["b"])->str_col == "foo");
        CHECK((*obj.map_embedded_col["b"])->str_col == "bar");

        realm.write([&obj] {
            obj.map_link_col["b"] = std::nullopt;
            obj.map_embedded_col["b"] = std::nullopt;
        });

        std::optional<AllTypesObjectLink> v = *obj.map_link_col["b"];
        CHECK(obj.map_link_col["b"] == std::nullopt);
        CHECK(obj.map_embedded_col["b"] == std::nullopt);
    }

    SECTION("unmanaged_managed_map_iter", "[mixed]") {
        auto obj = AllTypesObject();
        obj.map_str_col = {
                {"a", "foo"},
                {"b", "bar"}
        };
        for (auto [k, v] : obj.map_str_col) {
            if (k == "a") CHECK(v == "foo");
            else if (k == "b") CHECK(v == "bar");
        }
        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(std::move(config));
        realm.write([&realm, &obj] {
            realm.add(obj);
        });
        for (auto [k, v] : obj.map_str_col) {
            if (k == "a") CHECK(v == "foo");
            else if (k == "b") CHECK(v == "bar");
        }
    }



    SECTION("managed_map_observe", "[mixed]") {
        auto obj = AllTypesObject();
        obj.map_str_col = {
                {"a", std::string("foo")}
        };
        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(std::move(config));
        realm.write([&realm, &obj] {
            realm.add(obj);
        });
        std::promise<bool> p;
        auto token = obj.map_str_col.observe([&p](auto&& change) {
            if (!change.modifications.empty() && !change.insertions.empty()) {
                CHECK(change.modifications[0] == 0);
                CHECK(change.insertions[0] == 1);
                p.set_value(true);
            }
        });
        realm.write([&obj] {
            obj.map_str_col["a"] = "baz";
            obj.map_str_col["b"] = "food";
        });
        realm.refresh();
        auto future = p.get_future();
        switch (future.wait_for(std::chrono::seconds(5))) {
            case std::future_status::ready:
                CHECK(future.get());
                break;
            default:
                FAIL("observation timed out");
        }
    }

    SECTION("find_erase") {
        auto obj = AllTypesObject();
        obj.map_str_col = {
                {"a", std::string("baz")},
                {"b", std::string("food")}
        };

        // find
        CHECK(obj.map_str_col.find("a") != obj.map_str_col.end());
        CHECK(obj.map_str_col.find("foo") == obj.map_str_col.end());
        // erase
        obj.map_str_col.erase("a");
        CHECK(obj.map_str_col.find("a") == obj.map_str_col.end());
        obj.map_str_col.erase("foo");
        CHECK(obj.map_str_col.find("foo") == obj.map_str_col.end());

        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(std::move(config));
        realm.write([&realm, &obj] {
            obj.map_str_col["c"] = "boo";
            obj.map_str_col["d"] = "boop";
            realm.add(obj);
        });

        // find
        CHECK(obj.map_str_col.find("baz") == obj.map_str_col.end());
        CHECK(obj.map_str_col.find("foo") == obj.map_str_col.end());
        // erase
        realm.write([&realm, &obj] {
            // should not throw if key does not exist.
            obj.map_str_col.erase("a");
            obj.map_str_col.erase("c");
            obj.map_str_col.erase("foo");
        });
        CHECK(obj.map_str_col.find("a") == obj.map_str_col.end());
        CHECK(obj.map_str_col.find("foo") == obj.map_str_col.end());
        CHECK(obj.map_str_col.find("c") == obj.map_str_col.end());
        CHECK(obj.map_str_col.find("d") != obj.map_str_col.end());
    }
}
