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
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        auto managed_obj2 = realm.write([&realm, &obj2] {
            return realm.add(std::move(obj2));
        });
        CHECK(managed_obj.mixed_col == static_cast<int64_t>(42));

        realm.write([&managed_obj] {
            managed_obj.mixed_col = std::string("hello world");
        });
        CHECK(managed_obj.mixed_col == std::string("hello world"));
        realm.write([&managed_obj] {
            managed_obj.mixed_col = true;
        });
        CHECK(managed_obj.mixed_col == true);
        auto time_point = std::chrono::time_point<std::chrono::system_clock>();
        realm.write([&managed_obj, &time_point] {
            managed_obj.mixed_col = time_point;
        });
        CHECK(managed_obj.mixed_col == time_point);
        auto bin = std::vector<uint8_t>{0,1,2};
        realm.write([&managed_obj, &bin] {
            managed_obj.mixed_col = bin;
        });
        CHECK(managed_obj.mixed_col == bin);
        realm.write([&managed_obj] {
            managed_obj.mixed_col = 42.42;
        });
        CHECK(managed_obj.mixed_col == 42.42);
        auto u = uuid();
        realm.write([&managed_obj, u] {
            managed_obj.mixed_col = u;
        });
        CHECK(managed_obj.mixed_col == u);

        // Test plan
        // - CRUD primitive

        // Primitives
        auto null_val = realm::mixed();
        CHECK(mixed_cast<std::monostate>(null_val) == std::monostate());

        auto int_val = realm::mixed((int64_t)123);
        CHECK(mixed_cast<int64_t>(int_val) == (int64_t)123);

        auto bool_val = realm::mixed(true);
        CHECK(mixed_cast<bool>(bool_val) == true);

        auto str_val = realm::mixed(std::string("foo"));
        CHECK(mixed_cast<std::string>(str_val) == std::string("foo"));

        auto char_val = realm::mixed("foo_char");
        CHECK(mixed_cast<std::string>(char_val) == std::string("foo_char"));

        auto double_val = realm::mixed(123.456);
        CHECK(mixed_cast<double>(double_val) == 123.456);

        auto binary_val = realm::mixed(std::vector<uint8_t>({1,2,3,4}));
        CHECK(mixed_cast<std::vector<uint8_t>>(binary_val) == std::vector<uint8_t>({1,2,3,4}));

        auto time = std::chrono::system_clock::now();
        auto time_val = realm::mixed(time);
        CHECK(mixed_cast<std::chrono::time_point<std::chrono::system_clock>>(time_val) == time);

        auto uuid = realm::uuid();
        auto uuid_val = realm::mixed(uuid);
        CHECK(mixed_cast<realm::uuid>(uuid_val) == uuid);

        auto object_id = realm::object_id::generate();
        auto object_id_val = realm::mixed(object_id);
        CHECK(mixed_cast<realm::object_id>(object_id_val) == object_id);

        auto decimal_val = realm::mixed(realm::decimal128(12.34));
        CHECK(mixed_cast<realm::decimal128>(decimal_val) == realm::decimal128(12.34));

        // Collections
        auto vector_val = realm::mixed(std::vector<realm::mixed>({int_val, bool_val}));
        auto from_vector_val = realm::mixed(vector_val);
        CHECK(mixed_cast<realm::mixed::vector>(vector_val).operator std::vector<realm::mixed>() == std::vector<realm::mixed>({int_val, bool_val}));
//        CHECK(mixed_cast<realm::mixed::vector>(vector_val) == mixed_cast<realm::mixed::vector>(from_vector_val));
//        CHECK(mixed_cast<realm::mixed::vector>(vector_val) != mixed_cast<realm::mixed::vector>(from_vector_val));

        CHECK(mixed_cast<realm::mixed::vector>(from_vector_val).operator std::vector<realm::mixed>() == std::vector<realm::mixed>({int_val, bool_val}));
        CHECK_FALSE(mixed_cast<realm::mixed::vector>(from_vector_val).operator std::vector<realm::mixed>() != std::vector<realm::mixed>({int_val, bool_val}));

        auto map = std::map<std::string, realm::mixed>({
                                                           {"foo", str_val},
                                                           {"bar", null_val}
                                                       });
        auto map_val = realm::mixed(map);
        auto from_mixed_val = realm::mixed(map_val);
        CHECK(mixed_cast<realm::mixed::map>(from_mixed_val).operator std::map<std::string, realm::mixed>() == map);
        CHECK_FALSE(mixed_cast<realm::mixed::map>(from_mixed_val).operator std::map<std::string, realm::mixed>() != map);
