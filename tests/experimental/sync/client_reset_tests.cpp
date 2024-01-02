#include "../../admin_utils.hpp"
#include "../../main.hpp"
#include "test_objects.hpp"

#include <realm/object-store/sync/sync_session.hpp>
#include <realm/object-store/sync/sync_manager.hpp>
#include <realm/sync/client_base.hpp>

using namespace realm;

namespace realm::experimental {
    struct client_reset_obj {
        primary_key<int64_t> _id;
        std::string str_col;
    };

    REALM_SCHEMA(client_reset_obj, _id, str_col);
}

void simulate_client_reset_error_for_session(sync_session&& session) {
    realm::sync::SessionErrorInfo error = {{realm::ErrorCodes::BadChangeset, "Not a real error message"}, true};
    error.server_requests_action = realm::sync::ProtocolErrorInfo::Action::ClientReset;
    auto raw_session = session.operator std::weak_ptr<::realm::SyncSession>().lock();
    SyncSession::OnlyForTesting::handle_error(*raw_session, std::move(error));
}

void prepare_realm(const realm::db_config& flx_sync_config, const user& sync_user) {
    sync_user.call_function("deleteClientResetObjects", "[]").get();
    auto synced_realm = experimental::db(flx_sync_config);
    auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                          subs.add<experimental::client_reset_obj>("foo-strings");
                                                      }).get();
    CHECK(update_success == true);
    CHECK(synced_realm.subscriptions().size() == 1);
    sync_user.call_function("insertClientResetObject", "[]").get();

    auto session = synced_realm.get_sync_session()->operator std::weak_ptr<::realm::SyncSession>().lock().get();
    session->pause();
    Admin::shared().disable_sync();
    // Add an object to the local realm that won't be synced due to the suspend
    synced_realm.write([&synced_realm]() {
        experimental::client_reset_obj o;
        o._id = 2;
        o.str_col = "local only";
        synced_realm.add(std::move(o));
    });
    synced_realm.refresh();
    Admin::shared().enable_sync();
};

