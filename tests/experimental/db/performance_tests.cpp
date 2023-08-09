#include "../../main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("bulk_insert", "[performance]") {
    BENCHMARK_ADVANCED("write 1000")(Catch::Benchmark::Chronometer meter) {
        realm_path path;
        realm::db_config config;
        config.set_path(path);
        auto realm = experimental::db(std::move(config));

        return meter.measure([&]() {
            realm.write([&] {
                for (int64_t i = 0; i < 1000; i++) {
                    experimental::AllTypesObject o;
                    o._id = i;
                    realm.add(std::move(o));
                }
            });
        });

    };

    BENCHMARK_ADVANCED("read 1000")(Catch::Benchmark::Chronometer meter) {
        realm_path path;
        realm::db_config config;
        config.set_path(path);
        auto realm = experimental::db(std::move(config));

        realm.write([&] {
            for (int64_t i = 0; i < 1000; i++) {
                experimental::AllTypesObject o;
                o._id = i;
                realm.add(std::move(o));
            }
        });

        return meter.measure([&]() {
            auto results = realm.objects<experimental::AllTypesObject>();
            CHECK(results.size() == 1000);
            for (int64_t i = 0; i < 1000; i++) {
                CHECK(results[i]._id == i);
            }
        });
    };
}
