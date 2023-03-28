#include "main.hpp"
#include "test_objects.hpp"
#include <cpprealm/notifications.hpp>

using namespace realm;

template<typename Cls, typename ...Ts>
inline void validate_equals(db<Ts...>& realm, u_int equal_count, std::function<rbool(Cls&)> expr) {
    auto results = realm.template objects<Cls>().where([expr](auto& obj) {
        return expr(obj);
    });
    CHECK(results.size() == equal_count);
}
#define query_results_size(Cls, fn)  \
    realm.template objects<Cls>().where(fn).size()

TEST_CASE("query") {
    realm_path path;
    db_config config;
    config.set_path(path);
    SECTION("tsq_basic_comparison", "[query]") {
        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded, Dog>(std::move(config));
        auto date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        auto create_obj = [&]() {
            auto obj = AllTypesObject{
                    ._id = 123,
                    .str_col = "foo bar",
                    .enum_col = AllTypesObject::Enum::two,
                    .date_col = std::chrono::system_clock::from_time_t(date),
                    .uuid_col = realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002")
            };

            obj.binary_col.push_back(0);
            obj.binary_col.push_back(1);
            obj.binary_col.push_back(2);
            obj.binary_col.push_back(3);

            realm.write([&]() {
                realm.add(obj);
            });

            return obj;
        };

        auto obj = create_obj();
        auto obj2 = create_obj();

        // With literal as RHS.
        CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id == 123; }) == 1);
        CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id != 123; }) == 0);
        CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.str_col == "foo bar"; }) == 1);
        CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.str_col != "foo bar"; }) == 0);
        CHECK(query_results_size(AllTypesObject, [](auto &o) {
            return o.binary_col == std::vector<uint8_t>({0, 1, 2, 3});
        }) == 1);
        CHECK(query_results_size(AllTypesObject,
                                 [](auto &o) { return o.binary_col != std::vector<uint8_t>({0, 1, 2, 3}); }) == 0);
        CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col == AllTypesObject::Enum::two; }) == 1);
        CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col != AllTypesObject::Enum::two; }) == 0);
        CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
            return o.date_col == std::chrono::system_clock::from_time_t(date);
        }) == 1);
        CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
            return o.date_col != std::chrono::system_clock::from_time_t(date);
        }) == 0);
        CHECK(query_results_size(AllTypesObject, [](auto &o) {
            return o.uuid_col == realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002");
        }) == 1);
        CHECK(query_results_size(AllTypesObject, [](auto &o) {
            return o.uuid_col != realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002");
        }) == 0);

        // With col as RHS.
        CHECK(query_results_size(AllTypesObject, [&obj2](auto &o) { return o._id == obj2._id; }) == 1);
        CHECK(query_results_size(AllTypesObject, [&obj2](auto &o) { return o._id != obj2._id; }) == 0);
        CHECK(query_results_size(AllTypesObject, [&obj2](auto &o) { return o.str_col == obj2.str_col; }) == 1);
        CHECK(query_results_size(AllTypesObject, [&obj2](auto &o) { return o.str_col != obj2.str_col; }) == 0);
        CHECK(query_results_size(AllTypesObject, [&obj2](auto &o) { return o.binary_col == obj2.binary_col; }) == 1);
        CHECK(query_results_size(AllTypesObject, [&obj2](auto &o) { return o.binary_col != obj2.binary_col; }) == 0);
        CHECK(query_results_size(AllTypesObject, [&obj2](auto &o) { return o.enum_col == obj2.enum_col; }) == 1);
        CHECK(query_results_size(AllTypesObject, [&obj2](auto &o) { return o.enum_col != obj2.enum_col; }) == 0);
        CHECK(query_results_size(AllTypesObject, [&obj2](auto &o) { return o.date_col == obj2.date_col; }) == 1);
        CHECK(query_results_size(AllTypesObject, [&obj2](auto &o) { return o.date_col != obj2.date_col; }) == 0);
        CHECK(query_results_size(AllTypesObject, [&obj2](auto &o) { return o.uuid_col == obj2.uuid_col; }) == 1);
        CHECK(query_results_size(AllTypesObject, [&obj2](auto &o) { return o.uuid_col != obj2.uuid_col; }) == 0);
    }

    SECTION("tsq_greater_less_than", "[query]") {
        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded, Dog>(std::move(config));
        auto date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        auto create_obj = [&]() {
            auto obj = AllTypesObject{
                    ._id = 123,
                    .str_col = "foo bar",
                    .enum_col = AllTypesObject::Enum::two,
                    .date_col = std::chrono::system_clock::from_time_t(date),
                    .uuid_col = realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002")
            };

            obj.binary_col.push_back(0);
            obj.binary_col.push_back(1);
            obj.binary_col.push_back(2);
            obj.binary_col.push_back(3);

            realm.write([&]() {
                realm.add(obj);
            });

            return obj;
        };

        auto obj = create_obj();
        auto obj2 = create_obj();

        // With literal as RHS.
        validate_equals<AllTypesObject>(realm, 0U, [](auto &o) { return o._id > 123; });
        validate_equals<AllTypesObject>(realm, 1U, [](auto &o) { return o._id >= 123; });
        validate_equals<AllTypesObject>(realm, 0U, [](auto &o) { return o._id < 123; });
        validate_equals<AllTypesObject>(realm, 1U, [](auto &o) { return o._id <= 123; });
        validate_equals<AllTypesObject>(realm, 0U, [](auto &o) { return o.enum_col > AllTypesObject::Enum::two; });
        validate_equals<AllTypesObject>(realm, 1U, [](auto &o) { return o.enum_col >= AllTypesObject::Enum::two; });
        validate_equals<AllTypesObject>(realm, 0U, [](auto &o) { return o.enum_col < AllTypesObject::Enum::two; });
        validate_equals<AllTypesObject>(realm, 1U, [](auto &o) { return o.enum_col <= AllTypesObject::Enum::two; });
        validate_equals<AllTypesObject>(realm, 0U, [&date](auto &o) {
            return o.date_col > std::chrono::system_clock::from_time_t(date);
        });
        validate_equals<AllTypesObject>(realm, 1U, [&date](auto &o) {
            return o.date_col >= std::chrono::system_clock::from_time_t(date);
        });
        validate_equals<AllTypesObject>(realm, 0U, [&date](auto &o) {
            return o.date_col < std::chrono::system_clock::from_time_t(date);
        });
        validate_equals<AllTypesObject>(realm, 1U, [&date](auto &o) {
            return o.date_col <= std::chrono::system_clock::from_time_t(date);
        });

        // With col as RHS.
        validate_equals<AllTypesObject>(realm, 0U, [&obj2](auto &o) { return o._id > obj2._id; });
        validate_equals<AllTypesObject>(realm, 1U, [&obj2](auto &o) { return o._id >= obj2._id; });
        validate_equals<AllTypesObject>(realm, 0U, [&obj2](auto &o) { return o._id < obj2._id; });
        validate_equals<AllTypesObject>(realm, 1U, [&obj2](auto &o) { return o._id <= obj2._id; });
        validate_equals<AllTypesObject>(realm, 0U, [&obj2](auto &o) { return o.enum_col > obj2.enum_col; });
        validate_equals<AllTypesObject>(realm, 1U, [&obj2](auto &o) { return o.enum_col >= obj2.enum_col; });
        validate_equals<AllTypesObject>(realm, 0U, [&obj2](auto &o) { return o.enum_col < obj2.enum_col; });
        validate_equals<AllTypesObject>(realm, 1U, [&obj2](auto &o) { return o.enum_col <= obj2.enum_col; });
        validate_equals<AllTypesObject>(realm, 0U, [&obj2](auto &o) { return o.date_col > obj2.date_col; });
        validate_equals<AllTypesObject>(realm, 1U, [&obj2](auto &o) { return o.date_col >= obj2.date_col; });
        validate_equals<AllTypesObject>(realm, 0U, [&obj2](auto &o) { return o.date_col < obj2.date_col; });
        validate_equals<AllTypesObject>(realm, 1U, [&obj2](auto &o) { return o.date_col <= obj2.date_col; });
    }

    SECTION("tsq_compound", "[query]") {
        auto realm = realm::open<Person, Dog>(std::move(config));

        auto person = Person{.name = "John", .age = 42};
        realm.write([&realm, &person]() {
            realm.add(person);
        });

        auto results = realm.objects<Person>().where([](auto &person) {
            return person.age == 42 && person.name != "John";
        });
        CHECK(results.size() == 0);

        results = realm.objects<Person>().where([](auto &person) {
            return person.age == 42 && person.name.contains("oh");
        });
        CHECK(results.size() == 1);

        results = realm.objects<Person>().where([](auto &person) {
            return person.age == 100 || person.name == "John";
        });
        CHECK(results.size() == 1);

        results = realm.objects<Person>().where([](auto &person) {
            return person.age == 100 || person.name.contains("oh") && person.name != "Foo";
        });
        CHECK(results.size() == 1);
    }
    SECTION("equality") {
        auto realm = realm::open<Person, Dog>(std::move(config));

        auto person = Person { .name = "John", .age = 42 };
        realm.write([&realm, &person](){
            realm.add(person);
        });

        auto results = realm.objects<Person>().where("age > $0", {42});
        CHECK(results.size() == 0);
        results = realm.objects<Person>().where("age = $0", {42});
        CHECK(results.size() == 1);
        Person john = results[0];
        CHECK(john.age == 42);
        CHECK(john.name == "John");
    }

    SECTION("optionals") {
        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(std::move(config));

        auto obj = AllTypesObject();
        realm.write([&]() {
            realm.add(obj);
        });
        auto res = realm.objects<AllTypesObject>().where([](auto& obj) {
            return obj.opt_str_col == std::nullopt;
        });
        CHECK(res.size() == 1);

        res = realm.objects<AllTypesObject>().where([](auto& obj) {
            return obj.opt_str_col != std::nullopt;
        });
        CHECK(res.size() == 0);

        res = realm.objects<AllTypesObject>().where([](auto& obj) {
            return obj.mixed_col == std::nullopt;
        });
        CHECK(res.size() == 1);

        res = realm.objects<AllTypesObject>().where([](auto& obj) {
            return obj.mixed_col != std::nullopt;
        });
        CHECK(res.size() == 0);
    }
}