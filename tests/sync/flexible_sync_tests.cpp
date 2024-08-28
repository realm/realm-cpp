#include "../admin_utils.hpp"
#include "../main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("flexible_sync", "[sync][flx]") {
    auto config = realm::App::configuration();
    config.app_id = Admin::Session::shared().cached_app_id();
    config.base_url = Admin::Session::shared().base_url();
    auto app = realm::App(config);
    app.get_sync_manager().set_log_level(logger::level::trace);
    SECTION("all") {
        auto user = app.login(realm::App::credentials::anonymous()).get();
        auto flx_sync_config = user.flexible_sync_configuration();
        auto synced_realm = db(flx_sync_config);

        auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                              subs.clear();
                                                          }).get();
        CHECK(update_success == true);
        CHECK(synced_realm.subscriptions().size() == 0);

        update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                         subs.add<AllTypesObject>("foo-strings", [](auto &obj) {
                                                             return obj.str_col == "foo";
                                                         });
                                                         subs.add<AllTypesObjectLink>("foo-link");
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
            AllTypesObject o;
            o._id = 1;
            o.str_col = "foo";
            synced_realm.add(std::move(o));
        });

        synced_realm.get_sync_session()->wait_for_upload_completion().get();
        synced_realm.get_sync_session()->wait_for_download_completion().get();

        synced_realm.refresh();
        auto objs = synced_realm.objects<AllTypesObject>();

        CHECK(objs[0]._id == (int64_t) 1);

        synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                        subs.update_subscription<AllTypesObject>("foo-strings", [](auto &obj) {
                                            return obj.str_col == "bar" && obj._id == (int64_t) 1230;
                                        });
                                    })
                .get();

        auto sub2 = *synced_realm.subscriptions().find("foo-strings");
        CHECK(sub2.name == "foo-strings");
        CHECK(sub2.object_class_name == "AllTypesObject");
        CHECK(sub2.query_string == "str_col == \"bar\" and _id == 1230");

        synced_realm.write([&synced_realm]() {
            AllTypesObject o;
            o._id = 1230;
            o.str_col = "bar";
            synced_realm.add(std::move(o));
        });

        synced_realm.get_sync_session()->wait_for_upload_completion().get();
        synced_realm.get_sync_session()->wait_for_download_completion().get();

        synced_realm.refresh();
        objs = synced_realm.objects<AllTypesObject>();
        CHECK(objs.size() == 1);
    }

    SECTION("encrypted sync realm") {
        std::array<char, 64> example_key = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 7, 7, 0, 0, 0, 0, 0, 0};
        realm::App::configuration app_config;
        app_config.app_id = Admin::Session::shared().create_app({"str_col", "_id"});
        app_config.base_url = Admin::Session::shared().base_url();
        app_config.metadata_encryption_key = example_key;
        auto encrypted_app = realm::App(app_config);
        auto user = encrypted_app.login(realm::App::credentials::anonymous()).get();
        auto flx_sync_config = user.flexible_sync_configuration();
        flx_sync_config.set_encryption_key(example_key);
        auto synced_realm = db(flx_sync_config);

        auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                              subs.clear();
                                                          })
                                      .get();
        CHECK(update_success == true);
        CHECK(synced_realm.subscriptions().size() == 0);
        // Missing encryption key
        auto flx_sync_config2 = user.flexible_sync_configuration();
        REQUIRE_THROWS(db(flx_sync_config2));
    }

    SECTION("open synced realm without sync config") {
        auto user = app.login(realm::App::credentials::anonymous()).get();
        std::string realm_path = user.flexible_sync_configuration().path();

        {
            auto flx_sync_config = user.flexible_sync_configuration();
            auto synced_realm = db(flx_sync_config);
            auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                subs.clear();
            }).get();
            CHECK(update_success == true);
            CHECK(synced_realm.subscriptions().size() == 0);

            update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                        subs.add<AllTypesObject>("my-sub", [](auto &obj) {
                            return obj.str_col == "a" && obj._id > 500 && obj._id < 520;
                        });
                        subs.add<AllTypesObjectLink>("foo-link");
                    })
                    .get();
            CHECK(update_success == true);
            CHECK(synced_realm.subscriptions().size() == 2);
            synced_realm.get_sync_session()->wait_for_upload_completion().get();
            synced_realm.get_sync_session()->wait_for_download_completion().get();

            auto objs = synced_realm.objects<AllTypesObject>();
            CHECK(objs.size() == 0);

            synced_realm.write([&synced_realm]() {
                AllTypesObject o;
                o._id = 501;
                o.str_col = "a";
                synced_realm.add(std::move(o));
            });

            synced_realm.refresh();
            synced_realm.get_sync_session()->wait_for_upload_completion().get();
            synced_realm.get_sync_session()->wait_for_download_completion().get();
            objs = synced_realm.objects<AllTypesObject>();
            CHECK(objs.size() == 1);
        }

        {
            auto local_config = realm::db_config();
            local_config.set_path(realm_path);
            local_config.enable_forced_sync_history();
            auto local_realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(local_config);
            CHECK(local_realm.objects<AllTypesObject>().size() == 1);
            local_realm.write([&local_realm]() {
                AllTypesObject o;
                o._id = 502;
                o.str_col = "a";
                local_realm.add(std::move(o));
            });
            // outside of subscription view, will be reverted once opened
            // with a sync config.
            local_realm.write([&local_realm]() {
                AllTypesObject o;
                o._id = 503;
                o.str_col = "b";
                local_realm.add(std::move(o));
            });
            CHECK(local_realm.objects<AllTypesObject>().size() == 3);
        }

        {
            auto flx_sync_config = user.flexible_sync_configuration();
            auto synced_realm = db(flx_sync_config);
            CHECK(synced_realm.objects<AllTypesObject>().size() == 3);
            auto subs_size = synced_realm.subscriptions().size();
            CHECK(subs_size == 2);

            synced_realm.get_sync_session()->wait_for_upload_completion().get();
            synced_realm.get_sync_session()->wait_for_download_completion().get();
            synced_realm.refresh();
            CHECK(synced_realm.objects<AllTypesObject>().size() == 2);
        }
    }
}