TEST_CASE("client_reset", "[sync]") {

    SECTION("error handler") {
        auto app = realm::App(realm::App::configuration({Admin::shared().create_app({"str_col", "_id"}), Admin::shared().base_url()}));
        app.get_sync_manager().set_log_level(logger::level::all);
        auto user = app.login(realm::App::credentials::anonymous()).get();
        auto flx_sync_config = user.flexible_sync_configuration();
        auto synced_realm = experimental::db(flx_sync_config);

        std::promise<void> p;
        std::future<void> f = p.get_future();

        flx_sync_config.sync_config().set_error_handler([&p](sync_session session, sync_error error){
            CHECK(error.message() == "Not a real error message");
            CHECK(error.get_status().reason() == "Not a real error message");
            CHECK(error.is_client_reset_requested());
            CHECK(error.get_status().code_string() == "BadChangeset");
            p.set_value();
        });

        simulate_client_reset_error_for_session(*synced_realm.get_sync_session());

        if (f.wait_for(std::chrono::milliseconds(15000)) == std::future_status::ready) {
            f.get();
        } else {
            std::runtime_error("Timeout exceeded");
        }
    }

    SECTION("manual handler") {
        auto app = realm::App(realm::App::configuration({Admin::shared().create_app({"_id", "str_col"}, "test", false, false), Admin::shared().base_url()}));
        app.get_sync_manager().set_log_level(logger::level::all);
        auto user = app.login(realm::App::credentials::anonymous()).get();
        auto flx_sync_config = user.flexible_sync_configuration();
        flx_sync_config.set_client_reset_handler(client_reset::manual());
        auto synced_realm = experimental::db(flx_sync_config);

        auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                         subs.add<experimental::client_reset_obj>("foo-strings");
                                                     }).get();
        CHECK(update_success == true);
        CHECK(synced_realm.subscriptions().size() == 1);

        synced_realm.write([&synced_realm]() {
            experimental::client_reset_obj o;
            o._id = 1;
            o.str_col = "foo";
            synced_realm.add(std::move(o));
        });

        synced_realm.get_sync_session()->wait_for_upload_completion();

        std::promise<void> p;
        std::future<void> f = p.get_future();

        flx_sync_config.sync_config().set_error_handler([&p](sync_session session, sync_error error){
            CHECK(error.message().find("Bad client file identifier") != std::string::npos);
            CHECK(error.get_status().reason().find("Bad client file identifier") != std::string::npos);
            CHECK(error.is_client_reset_requested());
            p.set_value();
        });

        auto session = synced_realm.get_sync_session()->operator std::weak_ptr<::realm::SyncSession>().lock().get();
        session->pause();
        Admin::shared().disable_sync();
        synced_realm.write([&synced_realm]() {
            experimental::client_reset_obj o;
            o._id = 2;
            o.str_col = "local only";
            synced_realm.add(std::move(o));
        });
        Admin::shared().enable_sync();
        session->resume();

        CHECK(f.wait_for(std::chrono::milliseconds(60000)) == std::future_status::ready);
    }

    SECTION("discard_unsynced_changes") {
        auto app = realm::App(realm::App::configuration({Admin::shared().create_app({"str_col", "_id"}, "test", false, false), Admin::shared().base_url()}));
        app.get_sync_manager().set_log_level(logger::level::all);
        auto user = app.login(realm::App::credentials::anonymous()).get();

        auto flx_sync_config = user.flexible_sync_configuration();

        std::promise<void> before_handler_promise;
        std::future<void> before_handler_future = before_handler_promise.get_future();
        std::promise<void> after_handler_promise;
        std::future<void> after_handler_future = after_handler_promise.get_future();

        auto before_handler = [&](experimental::db before) {
            auto results = before.objects<experimental::client_reset_obj>();
            CHECK(results.size() == 1);
            CHECK(before.is_frozen());
            before_handler_promise.set_value();
        };

        auto after_handler = [&](experimental::db local, experimental::db remote) {
            auto results_local = local.objects<experimental::client_reset_obj>();
            CHECK(local.is_frozen());
            CHECK(results_local.size() == 1);

            auto results_remote = remote.objects<experimental::client_reset_obj>();
            CHECK(!remote.is_frozen());
            CHECK(results_remote[0].str_col == "remote obj");
            CHECK(results_remote.size() == 1);
            after_handler_promise.set_value();
        };

        flx_sync_config.set_client_reset_handler(client_reset::discard_unsynced_changes(before_handler, after_handler));
        prepare_realm(flx_sync_config, user);

        auto synced_realm2 = experimental::db(flx_sync_config);
        synced_realm2.refresh();
        // The client_reset_object created locally with _id=2 should have been discarded,
        // while the one from the server _id=1 should be present

        CHECK(before_handler_future.wait_for(std::chrono::milliseconds(60000)) == std::future_status::ready);
        CHECK(after_handler_future.wait_for(std::chrono::milliseconds(60000)) == std::future_status::ready);
    }

    SECTION("recover_or_discard_unsynced_changes") {
        auto app = realm::App(realm::App::configuration({Admin::shared().create_app({"str_col", "_id"}, "test", false, false), Admin::shared().base_url()}));
        app.get_sync_manager().set_log_level(logger::level::all);
        auto user = app.login(realm::App::credentials::anonymous()).get();
        auto flx_sync_config = user.flexible_sync_configuration();

        std::promise<void> before_handler_promise;
        std::future<void> before_handler_future = before_handler_promise.get_future();
        std::promise<void> after_handler_promise;
        std::future<void> after_handler_future = after_handler_promise.get_future();

        auto before_handler = [&](experimental::db before) {
            auto results = before.objects<experimental::client_reset_obj>();
            CHECK(results.size() == 1);
            CHECK(before.is_frozen());
            before_handler_promise.set_value();
        };

        auto after_handler = [&](experimental::db local, experimental::db remote) {
            auto results_local = local.objects<experimental::client_reset_obj>();
            CHECK(results_local.size() == 1);
            CHECK(local.is_frozen());

            auto results_remote = remote.objects<experimental::client_reset_obj>();
            CHECK(results_remote.size() == 2);
            after_handler_promise.set_value();
        };

        flx_sync_config.set_client_reset_handler(client_reset::recover_or_discard_unsynced_changes(before_handler, after_handler));

        prepare_realm(flx_sync_config, user);

        auto synced_realm2 = experimental::db(flx_sync_config);
        synced_realm2.refresh();
        // The client_reset_object created locally with _id=2 should be present as it should be recovered,
        // while the one from the server _id=1 should be present

        CHECK(before_handler_future.wait_for(std::chrono::milliseconds(60000)) == std::future_status::ready);
        CHECK(after_handler_future.wait_for(std::chrono::milliseconds(60000)) == std::future_status::ready);
    }

    SECTION("recover_unsynced_changes") {
        auto app = realm::App(realm::App::configuration({Admin::shared().create_app({"str_col", "_id"}, "test", false, false), Admin::shared().base_url()}));
        app.get_sync_manager().set_log_level(logger::level::all);
        auto user = app.login(realm::App::credentials::anonymous()).get();
        auto flx_sync_config = user.flexible_sync_configuration();

        std::promise<void> before_handler_promise;
        std::future<void> before_handler_future = before_handler_promise.get_future();
        std::promise<void> after_handler_promise;
        std::future<void> after_handler_future = after_handler_promise.get_future();

        auto before_handler = [&](experimental::db before) {
            auto results = before.objects<experimental::client_reset_obj>();
            CHECK(results.size() == 1);
            CHECK(before.is_frozen());
            before_handler_promise.set_value();
        };

        auto after_handler = [&](experimental::db local, experimental::db remote) {
            auto results_local = local.objects<experimental::client_reset_obj>();
            CHECK(results_local.size() == 1);
            CHECK(local.is_frozen());

            auto results_remote = remote.objects<experimental::client_reset_obj>();
            CHECK(results_remote.size() == 2);
            after_handler_promise.set_value();
        };

        flx_sync_config.set_client_reset_handler(client_reset::recover_unsynced_changes(before_handler, after_handler));

        prepare_realm(flx_sync_config, user);

        auto synced_realm2 = experimental::db(flx_sync_config);
        synced_realm2.refresh();
        // The object created locally and the object created on the server
        // should both be integrated into the new realm file.

        CHECK(before_handler_future.wait_for(std::chrono::milliseconds(60000)) == std::future_status::ready);
        CHECK(after_handler_future.wait_for(std::chrono::milliseconds(60000)) == std::future_status::ready);
    }

    SECTION("recover_unsynced_changes_with_failure") {
        auto app = realm::App(realm::App::configuration({Admin::shared().create_app({"str_col", "_id"}, "test", false, true), Admin::shared().base_url()}));
        app.get_sync_manager().set_log_level(logger::level::all);
        auto user = app.login(realm::App::credentials::anonymous()).get();
        auto flx_sync_config = user.flexible_sync_configuration();

        std::promise<void> error_handler_promise;
        std::future<void> error_handler_future = error_handler_promise.get_future();

        flx_sync_config.sync_config().set_error_handler([&error_handler_promise](sync_session session, sync_error error){
            CHECK(error.message().find("A client reset is required but the server does not permit recovery for this client") != std::string::npos);
            CHECK(error.get_status().reason().find("A client reset is required but the server does not permit recovery for this client") != std::string::npos);
            CHECK(error.is_client_reset_requested());
            error_handler_promise.set_value();
        });

        auto before_handler = [&](experimental::db before) {
            FAIL("Should not be called when automatic recovery fails");
        };

        auto after_handler = [&](experimental::db local, experimental::db remote) {
            FAIL("Should not be called when automatic recovery fails");
        };

        flx_sync_config.set_client_reset_handler(client_reset::recover_unsynced_changes(before_handler, after_handler));

        prepare_realm(flx_sync_config, user);
        auto synced_realm2 = experimental::db(flx_sync_config);
        synced_realm2.refresh();

        CHECK(error_handler_future.wait_for(std::chrono::milliseconds(60000)) == std::future_status::ready);
    }

}