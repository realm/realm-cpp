#include "test_objects.hpp"
#include "../../main.hpp"

namespace realm::experimental {
    TEST_CASE("frozen") {
        SECTION("realm") {
            realm_path path;
            realm::db_config config;
            config.set_path(path);
            auto realm = db(std::move(config));
            auto o = realm.write([&] {
                return realm.add(AllTypesObject());
            });

            //realm.freeze();


            // List:
            // testIsFrozen
            // testFreezingObjectReturnsSelf
            // testAccessFrozenObjectFromDifferentThread
            // testObserveFrozenArray
            // testQueryFrozenArray
            // testFrozenListsDoNotUpdate

            // Same for Map and Set


            // testLinkingObjectsOnFrozenObject

            // testMutateFrozenObject
            // testFrozenObjectEquality

            // Realm
        }
    }
}