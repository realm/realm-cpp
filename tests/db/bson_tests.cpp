#include "../main.hpp"
#include "test_objects.hpp"

TEST_CASE("bson", "[bson]") {
    SECTION("types") {
        auto bson = realm::bsoncxx();
        CHECK(bson.get_type() == realm::bsoncxx::type::b_null);

        bson = realm::bsoncxx((int32_t)123);
        CHECK(bson.get_type() == realm::bsoncxx::type::b_int32);

        bson = realm::bsoncxx((int64_t)123);
        CHECK(bson.get_type() == realm::bsoncxx::type::b_int64);

        bson = realm::bsoncxx(true);
        CHECK(bson.get_type() == realm::bsoncxx::type::b_bool);

        bson = realm::bsoncxx(123.456);
        CHECK(bson.get_type() == realm::bsoncxx::type::b_double);

        bson = realm::bsoncxx(std::string("foo"));
        CHECK(bson.get_type() == realm::bsoncxx::type::b_string);

        bson = realm::bsoncxx("foo");
        CHECK(bson.get_type() == realm::bsoncxx::type::b_string);

        bson = realm::bsoncxx(std::vector<uint8_t>({0,0,0,0}));
        CHECK(bson.get_type() == realm::bsoncxx::type::b_binary);

        bson = realm::bsoncxx(std::chrono::system_clock::now());
        CHECK(bson.get_type() == realm::bsoncxx::type::b_timestamp);

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
}