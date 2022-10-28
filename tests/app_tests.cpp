#include "admin_utils.hpp"
#include "main.hpp"
#include "test_objects.hpp"

#include <realm/util/base64.hpp>
#include <external/json/json.hpp>

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
    realm::util::base64_encode(headerStr.data(), headerStr.length(), encoded_header.data(), encoded_header.size());
    std::string encoded_payload;
    encoded_payload.resize(realm::util::base64_encoded_size(payloadStr.length()));
    realm::util::base64_encode(payloadStr.data(), payloadStr.length(), encoded_payload.data(), encoded_payload.size());

    // Remove padding characters.
    while (encoded_header.back() == '=')
        encoded_header.pop_back();
    while (encoded_payload.back() == '=')
        encoded_payload.pop_back();

    std::string jwtPayload = encoded_header + "." + encoded_payload;

    auto mac = HMAC_SHA256("My_very_confidential_secretttttt", jwtPayload);

    std::string signature;
    signature.resize(realm::util::base64_encoded_size(mac.length()));
    realm::util::base64_encode(mac.data(), mac.length(), signature.data(), signature.size());
    while (signature.back() == '=')
        signature.pop_back();
    std::replace(signature.begin(), signature.end(), '+', '-');
    std::replace(signature.begin(), signature.end(), '/', '_');

    return jwtPayload + "." + signature;
}

TEST_CASE("app", "[app]") {
    SECTION("auth_providers_promise") {
        auto app_id = Admin::shared().create_app();
        auto app = realm::App(app_id, Admin::shared().base_url());

        auto run_login = [&app](realm::App::Credentials&& credentials) {
            auto user = app.login(std::move(credentials)).get_future().get();
            CHECK(!user.access_token().empty());
        };

        app.register_user("foo@mongodb.com", "foobar").get_future().get();
        run_login(realm::App::Credentials::username_password("foo@mongodb.com", "foobar"));
        run_login(realm::App::Credentials::anonymous());
        run_login(realm::App::Credentials::custom(create_jwt(app_id)));
    }

    SECTION("auth_providers_completion_hander") {
        auto app_id = Admin::shared().create_app();
        auto app = realm::App(app_id, Admin::shared().base_url());

        app.register_user("foo@mongodb.com", "foobar").get_future().get();
        std::promise<realm::User> p;
        app.login(realm::App::Credentials::username_password("foo@mongodb.com", "foobar"), [&](auto user, auto err){
            p.set_value(user);
        });
        auto user = p.get_future().get();
        CHECK(user.state() == realm::User::state::logged_in);
    }

    SECTION("logout_anonymous") {
        auto app_id = Admin::shared().create_app();
        auto app = realm::App(app_id, Admin::shared().base_url());
        auto user = app.login(realm::App::Credentials::anonymous()).get_future().get();
        CHECK(!user.access_token().empty());
        CHECK(user.state() == realm::User::state::logged_in);

        user.log_out().get_future().get();
        CHECK(user.state() == realm::User::state::removed);
    }

    SECTION("logout") {
        auto app_id = Admin::shared().create_app();
        auto app = realm::App(app_id, Admin::shared().base_url());
        app.register_user("foo@mongodb.com", "foobar").get_future().get();
        auto user = app.login(
                           realm::App::Credentials::username_password("foo@mongodb.com", "foobar")).get_future().get();
        CHECK(!user.access_token().empty());
        CHECK(user.state() == realm::User::state::logged_in);

        user.log_out().get_future().get();
        CHECK(user.state() == realm::User::state::logged_out);
    }

    SECTION("logout_completion_handler") {
        auto app_id = Admin::shared().create_app();
        auto app = realm::App(app_id, Admin::shared().base_url());
        auto user = app.login(realm::App::Credentials::anonymous()).get_future().get();
        CHECK(user.state() == realm::User::state::logged_in);

        std::promise<void> p;
        user.log_out([&p](auto err){
          CHECK(!err);
          p.set_value();
        });
        p.get_future().get();

        CHECK(user.state() == realm::User::state::removed);
    }

    SECTION("function_and_custom_user_data") {
        auto app_id = Admin::shared().create_app();
        auto app = realm::App(app_id, Admin::shared().base_url());
        app.register_user("foo@mongodb.com", "foobar").get_future().get();
        auto user = app.login(
                           realm::App::Credentials::username_password("foo@mongodb.com", "foobar")).get_future().get();
        auto result = user.call_function("updateUserData", {realm::bson::BsonDocument({{"name", "john"}})}).get_future().get();
        CHECK(result);

        user.refresh_custom_user_data().get_future().get();
        CHECK((*user.custom_data())["name"] == "john");
        std::promise<std::optional<realm::bson::Bson>> function_promise;
        user.call_function("updateUserData", {realm::bson::BsonDocument({{"name", "jane"}})}, [&function_promise](auto result, auto err) {
          function_promise.set_value(result);
        });
        auto results = function_promise.get_future().get();
        CHECK(result);
        std::promise<void> refresh_promise;
        user.refresh_custom_user_data([&refresh_promise](auto err){
          refresh_promise.set_value();
        });
        refresh_promise.get_future().get();
        CHECK((*user.custom_data())["name"] == "jane");
    }
}
