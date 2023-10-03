#include "../../admin_utils.hpp"
#include "../../main.hpp"
#include "../../sync_test_utils.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("app", "[sync]") {
    auto app = realm::App(Admin::shared().cached_app_id(), Admin::shared().base_url());

    SECTION("get_current_user") {
        auto user = app.login(realm::App::credentials::anonymous()).get();

        auto user_from_app = app.get_current_user();
        CHECK(user_from_app);

        CHECK(user.state() == realm::user::state::logged_in);
        CHECK(user_from_app->state() == realm::user::state::logged_in);
        CHECK(user_from_app->is_logged_in());
        CHECK(user == user_from_app);

        app.register_user("get_current_user@mongodb.com", "foobar").get();
        auto user2 = app.login(realm::App::credentials::username_password("get_current_user@mongodb.com", "foobar")).get();
        CHECK(user2 != user_from_app);

        user.log_out().get();
        user = app.login(realm::App::credentials::anonymous()).get();
        user_from_app = app.get_current_user();
        CHECK(user == user_from_app);
    }

    SECTION("clear_cached_apps") {
        auto temp_app_id = Admin::shared().create_app({"str_col", "_id"});
        auto temp_app = realm::App(temp_app_id, Admin::shared().base_url());
        auto cached_app = temp_app.get_cached_app(temp_app_id, Admin::shared().base_url());
        CHECK(cached_app.has_value());
        app.clear_cached_apps();
        cached_app = temp_app.get_cached_app(temp_app_id, Admin::shared().base_url());
        CHECK(cached_app.has_value() == false);
    }

    SECTION("error handling") {
        auto dead_app = realm::App("NA", Admin::shared().base_url());
        REQUIRE_THROWS_AS(dead_app.login(realm::App::credentials::anonymous()).get(), realm::app_error);
        REQUIRE_THROWS_AS(dead_app.register_user("", "").get(), realm::app_error);

        std::promise<realm::app_error> error_promise;
        std::future<realm::app_error> future = error_promise.get_future();
        dead_app.login(realm::App::credentials::anonymous(), [&](realm::user, std::optional<realm::app_error> e) mutable {
            CHECK(e);
            error_promise.set_value(*e);
        });
        CHECK(future.get().is_http_error());

        auto user = app.login(realm::App::credentials::anonymous()).get();
        user.log_out().get();
        REQUIRE_THROWS(user.log_out().get());
    }
}