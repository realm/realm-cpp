#include "test_objects.hpp"
#include "main.hpp"

TEST_CASE("object_observe") {
    realm_path path;
    auto realm = realm::open<OneStrCol, TwoCols, TwoColsRev>({.path=path});

    auto test = [&] (auto &&object, size_t expected_index) {
        realm.write([&] () { realm.add(object); });

        using TestType = typename std::decay_t<decltype(object)>;
        using ValueType = decltype(realm::PropertyChange<TestType>::new_value);
        ValueType expected_old;
        std::string expected = typeid(TestType).name();

        int notification_count = 0;
        auto token = object.template observe<TestType>([&] (auto &&change) {
            ++notification_count;
            CHECK(!change.error);
            CHECK(!change.is_deleted);
            CHECK(change.property_changes.size() == 1);

            auto &&property = change.property_changes.front();
            CHECK(property.name == "str_col");

            // same thread notification doesn't have it
            CHECK(!property.old_value.has_value());

            CHECK(property.new_value.has_value());
            auto &&actual = property.new_value.value();
            CHECK(std::holds_alternative<std::string>(actual));
            CHECK(actual.index() == expected_index);
            CHECK(std::get<std::string>(actual) == expected);
        });

        realm.write([&] () { object.str_col = expected; });
        realm.write([] { });
        CHECK(notification_count == 1);

        expected_old = expected;
        expected += "42";
        realm.write([&] () { object.str_col = expected; });
        realm.write([] { });
        CHECK(notification_count == 2);

        token = object.template observe<TestType>([&] (auto &&change) {
            ++notification_count;
            CHECK(!change.error);
            CHECK(change.is_deleted);
            CHECK(change.property_changes.empty());
        });
        realm.write([&] { realm.remove(object); });
        realm.write([] { });
        CHECK(notification_count == 3);
    };

    SECTION("observe_one_str_col") {
        test(OneStrCol(), 0);
    }

    SECTION("observe_two_cols") {
        test(TwoCols(), 0);
    }

    SECTION("observe_two_cols_rev") {
        test(TwoColsRev(), 1);
    }
}
