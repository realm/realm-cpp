#include "../../main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("map", "[map]") {
    realm_path path;
    db_config config;
    config.set_path(path);
    SECTION("unmanaged_managed_map_get_set", "[mixed]") {
        auto obj = experimental::AllTypesObject();
        auto link = experimental::AllTypesObjectLink();
        link.str_col = "foo";
        auto embedded = experimental::AllTypesObjectEmbedded();
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
                {"a", experimental::AllTypesObject::Enum::one}
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
                {"a", &link}
        };
        obj.map_embedded_col = {
                {"a", &embedded}
        };
        CHECK(obj.map_int_col["a"] == 42);
        CHECK(obj.map_str_col["a"] == "foo");
        CHECK(obj.map_bool_col["a"] == true);
        CHECK(obj.map_enum_col["a"] == experimental::AllTypesObject::Enum::one);
        CHECK(obj.map_uuid_col["a"] == realm::uuid());
        CHECK(obj.map_binary_col["a"] == std::vector<uint8_t>{0, 1, 2});
        auto realm = experimental::db(std::move(config));
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        CHECK(managed_obj.map_int_col["a"] == 42);
        CHECK(managed_obj.map_str_col["a"] == "foo");
        CHECK(managed_obj.map_bool_col["a"] == true);
        CHECK(managed_obj.map_enum_col["a"] == experimental::AllTypesObject::Enum::one);
        CHECK(managed_obj.map_uuid_col["a"] == realm::uuid());
        CHECK(managed_obj.map_binary_col["a"] == std::vector<uint8_t>{0, 1, 2});
        auto managed_link = realm.objects<experimental::AllTypesObjectLink>()[0];
        auto res = realm.objects<experimental::AllTypesObject>()[0];
        CHECK(managed_obj.map_link_col["a"] == managed_link);
        CHECK(managed_obj.map_embedded_col["a"] == res.map_embedded_col["a"]);


        CHECK(managed_obj.map_link_col["a"]->str_col == "foo");
        CHECK(managed_obj.map_embedded_col["a"]->str_col == "bar");

        auto link2 = experimental::AllTypesObjectLink();
        link2.str_col = "foo";
        auto embedded2 = experimental::AllTypesObjectEmbedded();
        embedded2.str_col = "bar";

        realm.write([&managed_obj, &link2, &embedded2] {
            managed_obj.map_int_col["b"] = 84;
            managed_obj.map_str_col["b"] = "bar";
            managed_obj.map_bool_col["b"] = true;
            managed_obj.map_enum_col["b"] = experimental::AllTypesObject::Enum::two;
            managed_obj.map_uuid_col["b"] = realm::uuid();
            managed_obj.map_binary_col["b"] = std::vector<uint8_t>{3,4,5};
            managed_obj.map_link_col["b"] = &link2;
            managed_obj.map_embedded_col["b"] = &embedded2;
        });
        CHECK(managed_obj.map_int_col["a"] == 42);
        CHECK(managed_obj.map_int_col["b"] == 84);
        CHECK(managed_obj.map_str_col["a"] == "foo");
        CHECK(managed_obj.map_str_col["b"] == "bar");
        CHECK(managed_obj.map_bool_col["a"] == true);
        CHECK(managed_obj.map_bool_col["b"] == true);
        CHECK(managed_obj.map_enum_col["a"] == experimental::AllTypesObject::Enum::one);
        CHECK(managed_obj.map_enum_col["b"] == experimental::AllTypesObject::Enum::two);
        CHECK(managed_obj.map_uuid_col["a"] == realm::uuid());
        CHECK(managed_obj.map_uuid_col["b"] == realm::uuid());
        CHECK(managed_obj.map_binary_col["a"] == std::vector<uint8_t>{0, 1, 2});
        CHECK(managed_obj.map_binary_col["b"] == std::vector<uint8_t>{3, 4, 5});
//        CHECK(managed_obj.map_link_col["a"] == link);
//        CHECK(managed_obj.map_embedded_col["a"] == embedded);
//        CHECK(managed_obj.map_link_col["b"] == link2);
//        CHECK(managed_obj.map_embedded_col["b"] == embedded2);
        CHECK(managed_obj.map_link_col["b"]->str_col == "foo");
        CHECK(managed_obj.map_embedded_col["b"]->str_col == "bar");

        realm.write([&managed_obj] {
            managed_obj.map_link_col["b"] = nullptr;
            managed_obj.map_embedded_col["b"] = nullptr;
        });

        CHECK(obj.map_link_col["b"] == nullptr);
        CHECK(obj.map_embedded_col["b"] == nullptr);
    }

    SECTION("unmanaged_managed_map_iter", "[mixed]") {
        auto obj = experimental::AllTypesObject();
        obj.map_str_col = {
                {"a", "foo"},
                {"b", "bar"}
        };
        for (auto [k, v] : obj.map_str_col) {
            if (k == "a") CHECK(v == "foo");
            else if (k == "b") CHECK(v == "bar");
        }
        auto realm = experimental::db(std::move(config));
        auto managed_obj =realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        for (auto [k, v] : managed_obj.map_str_col) {
            if (k == "a") CHECK(v == "foo");
            else if (k == "b") CHECK(v == "bar");
        }
    }

    SECTION("managed_map_observe", "[mixed]") {
        auto obj = experimental::AllTypesObject();
        obj.map_str_col = {
                {"a", std::string("foo")}
        };
        auto realm = experimental::db(std::move(config));
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        std::promise<bool> p;
        auto token = managed_obj.map_str_col.observe([&p](auto&& change) {
            if (!change.modifications.empty() && !change.insertions.empty()) {
                CHECK(change.modifications[0] == "a");
                CHECK(change.insertions[0] == "b");
                p.set_value(true);
            }
        });
        realm.write([&managed_obj] {
            managed_obj.map_str_col["a"] = "baz";
            managed_obj.map_str_col["b"] = "food";
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
        auto obj = experimental::AllTypesObject();
        obj.map_str_col = {
                {"a", std::string("baz")},
                {"b", std::string("foo")}
        };

        auto realm = experimental::db(std::move(config));
        auto managed_obj = realm.write([&realm, &obj] {
            obj.map_str_col["c"] = "boo";
            obj.map_str_col["d"] = "boop";
            return realm.add(std::move(obj));
        });

        // find
        CHECK(managed_obj.map_str_col.find("baz") == managed_obj.map_str_col.end());
        CHECK(managed_obj.map_str_col.find("food") == managed_obj.map_str_col.end());
        // erase
        realm.write([&realm, &managed_obj] {
            // should not throw if key does not exist.
            managed_obj.map_str_col.erase("a");
            managed_obj.map_str_col.erase("c");
        });

        CHECK_THROWS(realm.write([&realm, &managed_obj] { managed_obj.map_str_col.erase("food"); }));

        CHECK(managed_obj.map_str_col.find("a") == managed_obj.map_str_col.end());
        CHECK(managed_obj.map_str_col.find("foo") == managed_obj.map_str_col.end());
        CHECK(managed_obj.map_str_col.find("c") == managed_obj.map_str_col.end());
        CHECK(managed_obj.map_str_col.find("d") != managed_obj.map_str_col.end());
    }

    SECTION("as_value") {
        auto obj = experimental::AllTypesObject();
        obj.map_str_col = {
                {"a", std::string("baz")},
                {"b", std::string("foo")}
        };

        auto realm = experimental::db(std::move(config));
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });

        std::map<std::string, std::string> as_values = managed_obj.map_str_col.value();
        CHECK(as_values == std::map<std::string, std::string>({{"a", std::string("baz")}, {"b", std::string("foo")}}));
    }
}
