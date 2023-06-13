#include "../../admin_utils.hpp"
#include "../../main.hpp"
#include "../../sync_test_utils.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("asymmetric object", "[sync_beta]") {
    SECTION("basic", "[sync]") {
        auto asymmetric_app_id = Admin::shared().create_app();
        auto app = realm::App(asymmetric_app_id, Admin::shared().base_url());
        auto user = app.login(realm::App::credentials::anonymous()).get();
        auto synced_realm = experimental::db(user.flexible_sync_configuration());

        auto obj = experimental::AllTypesAsymmetricObject();
        const realm::object_id oid = realm::object_id::generate();
        obj._id = oid;
        synced_realm.write([&] {
            synced_realm.add(std::move(obj));
        });

        test::wait_for_sync_uploads(user).get();
        test::wait_for_sync_downloads(user).get();

        auto result = user.call_function("asymmetricSyncData", bson::BsonArray({bson::BsonDocument{{"_id", oid.to_string()}}})).get_future().get();
        CHECK(result);
        auto arr = bson::BsonArray(*result);
        CHECK(arr.size() == 1);
        CHECK(bson::BsonDocument(arr[0])["_id"].operator ObjectId().to_string() == oid.to_string());
    }
}
