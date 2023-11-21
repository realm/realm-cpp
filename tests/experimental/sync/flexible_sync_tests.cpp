#include "../../admin_utils.hpp"
#include "../../main.hpp"
#include "../../sync_test_utils.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("flexible_sync_beta", "[sync]") {
    auto app = realm::App(realm::App::configuration({Admin::shared().cached_app_id(), Admin::shared().base_url()}));
    SECTION("all") {
        app.get_sync_manager().set_log_level(logger::level::all);
        auto user = app.login(realm::App::credentials::anonymous()).get();
        auto flx_sync_config = user.flexible_sync_configuration();
        auto synced_realm = experimental::db(flx_sync_config);

        auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                              subs.clear();
                                                          }).get();
        CHECK(update_success == true);
        CHECK(synced_realm.subscriptions().size() == 0);

        update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                         subs.add<experimental::AllTypesObject>("foo-strings", [](auto &obj) {
                                                             return obj.str_col == "foo";
                                                         });
                                                         subs.add<experimental::AllTypesObjectLink>("foo-link");
                                                     })
                                 .get();
        CHECK(update_success == true);
        CHECK(synced_realm.subscriptions().size() == 2);

        auto sub = *synced_realm.subscriptions().find("foo-strings");
        CHECK(sub.name == "foo-strings");
        CHECK(sub.object_class_name == "AllTypesObject");
        CHECK(sub.query_string == "str_col == \"foo\"");

        auto non_existent_sub = synced_realm.subscriptions().find("non-existent");
        CHECK((non_existent_sub == std::nullopt) == true);

        synced_realm.write([&synced_realm]() {
            experimental::AllTypesObject o;
            o._id = 1;
            o.str_col = "foo";
            synced_realm.add(std::move(o));
        });

        test::wait_for_sync_uploads(user).get();
        test::wait_for_sync_downloads(user).get();
        synced_realm.refresh();
        auto objs = synced_realm.objects<experimental::AllTypesObject>();

        CHECK(objs[0]._id == (int64_t) 1);

        synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                        subs.update_subscription<experimental::AllTypesObject>("foo-strings", [](auto &obj) {
                                            return obj.str_col == "bar" && obj._id == (int64_t) 1230;
                                        });
                                    })
                .get();

        auto sub2 = *synced_realm.subscriptions().find("foo-strings");
        CHECK(sub2.name == "foo-strings");
        CHECK(sub2.object_class_name == "AllTypesObject");
        CHECK(sub2.query_string == "str_col == \"bar\" and _id == 1230");

        synced_realm.write([&synced_realm]() {
            experimental::AllTypesObject o;
            o._id = 1230;
            o.str_col = "bar";
            synced_realm.add(std::move(o));
        });

        test::wait_for_sync_uploads(user).get();
        test::wait_for_sync_downloads(user).get();

        synced_realm.refresh();
        objs = synced_realm.objects<experimental::AllTypesObject>();
        CHECK(objs.size() == 1);
    }

    SECTION("encrypted sync realm") {
        std::array<char, 64> example_key = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 7, 7, 0, 0, 0, 0, 0, 0};
        realm::App::configuration app_config;
        app_config.app_id = Admin::shared().create_app({"str_col", "_id"});
        app_config.base_url = Admin::shared().base_url();
        app_config.metadata_encryption_key = example_key;
        auto encrypted_app = realm::App(app_config);
        auto user = encrypted_app.login(realm::App::credentials::anonymous()).get();
        auto flx_sync_config = user.flexible_sync_configuration();
        flx_sync_config.set_encryption_key(example_key);
        auto synced_realm = experimental::db(flx_sync_config);

        auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                              subs.clear();
                                                          })
                                      .get();
        CHECK(update_success == true);
        CHECK(synced_realm.subscriptions().size() == 0);
        // Missing encryption key
        auto flx_sync_config2 = user.flexible_sync_configuration();
        REQUIRE_THROWS(experimental::db(flx_sync_config2));
    }
}

