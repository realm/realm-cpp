#include "admin_utils.hpp"
#include "test_objects.hpp"
#include "main.hpp"

TEST_CASE("app", "[app]") {
    SECTION("pbs") {
        auto app = realm::App("car-wsney");
        auto user = app.login(realm::App::Credentials::anonymous()).get_future().get();

        auto tsr = user.realm<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>("foo").get_future().get();
        auto synced_realm = tsr.resolve();
        synced_realm.write([&synced_realm]() {
            synced_realm.add(AllTypesObject{._id=1});
        });

        CHECK(*synced_realm.object<AllTypesObject>(1)._id == 1);
    }

#if TARGET_OS_IOS == 0 && TARGET_OS_WATCH == 0
    SECTION("login_username_password") {
        auto app_id = Admin::shared().create_app();
        auto app = realm::App(app_id, "http://localhost:9090");
        CHECK_THROWS(app.login(realm::App::Credentials::apple("id_token")).get_future().get());

        app.register_user("foo@mongodb.com", "foobar").get_future().get();
        auto user = app.login(
                realm::App::Credentials::username_password("foo@mongodb.com", "foobar")).get_future().get();
        CHECK(!user.access_token().empty());
    }
#endif
}