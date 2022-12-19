#include "main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("bulk_insert", "[performance]") {
    BENCHMARK("insert") {
        realm_path path;

        auto realm = open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(path);
        std::vector<AllTypesObject> v;
        for (size_t i = 0; i < 10000; i++) {
            v.emplace_back(AllTypesObject());
        }
        realm.write([&v, &realm] {
           for (auto& o : v) {
               realm.add(o);
           }
        });
        return path;
    };
    BENCHMARK("mass_insert") {
        realm_path path;

        auto realm = open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(path);
        std::vector<AllTypesObject> v;
        for (size_t i = 0; i < 10000; i++) {
            v.emplace_back(AllTypesObject());
        }
        realm.write([&v, &realm] {
            realm.add(v);
        });
        return path;
    };
}
