//#include "admin_utils.hpp"
//#include "main.hpp"
//#include "test_objects.hpp"
//#include "sync_test_utils.hpp"
//
//using namespace realm;
//
//TEST_CASE("asymmetric object", "[asymmetric]") {
//realm_path path;
//SECTION("basic", "[asymmetric]") {
//    auto app = realm::App(Admin::shared().create_app({"str_col", "_id"}), Admin::shared().base_url());
//    auto user = app.login(realm::App::credentials::anonymous()).get_future().get();
//    auto flx_sync_config = user.flexible_sync_configuration();
//    auto p = realm::async_open<AllTypesAsymmetricObject, AllTypesObjectEmbedded>(flx_sync_config);
//    auto tsr = p.get_future().get();
//    auto synced_realm = tsr.resolve();
//
//    auto obj = AllTypesAsymmetricObject();
//    synced_realm.add(obj);
//}
//}