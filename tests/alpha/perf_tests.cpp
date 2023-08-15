#include "../main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("basic_alpha", "[performance]") {
    BENCHMARK_ADVANCED("write 1000")(Catch::Benchmark::Chronometer meter) {
        realm_path path;
        realm::db_config config;
        config.set_path(path);
        auto realm = open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(std::move(config));

        return meter.measure([&]() {
            realm.write([&] {
                for (int64_t i = 0; i < 10000; i++) {
                    AllTypesObject o;
                    o._id = i;
                    realm.add(std::move(o));
                }
            });
        });

        CHECK(realm.objects<AllTypesObject>().size() == 10000);

    };

    BENCHMARK_ADVANCED("read 1000")(Catch::Benchmark::Chronometer meter) {
        realm_path path;
        realm::db_config config;
        config.set_path(path);
        auto realm = open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(std::move(config));

        realm.write([&] {
            for (int64_t i = 0; i < 10000; i++) {
                AllTypesObject o;
                o._id = i;
                realm.add(std::move(o));
            }
        });

        return meter.measure([&]() {
            auto results = realm.objects<AllTypesObject>();
            CHECK(results.size() == 10000);
            for (int64_t i = 0; i < 10000; i++) {
                CHECK(results[i]._id == i);
            }
        });
    };

    BENCHMARK_ADVANCED("write 10000")(Catch::Benchmark::Chronometer meter) {
        realm_path path;
        realm::db_config config;
        config.set_path(path);
        auto realm = open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(std::move(config));

        return meter.measure([&]() {
            realm.write([&] {
                for (int64_t i = 0; i < 10000; i++) {
                    AllTypesObject o;
                    o._id = i;
                    realm.add(std::move(o));
                }
            });
        });

        CHECK(realm.objects<AllTypesObject>().size() == 10000);

    };

    BENCHMARK_ADVANCED("read 10000")(Catch::Benchmark::Chronometer meter) {
        realm_path path;
        realm::db_config config;
        config.set_path(path);
        auto realm = open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(std::move(config));

        realm.write([&] {
            for (int64_t i = 0; i < 10000; i++) {
                AllTypesObject o;
                o._id = i;
                realm.add(std::move(o));
            }
        });

        return meter.measure([&]() {
            auto results = realm.objects<AllTypesObject>();
            CHECK(results.size() == 10000);
            for (int64_t i = 0; i < 10000; i++) {
                CHECK(results[i]._id == i);
            }
        });
    };
}
