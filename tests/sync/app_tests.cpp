#include "../admin_utils.hpp"
#include "../main.hpp"
#include "test_objects.hpp"

#include "external/json/json.hpp"
#include "realm/util/base64.hpp"

#if REALM_PLATFORM_APPLE
#import <CommonCrypto/CommonHMAC.h>
#else
#include <openssl/sha.h>
#include <openssl/hmac.h>
#endif

static std::string HMAC_SHA256(std::string_view key, std::string_view data)
{
#if REALM_PLATFORM_APPLE
    std::string ret;
    ret.resize(CC_SHA256_DIGEST_LENGTH);
    CCHmac(kCCHmacAlgSHA256, key.data(), key.size(), data.data(), data.size(),
           reinterpret_cast<uint8_t*>(const_cast<char*>(ret.data())));
    return ret;
#else
    std::array<unsigned char, EVP_MAX_MD_SIZE> hash;
    unsigned int hashLen;
    HMAC(EVP_sha256(), key.data(), static_cast<int>(key.size()), reinterpret_cast<unsigned char const*>(data.data()),
         static_cast<int>(data.size()), hash.data(), &hashLen);
    return std::string{reinterpret_cast<char const*>(hash.data()), hashLen};
#endif
}

static std::string create_jwt(const std::string& appId)
{
    nlohmann::json header = {{"alg", "HS256"}, {"typ", "JWT"}};
    nlohmann::json payload = {{"aud", appId}, {"sub", "someUserId"}, {"exp", 1961896476}};
    payload["user_data"]["name"] = "Foo Bar";
    payload["user_data"]["occupation"] = "firefighter";
    payload["my_metadata"]["name"] = "Bar Foo";
    payload["my_metadata"]["occupation"] = "stock analyst";
    std::string headerStr = header.dump();
    std::string payloadStr = payload.dump();
    std::string encoded_header;
    encoded_header.resize(realm::util::base64_encoded_size(headerStr.length()));
    realm::util::base64_encode(headerStr, encoded_header);
    std::string encoded_payload;
    encoded_payload.resize(realm::util::base64_encoded_size(payloadStr.length()));
    realm::util::base64_encode(payloadStr, encoded_payload);

    // Remove padding characters.
    while (encoded_header.back() == '=')
        encoded_header.pop_back();
    while (encoded_payload.back() == '=')
        encoded_payload.pop_back();

    std::string jwtPayload = encoded_header + "." + encoded_payload;

    auto mac = HMAC_SHA256("My_very_confidential_secretttttt", jwtPayload);

    std::string signature;
    signature.resize(realm::util::base64_encoded_size(mac.length()));
    realm::util::base64_encode(mac, signature);
    while (signature.back() == '=')
        signature.pop_back();
    std::replace(signature.begin(), signature.end(), '+', '-');
    std::replace(signature.begin(), signature.end(), '/', '_');

    return jwtPayload + "." + signature;
}

using namespace realm;

