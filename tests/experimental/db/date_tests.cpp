#include "test_objects.hpp"
#include "../../main.hpp"

TEST_CASE("date", "[date]") {
    realm_path path;
    realm::db_config config;
    config.set_path(path);

    SECTION("unmanaged_managed_ts_time_since_epoch", "[date]") {
        auto realm = realm::experimental::db(std::move(config));
        auto ts = std::chrono::time_point<std::chrono::system_clock>();
        realm::experimental::AllTypesObject object;
        object.date_col = ts;
        CHECK(object.date_col.time_since_epoch() == ts.time_since_epoch());
        auto managed_obj = realm.write([&] {
            return realm.add(std::move(object));
        });
        CHECK(managed_obj.date_col.time_since_epoch() == ts.time_since_epoch());
        CHECK(managed_obj.date_col.time_since_epoch() != (ts + std::chrono::seconds(42)).time_since_epoch());
    }

    SECTION("add") {
        auto realm = realm::experimental::db(std::move(config));
        realm::experimental::AllTypesObject obj;
        CHECK(obj.date_col == std::chrono::time_point<std::chrono::system_clock>{});
        auto now = std::chrono::system_clock::now();
        obj.date_col = now;
        CHECK(obj.date_col == now);
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        CHECK(managed_obj.date_col == now);
        realm.write([&managed_obj] {
            managed_obj.date_col += std::chrono::seconds(42);
        });
        CHECK(managed_obj.date_col == now + std::chrono::seconds(42));
    }

    SECTION("optional_unmanaged_managed_ts_time_since_epoch", "[date]") {
        auto realm = realm::experimental::db(std::move(config));
        auto ts = std::chrono::time_point<std::chrono::system_clock>();
        realm::experimental::AllTypesObject object;
        object.opt_date_col = ts;
        auto managed_obj = realm.write([&] {
            return realm.add(std::move(object));
        });
        CHECK(managed_obj.opt_date_col == ts);
        CHECK(managed_obj.opt_date_col != std::nullopt);
    }

    SECTION("optional_add") {
        auto realm = realm::experimental::db(std::move(config));
        realm::experimental::AllTypesObject obj;
        auto now = std::chrono::system_clock::now();
        obj.opt_date_col = now;
        CHECK(obj.opt_date_col == now);
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        CHECK(managed_obj.opt_date_col == now);
        realm.write([&managed_obj] {
            (*managed_obj.opt_date_col) += std::chrono::seconds(42);
        });
        CHECK(managed_obj.opt_date_col == now + std::chrono::seconds(42));
    }

    SECTION("rbool_comparison") {
        auto realm = realm::experimental::db(std::move(config));
        auto obj = realm::experimental::AllTypesObject();
        auto now = std::chrono::system_clock::now();
        obj.date_col = now;

        realm.write([&obj, &realm] {
            realm.add(std::move(obj));
        });

        auto results = realm.objects<realm::experimental::AllTypesObject>().where([&now](auto& o) {
            return o.date_col == now;
        });
        CHECK(results.size() == 1);
        CHECK(results[0].date_col.time_since_epoch() == now.time_since_epoch());

        results = realm.objects<realm::experimental::AllTypesObject>().where([&now](auto& o) {
            return o.date_col != now;
        });
        CHECK(results.size() == 0);
    }

    SECTION("optional_rbool_comparison") {
        auto realm = realm::experimental::db(std::move(config));
        auto obj = realm::experimental::AllTypesObject();
        auto now = std::chrono::system_clock::now();
        obj.opt_date_col = now;

        realm.write([&obj, &realm] {
            realm.add(std::move(obj));
        });

        auto results = realm.objects<realm::experimental::AllTypesObject>().where([&now](auto& o) {
            return o.opt_date_col == now;
        });
        CHECK(results.size() == 1);
        (*results[0].opt_date_col).time_since_epoch();
        CHECK(results[0].opt_date_col->time_since_epoch() == now.time_since_epoch());

        results = realm.objects<realm::experimental::AllTypesObject>().where([&now](auto& o) {
            return o.opt_date_col != now;
        });
        CHECK(results.size() == 0);
    }

    SECTION("operator_time_since_epoch", "[date]") {
        auto realm = realm::experimental::db(std::move(config));
        auto ts = std::chrono::time_point<std::chrono::system_clock>();
        realm::experimental::AllTypesObject object;
        object.date_col = ts;
        CHECK(object.date_col.time_since_epoch() == ts.time_since_epoch());
        auto managed_obj = realm.write([&] {
            return realm.add(std::move(object));
        });
        std::chrono::time_point<std::chrono::system_clock> val = managed_obj.date_col;
        CHECK(val == ts);
    }

    SECTION("optional_operator_time_since_epoch", "[date]") {
        auto realm = realm::experimental::db(std::move(config));
        auto ts = std::chrono::time_point<std::chrono::system_clock>();
        realm::experimental::AllTypesObject object;
        object.opt_date_col = ts;
        CHECK(object.opt_date_col->time_since_epoch() == ts.time_since_epoch());
        auto managed_obj = realm.write([&] {
            return realm.add(std::move(object));
        });
        std::optional<std::chrono::time_point<std::chrono::system_clock>> val = managed_obj.opt_date_col;
        CHECK(val == ts);
    }
}

