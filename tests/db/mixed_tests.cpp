#include "../main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("mixed", "[mixed]") {
    realm_path path;
    realm::db_config config;
    config.set_path(path);
    SECTION("unmanaged_managed_mixed_get_set", "[mixed]") {
        auto obj = AllTypesObject();
        auto obj2 = AllTypesObject();
        obj2._id = 2;

        obj.mixed_col = (int64_t)42;
        CHECK(obj.mixed_col == realm::mixed((int64_t)42));
        auto realm = db(std::move(config));
//        auto managed_obj = realm.write([&realm, &obj] {
//            return realm.add(std::move(obj));
//        });
//        CHECK(managed_obj.mixed_col == static_cast<int64_t>(42));
//
//
//        realm.write([&managed_obj] {
//            managed_obj.mixed_col = std::string("hello world");
//        });
//        CHECK(managed_obj.mixed_col == std::string("hello world"));
//        realm.write([&managed_obj] {
//            managed_obj.mixed_col = true;
//        });
//        CHECK(managed_obj.mixed_col == true);
//        auto time_point = std::chrono::time_point<std::chrono::system_clock>();
//        realm.write([&managed_obj, &time_point] {
//            managed_obj.mixed_col = time_point;
//        });
//        CHECK(managed_obj.mixed_col == time_point);
//        auto bin = std::vector<uint8_t>{0,1,2};
//        realm.write([&managed_obj, &bin] {
//            managed_obj.mixed_col = bin;
//        });
//        CHECK(managed_obj.mixed_col == bin);
//        realm.write([&managed_obj] {
//            managed_obj.mixed_col = 42.42;
//        });
//        CHECK(managed_obj.mixed_col == 42.42);
//        auto u = uuid();
//        realm.write([&managed_obj, u] {
//            managed_obj.mixed_col = u;
//        });
//        CHECK(managed_obj.mixed_col == u);

//        auto obj2 = AllTypesObject();
//
//        auto xx = realm::mixed(obj2);

//        auto x = realm::mixed(managed_obj);


        obj.mixed_col = std::map<std::string, realm::mixed>({
                                                                {"foo", realm::mixed("bar")},
//                                                                {"foo2", realm::mixed(std::map<std::string, realm::mixed>({ {"nested", 123.456} }))},
//                                                                {"bar", realm::mixed(std::map<std::string, realm::mixed>({ {"nested", realm::mixed(obj2)} }))}
                                                            });

        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });

        auto managed_obj2 = realm.write([&realm, &obj2] {
            return realm.add(std::move(obj2));
        });

        std::map<std::string, realm::mixed> d = realm::mixed_cast<std::map<std::string, realm::mixed>>(*managed_obj.mixed_col);
        realm::mixed::map e = realm::mixed_cast<realm::mixed::map>(*managed_obj.mixed_col);

        auto xxx = realm::mixed_cast<std::string>(*e["foo"]);

        realm.write([&] {
            e["foo"] = realm::mixed("dog");
        });

        realm.write([&] {
            e["foo"] = std::map<std::string, mixed>({{"bar", mixed("dog")}});
        });

        realm.write([&] {
            auto x = realm::mixed_cast<realm::mixed::map>(*e["foo"]);

            x = std::map<std::string, mixed>({{"bar", mixed("dog")}});
        });


        auto x = realm::mixed_cast<std::string>(d["foo"]);


        realm.write([&] {
            e["bar"] = realm::mixed(managed_obj2);
        });

        realm.write([&] {
            AllTypesObjectLink link;
            e["link"] = realm::mixed(link);
        });

        std::string a = realm::mixed_cast<std::string>(d["foo"]);
