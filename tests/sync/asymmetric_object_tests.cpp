#include "../admin_utils.hpp"
#include "../main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("asymmetric object", "[sync]") {
    SECTION("basic", "[sync]") {
        auto asymmetric_app_id = Admin::Session::shared().create_app({}, "test", true);
        auto config = realm::App::configuration();
        config.app_id = asymmetric_app_id;
        config.base_url = Admin::Session::shared().base_url();
        auto app = realm::App(config);
        auto user = app.login(realm::App::credentials::anonymous()).get();
        auto synced_realm = open<AllTypesAsymmetricObject, EmbeddedFoo>(user.flexible_sync_configuration());

        auto obj = AllTypesAsymmetricObject();
        const realm::object_id oid = realm::object_id::generate();
        obj._id = oid;
        synced_realm.write([&] {
            synced_realm.add(std::move(obj));
        });

        synced_realm.get_sync_session()->wait_for_upload_completion().get();

        auto result = user.call_function("asymmetricSyncData", { bsoncxx::document({{"_id", oid.to_string()}}) }).get();
        bsoncxx::array bson_array = *result;
        CHECK(bson_array.size() == 1);
        bsoncxx::document doc = bson_array[0];
        CHECK(doc["_id"] == oid);
    }
}