template<typename T, typename Func>
void test_set(realm::managed<T>* property, Func f,
          std::vector<typename T::value_type>&& values, size_t&& expected_count = 3) {
    f(property, values, expected_count);
}

TEST_CASE("set collection sync", "[set]") {
    auto config = realm::App::configuration();
    config.app_id = Admin::Session::shared().cached_app_id();
    config.base_url = Admin::Session::shared().base_url();
    auto app = realm::App(config);
    SECTION("insert") {
        auto user = app.login(realm::App::credentials::anonymous()).get();
        auto flx_sync_config = user.flexible_sync_configuration();
        auto realm = realm::db(std::move(flx_sync_config));
        auto update_success = realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                              subs.clear();
                                                          }).get();
        CHECK(update_success == true);
        CHECK(realm.subscriptions().size() == 0);

        update_success = realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                         subs.add<AllTypesObject>("foo-strings", [](auto& obj) {
                                                              return obj._id == 123;
                                                          });
                                                         subs.add<AllTypesObjectLink>("foo-link");
                                                     }).get();
        CHECK(update_success == true);
        CHECK(realm.subscriptions().size() == 2);

        auto obj = realm::AllTypesObject();
        obj._id = 123;

        auto managed_obj = realm.write([&]() {
            return realm.add(std::move(obj));
        });

        auto scenario = [&](auto &p, auto &values, size_t expected_count) {
            realm.write([&]() {
                p->clear();
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
        test_set(&managed_obj.set_object_id_col, scenario, {obj_id, obj_id, realm::object_id::generate(), realm::object_id::generate()}); // here

        auto bin_data = std::vector<uint8_t>({1, 2, 3, 4});
        test_set(&managed_obj.set_binary_col, scenario, {bin_data, bin_data, std::vector<uint8_t>({1, 3, 4}), std::vector<uint8_t>({1})});

        auto time = std::chrono::system_clock::now();
        auto time2 = time + time.time_since_epoch();
        test_set(&managed_obj.set_date_col, scenario, {time, time, time2, std::chrono::time_point<std::chrono::system_clock>()}); // here
        test_set(&managed_obj.set_mixed_col, scenario, {realm::mixed((int64_t) 42), realm::mixed((int64_t) 42), realm::mixed("24"), realm::mixed(realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002"))});
        realm.get_sync_session()->wait_for_upload_completion().get();
        realm.get_sync_session()->wait_for_download_completion().get();

        app.register_user("set_collection_sync@mongodb.com", "foobar").get();
        auto user2 = app.login(realm::App::credentials::username_password("set_collection_sync@mongodb.com", "foobar")).get();

        auto realm2 = realm::db(user2.flexible_sync_configuration());
        auto update_success2 = realm2.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                  subs.add<AllTypesObject>("foo-strings", [](auto& obj) {
                                                      return obj._id == 123;
                                                  });
                                                  subs.add<AllTypesObjectLink>("foo-link");
                                              }).get();
        CHECK(update_success2 == true);
        CHECK(realm2.subscriptions().size() == 2);
        realm2.get_sync_session()->wait_for_upload_completion().get();
        realm2.get_sync_session()->wait_for_download_completion().get();
        realm2.refresh();
        auto objs = realm2.objects<AllTypesObject>();
        CHECK(objs.size() == 1);
    }
}

TEST_CASE("pause_resume_sync", "[sync][flx]") {
    auto config = realm::App::configuration();
    config.app_id = Admin::Session::shared().cached_app_id();
    config.base_url = Admin::Session::shared().base_url();
    auto app = realm::App(config);

    SECTION("pause_resume") {
        auto user = app.login(realm::App::credentials::anonymous()).get();

        auto flx_sync_config = user.flexible_sync_configuration();
        auto synced_realm = db(flx_sync_config);
        auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                              subs.clear();
                                                          }).get();
        CHECK(update_success == true);
        update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                         subs.add<AllTypesObject>("foo-strings");
                                                         subs.add<AllTypesObjectLink>("foo-link");
                                                     })
                                 .get();
        CHECK(update_success == true);

        auto wait_for_state = [](enum realm::sync_session::connection_state expected_state,
                                 const db& realm,
                                 std::function<void(const db&)> fn) {
            std::promise<void> p;
            std::future<void> f = p.get_future();
            auto token = realm.get_sync_session()->observe_connection_change([&]
                                                                             (enum realm::sync_session::connection_state,
                                                                              enum realm::sync_session::connection_state new_state) {
                if (new_state == expected_state)
                    p.set_value();
            });
            fn(realm);

            if (f.wait_for(std::chrono::milliseconds(10000)) == std::future_status::ready) {
                realm.get_sync_session()->unregister_connection_change_observer(token);
                return f.get();
            } else {
                throw std::runtime_error("Timeout exceeded");
            }
        };

        wait_for_state(realm::sync_session::connection_state::disconnected, synced_realm, [](const db& realm) {
            realm.get_sync_session()->pause();
        });
        CHECK(synced_realm.get_sync_session()->state() == realm::sync_session::state::paused);

        wait_for_state(realm::sync_session::connection_state::connected, synced_realm, [](const db& realm) {
            realm.get_sync_session()->resume();
        });
        CHECK(synced_realm.get_sync_session()->state() == realm::sync_session::state::active);
    }
}

