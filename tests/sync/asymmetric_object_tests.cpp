#include "../admin_utils.hpp"
#include "../main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("asymmetric object", "[sync]") {
    SECTION("basic", "[sync]") {
        auto asymmetric_app_id = Admin::shared().create_app({}, "test", true);
        auto app = realm::App(realm::App::configuration({asymmetric_app_id, Admin::shared().base_url()}));
        auto user = app.login(realm::App::credentials::anonymous()).get();
        auto synced_realm = open<AllTypesAsymmetricObject, EmbeddedFoo>(user.flexible_sync_configuration());

        auto obj = AllTypesAsymmetricObject();
        const realm::object_id oid = realm::object_id::generate();
        obj._id = oid;
        synced_realm.write([&] {
            synced_realm.add(std::move(obj));
        });

        synced_realm.get_sync_session()->wait_for_upload_completion().get();

        auto result = user.call_function("asymmetricSyncData", bson::Bson(bson::BsonArray({bson::BsonDocument{{"_id", oid.to_string()}}})).toJson()).get();
        CHECK(result);
        CHECK(result->find(oid.to_string()) != std::string::npos);
    }
}