//        managed<AllTypesObject> a = d["bar"]->get<managed<AllTypesObject>>();




    }

    SECTION("holds_alternative") {
        auto obj = AllTypesObject();
        obj.mixed_col = (int64_t)42;
        CHECK(obj.mixed_col == realm::mixed((int64_t)42));
        auto realm = db(std::move(config));
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        bool result = realm::holds_alternative<int64_t>(managed_obj.mixed_col);
        CHECK(result);
        result = realm::holds_alternative<bool>(managed_obj.mixed_col);
        CHECK_FALSE(result);

        realm.write([&realm, &managed_obj] {
            managed_obj.mixed_col = std::string("foo");
        });
        result = realm::holds_alternative<std::string>(managed_obj.mixed_col);
        CHECK(result);

        realm.write([&realm, &managed_obj] {
            managed_obj.mixed_col = std::vector<uint8_t>({1,1,1,1});
        });
        result = realm::holds_alternative<std::vector<uint8_t>>(managed_obj.mixed_col);
        CHECK(result);

        realm.write([&realm, &managed_obj] {
            managed_obj.mixed_col = realm::mixed((int64_t)1234);
        });
        result = realm::holds_alternative<int64_t>(managed_obj.mixed_col);
        CHECK(result);

        realm.write([&realm, &managed_obj] {
            managed_obj.mixed_col = std::chrono::time_point<std::chrono::system_clock>();
        });
        result = realm::holds_alternative<std::chrono::time_point<std::chrono::system_clock>>(managed_obj.mixed_col);
        CHECK(result);

        realm.write([&realm, &managed_obj] {
            managed_obj.mixed_col = 123.456;
        });
        result = realm::holds_alternative<double>(managed_obj.mixed_col);
        CHECK(result);

        realm.write([&realm, &managed_obj] {
            managed_obj.mixed_col = realm::decimal128("123.456");
        });
        result = realm::holds_alternative<realm::decimal128>(managed_obj.mixed_col);
        CHECK(result);

        realm.write([&realm, &managed_obj] {
            managed_obj.mixed_col = realm::object_id();
        });
        result = realm::holds_alternative<realm::object_id>(managed_obj.mixed_col);
        CHECK(result);

        realm.write([&realm, &managed_obj] {
            managed_obj.mixed_col = realm::uuid();
        });
        result = realm::holds_alternative<realm::uuid>(managed_obj.mixed_col);
        CHECK(result);

        realm.write([&realm, &managed_obj] {
            managed_obj.mixed_col = std::monostate();
        });
        result = realm::holds_alternative<std::monostate>(managed_obj.mixed_col);
        CHECK(result);
    }

    SECTION("links") {
        auto obj = AllTypesObject();
        obj.mixed_col = (int64_t)42;
        CHECK(obj.mixed_col == realm::mixed((int64_t)42));
        auto realm = db(std::move(config));
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        CHECK_FALSE(managed_obj.mixed_col.has_link());

        auto link = AllTypesObjectLink();
        link._id = 0;
        link.str_col = "foo";

        realm.write([&] {
            managed_obj.mixed_col.set_link(std::move(link));
        });
        CHECK(managed_obj.mixed_col.has_link());

        auto mixed_link = managed_obj.mixed_col.get_stored_link<AllTypesObjectLink*>();
        CHECK(mixed_link->str_col == "foo");

        bool result = realm::holds_alternative<AllTypesObjectLink*>(managed_obj.mixed_col);
        CHECK(result);

        result = realm::holds_alternative<StringObject*>(managed_obj.mixed_col);
        CHECK_FALSE(result);

        auto link2 = AllTypesObjectLink();
        link2._id = 0;
        link2.str_col = "bar";

        auto managed_link = realm.write([&] {
            return realm.add(std::move(link2));
        });

        realm.write([&] {
            managed_obj.mixed_col.set_link(managed_link);
        });
        mixed_link = managed_obj.mixed_col.get_stored_link<AllTypesObjectLink*>();
        CHECK(mixed_link->str_col == "bar");

        CHECK_THROWS(managed_obj.mixed_col.get_stored_link<StringObject*>());
    }
}