TEST_CASE("delete created sync objects", "[sync][flx]") {
    auto config = realm::App::configuration();
    config.app_id = Admin::Session::shared().cached_app_id();
    config.base_url = Admin::Session::shared().base_url();
    auto app = realm::App(config);

    auto user = app.login(realm::App::credentials::anonymous()).get();
    auto flx_sync_config = user.flexible_sync_configuration();
    auto synced_realm = db(flx_sync_config);
    auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                          subs.clear();
                                                      })
                                  .get();
    CHECK(update_success == true);
    update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                     subs.add<AllTypesObject>("foo-strings");
                                                     subs.add<AllTypesObjectLink>("foo-link");
                                                 })
                             .get();
    CHECK(update_success == true);

    CHECK(synced_realm.subscriptions().size() == 2);
    synced_realm.get_sync_session()->wait_for_upload_completion().get();
    synced_realm.get_sync_session()->wait_for_download_completion().get();
    synced_realm.refresh();
    auto links = synced_realm.objects<AllTypesObjectLink>();
    // No links were created during the tests
    CHECK(links.size() == 0);
    auto objs = synced_realm.objects<AllTypesObject>();
    CHECK(objs.size() > 0);
    synced_realm.write([&synced_realm, &objs]() {
        while (objs.size() > 0) {
            auto obj = objs[0];
            synced_realm.remove(obj);
        }
    });
    synced_realm.get_sync_session()->wait_for_upload_completion().get();
    synced_realm.get_sync_session()->wait_for_download_completion().get();
    synced_realm.refresh();

    links = synced_realm.objects<AllTypesObjectLink>();
    CHECK(objs.size() == 0);
    objs = synced_realm.objects<AllTypesObject>();
    CHECK(objs.size() == 0);
}
