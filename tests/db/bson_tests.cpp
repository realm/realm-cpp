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
        auto bson = realm::bsoncxx();
        CHECK(bson.get_type() == realm::bsoncxx::type::b_null);

        bson = realm::bsoncxx((int32_t) 123);
        CHECK(bson.operator int32_t() == (int32_t)123);

        bson = realm::bsoncxx((int64_t) 123);
        CHECK(bson.operator int64_t() == (int64_t)123);

        bson = realm::bsoncxx(true);
        CHECK(bson.operator bool() == true);

        bson = realm::bsoncxx(123.456);
        CHECK(bson.operator double() == 123.456);

        bson = realm::bsoncxx(std::string("foo"));
        CHECK(bson.operator std::string() == std::string("foo"));

        bson = realm::bsoncxx(std::vector<uint8_t>({0, 0, 0, 0}));
        CHECK(bson.operator std::vector<uint8_t>() == std::vector<uint8_t>({0, 0, 0, 0}));

        bson = realm::bsoncxx(realm::bsoncxx::timestamp(100, 0));
        CHECK(bson.operator realm::bsoncxx::timestamp() == realm::bsoncxx::timestamp(100, 0));

        auto d = std::chrono::system_clock::now();
        bson = realm::bsoncxx(realm::bsoncxx::date(d));
        CHECK(bson.operator realm::bsoncxx::date() == realm::bsoncxx::date(d));

        auto oid = realm::object_id::generate();
        bson = realm::bsoncxx(oid);
        CHECK(bson.operator realm::object_id() == oid);

        bson = realm::bsoncxx(realm::decimal128(123.456));
        CHECK(bson.operator realm::decimal128() == realm::decimal128(123.456));

        bson = realm::bsoncxx(realm::bsoncxx::regular_expression("[0-9]", "i"));
        CHECK(bson.operator realm::bsoncxx::regular_expression() == realm::bsoncxx::regular_expression("[0-9]", "i"));

        bson = realm::bsoncxx(realm::bsoncxx::max_key());
        CHECK(bson.operator realm::bsoncxx::max_key() == realm::bsoncxx::max_key());

        bson = realm::bsoncxx(realm::bsoncxx::min_key());
        CHECK(bson.operator realm::bsoncxx::min_key() == realm::bsoncxx::min_key());

        bson = realm::bsoncxx(realm::bsoncxx::document({{"name", "foo"}}));
        CHECK((bson.operator realm::bsoncxx::document() == realm::bsoncxx::document({{"name", "foo"}})));

        bson = realm::bsoncxx(std::vector<realm::bsoncxx>({{"one", 123, true}}));
        CHECK(bson.operator std::vector<realm::bsoncxx>() == std::vector<realm::bsoncxx>({{"one", 123, true}}));

        bson = realm::bsoncxx(realm::uuid());
        CHECK(bson.operator realm::uuid() == realm::uuid());
    }

    SECTION("comparision") {
        auto bson1 = realm::bsoncxx();
        auto bson2 = realm::bsoncxx();
        auto bson3 = realm::bsoncxx("foo");
        CHECK(bson1.get_type() == realm::bsoncxx::type::b_null);
        CHECK(bson2.get_type() == realm::bsoncxx::type::b_null);
        CHECK(bson1 == bson2);
        CHECK(bson1 != bson3);
    }
}

TEST_CASE("document", "[bson]") {
    SECTION("assign subscript") {
        auto doc = realm::bsoncxx::document({{}});
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
    }

    SECTION("comparison") {
        auto doc = realm::bsoncxx::document();
        doc["key"] = "value";

        auto doc2 = realm::bsoncxx::document();
        doc2["key"] = "value";
        CHECK((doc == doc2));

        doc2["key"] = 123;
        CHECK((doc != doc2));
    }
}

TEST_CASE("regular expression", "[bson]") {
    SECTION("options") {
        auto regex = realm::bsoncxx::regular_expression("[0-9]", "i");
        CHECK(regex.get_options() == realm::bsoncxx::regular_expression::option::ignore_case);
        CHECK(regex.get_pattern() == "[0-9]");

        regex = realm::bsoncxx::regular_expression("[0-9]", realm::bsoncxx::regular_expression::option::dotall);
        CHECK(regex.get_options() == realm::bsoncxx::regular_expression::option::dotall);
        CHECK(regex.get_pattern() == "[0-9]");
        realm::bsoncxx::regular_expression::option options;
        options = options | realm::bsoncxx::regular_expression::option::dotall;
        options = options | realm::bsoncxx::regular_expression::option::extended;
        options = options | realm::bsoncxx::regular_expression::option::ignore_case;
        options = options | realm::bsoncxx::regular_expression::option::multiline;
        regex = realm::bsoncxx::regular_expression("[0-9]", options);
        CHECK(regex.get_options() == options);
        CHECK(regex.get_pattern() == "[0-9]");
    }

    SECTION("comparison") {
        auto regex = realm::bsoncxx::regular_expression("[0-9]", "i");
        auto regex2 = realm::bsoncxx::regular_expression("[0-9]", "i");
        auto regex3 = realm::bsoncxx::regular_expression("[0-9]", "m");
        CHECK(regex == regex2);
        CHECK(regex != regex3);
    }
}

TEST_CASE("timestamps", "[bson]") {
    auto timestamp1 = realm::bsoncxx::timestamp(100, 200);
    auto timestamp2 = realm::bsoncxx::timestamp(100, 200);
    auto timestamp3 = realm::bsoncxx::timestamp(300, 200);

    CHECK(timestamp1 == timestamp2);
    CHECK(timestamp1 != timestamp3);

    CHECK(timestamp1.get_seconds() == 100);
    CHECK(timestamp1.get_increment() == 200);

    auto d = std::chrono::system_clock::now();

    auto date1 = realm::bsoncxx::date(d);
    auto date2 = realm::bsoncxx::date(d);
    auto date3 = realm::bsoncxx::date(std::chrono::system_clock::time_point());

    CHECK(date1 == date2);
    CHECK(date1 != date3);

    CHECK(date1.get_date() == d);
}