TEST_CASE("app", "[app]") {
    auto config = realm::App::configuration();
    config.app_id = Admin::Session::shared().cached_app_id();
    config.base_url = Admin::Session::shared().base_url();
    auto app = realm::App(config);

    SECTION("base_url") {
        auto config = realm::App::configuration();
        config.app_id = "NA";
        auto no_url_provided_app = realm::App(config);
        CHECK(no_url_provided_app.get_base_url() == "https://services.cloud.mongodb.com");

        auto config2 = realm::App::configuration();
        config2.app_id = "NA";
        config2.base_url = "https://foobar.com";
        auto with_url_provided_app = realm::App(config2);
        CHECK(with_url_provided_app.get_base_url() == "https://foobar.com");
    }

#ifdef REALM_ENABLE_EXPERIMENTAL
    SECTION("update_base_url") {
        auto config = realm::App::configuration();
        config.app_id = "NA";
        auto no_url_provided_app = realm::App(config);
        CHECK(no_url_provided_app.get_base_url() == "https://services.cloud.mongodb.com");
        REQUIRE_THROWS_AS(no_url_provided_app.update_base_url("https://foobar.com").get(), realm::app_error);
        CHECK(no_url_provided_app.get_base_url() == "https://services.cloud.mongodb.com");
        REQUIRE_THROWS(no_url_provided_app.update_base_url("asdfghjkl").get());
        CHECK(no_url_provided_app.get_base_url() == "https://services.cloud.mongodb.com");

        CHECK(app.get_base_url() == Admin::Session::shared().base_url());
        REQUIRE_THROWS_AS(app.update_base_url("https://foobar.com").get(), realm::app_error);
        CHECK(app.get_base_url() == Admin::Session::shared().base_url());
         // Cannot be changed because app id not available in atlas
        REQUIRE_THROWS_AS(app.update_base_url("").get(), realm::app_error);
        CHECK(app.get_base_url() == Admin::Session::shared().base_url());
        // This succeeds but the url is the same
        app.update_base_url(Admin::Session::shared().base_url()).get();
        CHECK(app.get_base_url() == Admin::Session::shared().base_url());
    }
#endif

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
        auto temp_app_id = Admin::Session::shared().create_app({"str_col", "_id"});
        auto config = realm::App::configuration();
        config.app_id = temp_app_id;
        config.base_url = Admin::Session::shared().base_url();

        auto temp_app = realm::App(config);
        auto cached_app = temp_app.get_cached_app(temp_app_id, Admin::Session::shared().base_url());
        CHECK(cached_app.has_value());
        app.clear_cached_apps();
        cached_app = temp_app.get_cached_app(temp_app_id, Admin::Session::shared().base_url());
        CHECK(cached_app.has_value() == false);
    }

    SECTION("error handling") {
        auto config = realm::App::configuration();
        config.app_id = "NA";
        config.base_url = Admin::Session::shared().base_url();
        auto dead_app = realm::App(config);
        REQUIRE_THROWS_AS(dead_app.login(realm::App::credentials::anonymous()).get(), realm::app_error);
        REQUIRE_THROWS_AS(dead_app.register_user("", "").get(), realm::app_error);

        std::promise<void> error_promise;
        std::future<void> future = error_promise.get_future();
        dead_app.login(realm::App::credentials::anonymous(), [&](realm::user, std::optional<realm::app_error> e) mutable {
            CHECK(e);
            CHECK(e->message().find("404 message: cannot find app using Client App ID 'NA'"));
            error_promise.set_value();
        });
    }

    SECTION("auth_providers_promise") {
        auto run_login = [&app](realm::App::credentials &&credentials) {
            auto user = app.login(std::move(credentials)).get();
            CHECK(!user.access_token().empty());
        };

        app.register_user("auth_providers_promise@mongodb.com", "foobar").get();
        run_login(realm::App::credentials::username_password("auth_providers_promise@mongodb.com", "foobar"));
        run_login(realm::App::credentials::anonymous());
        run_login(realm::App::credentials::custom(create_jwt(Admin::Session::shared().cached_app_id())));
    }

    SECTION("auth_providers_completion_hander") {
        app.register_user("auth_providers_completion_hander@mongodb.com", "foobar").get();
        std::promise<realm::user> p;
        app.login(realm::App::credentials::username_password("auth_providers_completion_hander@mongodb.com", "foobar"), [&](auto user, auto err) {
            p.set_value(user);
        });
        auto user = p.get_future().get();
        CHECK(user.state() == realm::user::state::logged_in);
    }

    SECTION("logout_anonymous") {
        auto user = app.login(realm::App::credentials::anonymous()).get();
        CHECK(!user.access_token().empty());
        CHECK(user.state() == realm::user::state::logged_in);

        user.log_out().get();
        CHECK(user.state() == realm::user::state::removed);
    }

    SECTION("logout") {
        app.register_user("logout@mongodb.com", "foobar").get();
        auto user = app.login(
                               realm::App::credentials::username_password("logout@mongodb.com", "foobar"))
                            .get();
        CHECK(!user.access_token().empty());
        CHECK(user.state() == realm::user::state::logged_in);

        user.log_out().get();
        CHECK(user.state() == realm::user::state::logged_out);
    }

    SECTION("logout_completion_handler") {
        auto user = app.login(realm::App::credentials::anonymous()).get();
        CHECK(user.state() == realm::user::state::logged_in);

        std::promise<void> p;
        user.log_out([&p](auto err) {
            CHECK(!err);
            p.set_value();
        });
        p.get_future().get();

        CHECK(user.state() == realm::user::state::removed);
    }

    SECTION("function_and_custom_user_data") {
        app.register_user("function_and_custom_user_data@mongodb.com", "foobar").get();
        auto user = app.login(
                               realm::App::credentials::username_password("function_and_custom_user_data@mongodb.com", "foobar"))
                            .get();
        auto result = user.call_function("updateUserData", { bsoncxx::document({{"name", "john"}}) }).get();
        CHECK(result);

        user.refresh_custom_user_data().get();
        auto res = user.get_custom_data();
        CHECK(res);
        CHECK((*res)["name"] == "john");

        std::promise<std::optional<bsoncxx>> function_promise;
        std::future<std::optional<bsoncxx>> function_future = function_promise.get_future();
        user.call_function("updateUserData", { bsoncxx::document({{"name", "jane"}}) }, [&function_promise](auto result, auto err) {
            function_promise.set_value(result);
        });
        auto results = function_future.get();
        CHECK(result);
        std::promise<void> refresh_promise;
        user.refresh_custom_user_data([&refresh_promise](auto err) {
            refresh_promise.set_value();
        });
        refresh_promise.get_future().get();
        res = user.get_custom_data();
        CHECK(res);
        CHECK((*res)["name"] == "jane");
    }
}
