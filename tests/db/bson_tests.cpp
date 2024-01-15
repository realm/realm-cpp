#include "../main.hpp"
#include "test_objects.hpp"

TEST_CASE("types", "[bson]") {
    SECTION("creates correct type") {
        auto bson = realm::bsoncxx();
        CHECK(bson.get_type() == realm::bsoncxx::type::b_null);

        bson = realm::bsoncxx((int32_t) 123);
        CHECK(bson.get_type() == realm::bsoncxx::type::b_int32);

        bson = realm::bsoncxx((int64_t) 123);
        CHECK(bson.get_type() == realm::bsoncxx::type::b_int64);

        bson = realm::bsoncxx(true);
        CHECK(bson.get_type() == realm::bsoncxx::type::b_bool);

        bson = realm::bsoncxx(123.456);
        CHECK(bson.get_type() == realm::bsoncxx::type::b_double);

        bson = realm::bsoncxx(std::string("foo"));
        CHECK(bson.get_type() == realm::bsoncxx::type::b_string);

        bson = realm::bsoncxx("foo");
        CHECK(bson.get_type() == realm::bsoncxx::type::b_string);

        bson = realm::bsoncxx(std::vector<uint8_t>({0, 0, 0, 0}));
        CHECK(bson.get_type() == realm::bsoncxx::type::b_binary);

        bson = realm::bsoncxx(realm::bsoncxx::timestamp(100, 0));
        CHECK(bson.get_type() == realm::bsoncxx::type::b_timestamp);

        bson = realm::bsoncxx(realm::bsoncxx::date(std::chrono::system_clock::now()));
        CHECK(bson.get_type() == realm::bsoncxx::type::b_datetime);

        bson = realm::bsoncxx(realm::object_id::generate());
        CHECK(bson.get_type() == realm::bsoncxx::type::b_objectId);

        bson = realm::bsoncxx(realm::decimal128(123.456));
        CHECK(bson.get_type() == realm::bsoncxx::type::b_decimal128);

        bson = realm::bsoncxx(realm::bsoncxx::regular_expression());
        CHECK(bson.get_type() == realm::bsoncxx::type::b_regular_expression);

        bson = realm::bsoncxx(realm::bsoncxx::max_key());
        CHECK(bson.get_type() == realm::bsoncxx::type::b_max_key);

        bson = realm::bsoncxx(realm::bsoncxx::min_key());
        CHECK(bson.get_type() == realm::bsoncxx::type::b_min_key);

        bson = realm::bsoncxx(realm::bsoncxx::document({{"name", "foo"}}));
        CHECK(bson.get_type() == realm::bsoncxx::type::b_document);

        bson = realm::bsoncxx(std::vector<realm::bsoncxx>({{"one", 123, true}}));
        CHECK(bson.get_type() == realm::bsoncxx::type::b_array);

        bson = realm::bsoncxx(realm::uuid());
        CHECK(bson.get_type() == realm::bsoncxx::type::b_uuid);
    }

    SECTION("value operator") {
    }

    SECTION("assignment operator") {
    }
}

TEST_CASE("document", "[bson]") {
    SECTION("assign subscript") {
        auto doc = realm::bsoncxx::document();
        doc["key"] = "value";
        auto str_val = doc["key"];
        CHECK(str_val == "value");
    }

    SECTION("iterate") {
        auto doc = realm::bsoncxx::document();
        doc["key"] = "value";
        doc["key2"] = (int64_t)123;
        auto str_val = doc["key"];
        CHECK(str_val == "value");
        auto int_val = doc["key2"];
        CHECK(int_val == (int64_t)123);
        size_t count = 0;
        for(auto [k, v] : doc) {
            count++;
            if (count == 1) {
                CHECK(k == "key");
                CHECK(v == "value");
            }
            else if (count == 2) {
                CHECK(k == "key2");
                CHECK(v.operator int64_t() == 123);
            }
        }
        CHECK(count == 2);
        CHECK(doc.empty() == false);
        CHECK(doc.size() == 2);

        doc.pop_back();
        CHECK(doc.size() == 1);
        doc.pop_back();
        CHECK(doc.empty() == true);
    }

    SECTION("comparison") {

    }
}

TEST_CASE("regular expression", "[bson]") {
}