template<typename T, typename Func>
void test_set(realm::experimental::managed<T>* property, Func f,
          std::vector<typename T::value_type>&& values, size_t&& expected_count = 3) {
    f(property, values, expected_count);
}

TEST_CASE("set collection sync", "[set]") {
    auto app = realm::App(realm::App::configuration({Admin::shared().cached_app_id(), Admin::shared().base_url()}));

    SECTION("insert") {
        auto user = app.login(realm::App::credentials::anonymous()).get();
        auto flx_sync_config = user.flexible_sync_configuration();
        auto realm = realm::experimental::db(std::move(flx_sync_config));
        auto update_success = realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                              subs.clear();
                                                          }).get();
        CHECK(update_success == true);
        CHECK(realm.subscriptions().size() == 0);

        update_success = realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                         subs.add<experimental::AllTypesObject>("foo-strings", [](auto &obj) {
                                                             return obj.str_col == "foo";
                                                         });
                                                         subs.add<experimental::AllTypesObjectLink>("foo-link");
                                                     }).get();
        CHECK(update_success == true);
        CHECK(realm.subscriptions().size() == 2);

        auto obj = realm::experimental::AllTypesObject();

        auto managed_obj = realm.write([&]() {
            return realm.add(std::move(obj));
        });

        auto scenario = [&](auto &p, auto &values, size_t expected_count) {
            realm.write([&]() {
                for (auto v: values) {
                    p->insert(v);
                }
            });
            CHECK(p->size() == expected_count);
        };

        test_set(&managed_obj.set_int_col, scenario, {42, 42, 24, -1});
        test_set(&managed_obj.set_double_col, scenario, {42.001, 42.001, 24.001, -1.001});
        test_set(&managed_obj.set_bool_col, scenario, {true, false, true}, 2);
        test_set(&managed_obj.set_str_col, scenario, {"42", "42", "24", "-1"});
        test_set(&managed_obj.set_uuid_col, scenario, {realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120000"), realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120000"), realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120001"), realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002")});
        auto obj_id = realm::object_id::generate();
        test_set(&managed_obj.set_object_id_col, scenario, {obj_id, obj_id, realm::object_id::generate(), realm::object_id::generate()});

        auto bin_data = std::vector<uint8_t>({1, 2, 3, 4});
        test_set(&managed_obj.set_binary_col, scenario, {bin_data, bin_data, std::vector<uint8_t>({1, 3, 4}), std::vector<uint8_t>({1})});

        auto time = std::chrono::system_clock::now();
        auto time2 = time + time.time_since_epoch();
        test_set(&managed_obj.set_date_col, scenario, {time, time, time2, std::chrono::time_point<std::chrono::system_clock>()});
        test_set(&managed_obj.set_mixed_col, scenario, {realm::mixed((int64_t)42), realm::mixed((int64_t)42), realm::mixed("24"), realm::mixed(realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002"))});

        test::wait_for_sync_uploads(user).get();
        test::wait_for_sync_downloads(user).get();

        app.register_user("set_collection_sync@mongodb.com", "foobar").get();
        auto user2 = app.login(realm::App::credentials::username_password("set_collection_sync@mongodb.com", "foobar")).get();

        auto realm2 = realm::experimental::db(user2.flexible_sync_configuration());
        auto update_success2 = realm2.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                  subs.add<experimental::AllTypesObject>("foo-strings", [](auto &obj) {
                                                      return obj.str_col == "foo";
                                                  });
                                                  subs.add<experimental::AllTypesObjectLink>("foo-link");
                                              }).get();
        CHECK(update_success2 == true);
        CHECK(realm2.subscriptions().size() == 2);
        test::wait_for_sync_downloads(user2).get();
        realm2.refresh();
        auto objs = realm2.objects<experimental::AllTypesObject>();
        CHECK(objs.size() == 1);
    }
}