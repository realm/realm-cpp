#include "test_objects.hpp"
#include "main.hpp"
#include <realm/object-store/shared_realm.hpp>

TEST_CASE("cached realm") {
    SECTION("check realm is statically cached") {
        {
            realm_path path;
            auto realm = realm::internal::bridge::realm(realm::db_config(path));
            auto realm2 = realm::internal::bridge::realm(realm::db_config(path));
            realm::SharedRealm shared_realm = realm;
            realm::SharedRealm shared_realm2 = realm2;
            CHECK(shared_realm == shared_realm2);
            realm::internal::bridge::realm::reset_realm_state();
            auto realm3 = realm::internal::bridge::realm(realm::db_config(path));
            realm::SharedRealm shared_realm3 = realm3;
            CHECK(shared_realm != shared_realm3);
        }

        std::thread t1 = std::thread([]() {
            realm_path path;
            auto realm = realm::internal::bridge::realm(realm::db_config(path));
            auto realm2 = realm::internal::bridge::realm(realm::db_config(path));
            realm::SharedRealm shared_realm = realm;
            realm::SharedRealm shared_realm2 = realm2;
            CHECK(shared_realm == shared_realm2);
            realm::internal::bridge::realm::reset_realm_state();
            auto realm3 = realm::internal::bridge::realm(realm::db_config(path));
            realm::SharedRealm shared_realm3 = realm3;
            CHECK(shared_realm != shared_realm3);
        });
        t1.join();
    }

    SECTION("cached realm transactions") {
        realm_path path;
        auto o = AllTypesObject();
        {
            auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({path});
            realm.write([&] {
                realm.add(o);
            });
        }

        {
            CHECK(o.is_managed());
            auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({path});
            // If the cached realm is not returned this write would fail with a wrong transaction state.
            realm.write([&] {
                o.str_col = "foo";
            });
        }
        CHECK(*o.str_col == "foo");
    }
}