//        CHECK(mixed_cast<realm::mixed::map>(map_val) == mixed_cast<realm::mixed::vector>(from_mixed_val));
//        CHECK(mixed_cast<realm::mixed::map>(map_val) != mixed_cast<realm::mixed::vector>(from_mixed_val));



        // Make them managed
        realm.write([&] {
            managed_obj.mixed_col = null_val;
        });
        CHECK(mixed_cast<std::monostate>(*managed_obj.mixed_col) == std::monostate());

        realm.write([&] {
            managed_obj.mixed_col = int_val;
        });
        CHECK(mixed_cast<int64_t>(*managed_obj.mixed_col) == (int64_t)123);

        realm.write([&] {
            managed_obj.mixed_col = bool_val;
        });
        CHECK(mixed_cast<bool>(*managed_obj.mixed_col) == true);

        realm.write([&] {
            managed_obj.mixed_col = str_val;
        });
        CHECK(mixed_cast<std::string>(*managed_obj.mixed_col) == std::string("foo"));

        realm.write([&] {
            managed_obj.mixed_col = char_val;
        });
        CHECK(mixed_cast<std::string>(*managed_obj.mixed_col) == std::string("foo_char"));

        CHECK(mixed_cast<double>(double_val) == 123.456);

        realm.write([&] {
            managed_obj.mixed_col = double_val;
        });
        CHECK(mixed_cast<double>(*managed_obj.mixed_col) == 123.456);

        realm.write([&] {
            managed_obj.mixed_col = binary_val;
        });
        CHECK(mixed_cast<std::vector<uint8_t>>(*managed_obj.mixed_col) == std::vector<uint8_t>({1,2,3,4}));

        realm.write([&] {
            managed_obj.mixed_col = time_val;
        });
        CHECK(mixed_cast<std::chrono::time_point<std::chrono::system_clock>>(*managed_obj.mixed_col) == time);

        realm.write([&] {
            managed_obj.mixed_col = uuid_val;
        });
        CHECK(mixed_cast<realm::uuid>(*managed_obj.mixed_col) == uuid);

        realm.write([&] {
            managed_obj.mixed_col = object_id_val;
        });
        CHECK(mixed_cast<realm::object_id>(*managed_obj.mixed_col) == object_id);

        realm.write([&] {
            managed_obj.mixed_col = decimal_val;
        });
        CHECK(mixed_cast<realm::decimal128>(*managed_obj.mixed_col) == realm::decimal128(12.34));

        realm.write([&] {
            managed_obj.mixed_col = vector_val;
        });
        CHECK(mixed_cast<realm::mixed::vector>(*managed_obj.mixed_col).operator std::vector<realm::mixed>() == std::vector<realm::mixed>({int_val, bool_val}));
        //CHECK(mixed_cast<realm::mixed::vector>(*managed_obj.mixed_col).operator std::vector<realm::mixed>() == mixed_cast<realm::mixed::vector>(from_vector_val));

        realm.write([&] {
            managed_obj.mixed_col = map_val;
        });
        CHECK(mixed_cast<realm::mixed::map>(*managed_obj.mixed_col).operator std::map<std::string, realm::mixed>() == map);
        CHECK_FALSE(mixed_cast<realm::mixed::map>(from_mixed_val).operator std::map<std::string, realm::mixed>() != map);
        //CHECK(mixed_cast<realm::mixed::map>(*managed_obj.mixed_col) == realm::mixed::map(map_val));

        // - CRUD managed dictionary (with managed object)

        auto map2 = std::map<std::string, realm::mixed>({
                                                            {"foo", realm::mixed("some_val")},
                                                            {"baz", realm::mixed(managed_obj2)}
                                                        });
        realm.write([&] {
            managed_obj2.mixed_col = realm::mixed(map);
            managed_obj.mixed_col = realm::mixed(map2);
        });
        CHECK(mixed_cast<realm::mixed::map>(*managed_obj.mixed_col).operator std::map<std::string, realm::mixed>() == map2);
        auto nested_obj = mixed_cast<managed<AllTypesObject>>(*mixed_cast<realm::mixed::map>(*managed_obj.mixed_col)["baz"], realm);
        auto nested_map = mixed_cast<realm::mixed::map>(*nested_obj.mixed_col);
        CHECK(nested_map.operator std::map<std::string, realm::mixed>() == map);


        // Test when key doesnt exist
        mixed_cast<realm::mixed::map>(*managed_obj.mixed_col).find("NA");
        auto managed_map = mixed_cast<realm::mixed::map>(*managed_obj.mixed_col);
        auto unmanaged_map = mixed_cast<realm::mixed::map>(realm::mixed(map2));
        CHECK(managed_map.find("NA") == managed_map.end());
        CHECK(unmanaged_map.find("NA") == managed_map.end());
        CHECK(managed_map.find("baz") != managed_map.end());
        CHECK(unmanaged_map.find("foo") != managed_map.end());

        CHECK(mixed_cast<std::string>(managed_map.find("foo").operator*().second.operator*()) == "some_val");

        // - CRUD add unmanaged object
        // - CRUD modify managed object
        // - Delete a key and object from dictionary
        // Managed object with mixed that stored an object link that has a mixed that stores a collection of objects
        // vector, assign at index, remove, push_back
        // mixed_cast wrong type should throw
        // introspection

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