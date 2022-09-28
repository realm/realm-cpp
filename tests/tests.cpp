#include "test_utils.hpp"
#include "test_objects.hpp"
#include "sync_test_utils.hpp"
#include "admin_utils.hpp"

#include <realm/util/base64.hpp>
#include <realm/object-store/impl/realm_coordinator.hpp>
#include <external/json/json.hpp>

#if REALM_PLATFORM_APPLE
#import <CommonCrypto/CommonHMAC.h>
#else
#include <openssl/sha.h>
#include <openssl/hmac.h>
#endif

#include <semaphore>

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

TEST(log_out_anonymous) {
    auto app = realm::App(Admin::Session::shared.create_app<AllTypesObject, AllTypesObjectLink>({"str_col", "_id"}), "http://localhost:9090");

    auto user1 = co_await app.login(realm::App::credentials::anonymous());
    CHECK_EQUALS((user1.state() == realm::user::state::logged_in), true);

    co_await user1.log_out();
    CHECK_EQUALS((int)user1.state(), (int)realm::user::state::removed);

    // Test with completion handler
    auto user2 = co_await app.login(realm::App::credentials::anonymous());
    bool did_run = false;
    user2.log_out([&did_run](auto opt_error) {
        did_run = true;
    });
    CHECK(did_run);
    CHECK_EQUALS((int)user2.state(), (int)realm::user::state::removed);

    co_return;
}

TEST(log_out_username_password) {
    auto app = realm::App(Admin::Session::shared.create_app<AllTypesObject, AllTypesObjectLink>({"str_col", "_id"}), "http://localhost:9090");

    co_await app.register_user("foo@mongodb.com", "foobar");
    auto user1 = co_await app.login(realm::App::credentials::username_password("foo@mongodb.com", "foobar"));
    CHECK_EQUALS((user1.state() == realm::user::state::logged_in), true);

    co_await user1.log_out();
    CHECK_EQUALS((int)user1.state(), (int)realm::user::state::logged_out);

    // Test with completion handler
    auto user2 = co_await app.login(realm::App::credentials::username_password("foo@mongodb.com", "foobar"));
    bool did_run = false;
    user2.log_out([&did_run](auto opt_error) {
        did_run = true;
    });
    CHECK(did_run);
    CHECK_EQUALS((int)user2.state(), (int)realm::user::state::logged_out);

    co_return;
}

TEST(auth_providers_completion_handler) {
    auto app_id = Admin::Session::shared.create_app<AllTypesObject, AllTypesObjectLink>({"str_col", "_id"});
    auto app = realm::App(app_id, "http://localhost:9090");

    auto run_login = [&app](realm::App::credentials&& credentials) {
        bool did_run = false;
        realm::user user;
        app.login(credentials, [&did_run, &user](realm::user u, std::optional<realm::app_error> e) {
            user = std::move(u);
            CHECK(!e);
            did_run = true;
        });
        CHECK(did_run);
        CHECK_EQUALS((user.state() == realm::user::state::logged_in), true);
    };

    co_await app.register_user("foo@mongodb.com", "foobar");
    run_login(realm::App::credentials::username_password("foo@mongodb.com", "foobar"));
    run_login(realm::App::credentials::anonymous());
    run_login(realm::App::credentials::custom(create_jwt(app_id)));

    co_return;
}

TEST(async_open_completion) {
    std::binary_semaphore sema{0};
    auto app = realm::App(Admin::Session::shared.create_app<AllTypesObject, AllTypesObjectLink>({"str_col", "_id"}), "http://localhost:9090");
    realm::user user;
    app.login(realm::App::credentials::anonymous(), [&](auto u, auto opt_error) {
        CHECK(!opt_error)
        user = u;
    });
    auto flx_sync_config = user.flexible_sync_configuration();
    realm::thread_safe_reference<realm::db<AllTypesObject, AllTypesObjectLink>> tsr;

    realm::async_open_realm<AllTypesObject, AllTypesObjectLink>(flx_sync_config, [&tsr, &sema](realm::thread_safe_reference<realm::db<AllTypesObject, AllTypesObjectLink>> t, std::exception_ptr e) {
        tsr = std::move(t);
        sema.release();
    });

    sema.acquire();
    auto realm = tsr.resolve();

    realm.write([&](){
        for (auto obj : realm.objects<AllTypesObject>()) {
            realm.remove(obj);
        }
    });

    CHECK_EQUALS(realm.objects<AllTypesObject>().size(), 0)

    co_return;
}

TEST(custom_user_data) {
    auto app = realm::App(Admin::Session::shared.create_app<AllTypesObject, AllTypesObjectLink>({"str_col", "_id"}), "http://localhost:9090");
    co_await app.register_user("foo@mongodb.com", "foobar");
    auto user = co_await app.login(realm::App::credentials::username_password("foo@mongodb.com", "foobar"));

    user.call_function("updateUserData", {bson::BsonDocument({{"name", "john"}})}, [](auto&& res, auto err) {
        CHECK(!err);
    });

    user.refresh_custom_user_data([](auto opt_err) {
        CHECK(!opt_err)
    });

    auto name = (*user.custom_data())["name"];
    CHECK_EQUALS(name, "john")

    co_await user.call_function("updateUserData", {bson::BsonDocument({{"name", "jane"}})});
    co_await user.refresh_custom_user_data();
    name = (*user.custom_data())["name"];
    CHECK_EQUALS(name, "jane")

    co_return;
}

TEST(flx_sync) {
    auto app = realm::App(Admin::Session::shared.create_app<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({"str_col", "_id"}), "http://localhost:9090");
    auto user = co_await app.login(realm::App::Credentials::anonymous());
    auto flx_sync_config = user.flexible_sync_configuration();

    try {
        auto tsr = co_await realm::async_open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(flx_sync_config);
        auto synced_realm = tsr.resolve();

        auto update_success = co_await synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet &subs) {
            subs.clear();
        });
        CHECK_EQUALS(update_success, true);
        CHECK_EQUALS(synced_realm.subscriptions().size(), 0);

        update_success = co_await synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet &subs) {
            subs.add<AllTypesObject>("foo-strings", [](auto &obj) {
                return obj.str_col == "foo";
            });
        });
        CHECK_EQUALS(update_success, true);
        CHECK_EQUALS(synced_realm.subscriptions().size(), 1);

        auto sub = *synced_realm.subscriptions().find("foo-strings");
        CHECK_EQUALS(sub.name(), "foo-strings")
        CHECK_EQUALS(sub.object_class_name(), "AllTypesObject")
        CHECK_EQUALS(sub.query_string(), "str_col == \"foo\"")

        auto non_existent_sub = synced_realm.subscriptions().find("non-existent");
        CHECK_EQUALS((non_existent_sub == std::nullopt), true)

        synced_realm.write([&synced_realm]() {
            synced_realm.add(AllTypesObject{._id=1, .str_col="foo"});
        });

        co_await test::wait_for_sync_uploads(user);
        co_await test::wait_for_sync_downloads(user);
        synced_realm.write([]() {}); // refresh realm
        auto objs = synced_realm.objects<AllTypesObject>();

        CHECK_EQUALS(objs[0]->_id, 1)

        update_success = co_await synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet &subs) {
            subs.update_subscription<AllTypesObject>("foo-strings", [](auto &obj) {
                return obj.str_col == "bar" && obj._id == 123;
            });
        });

        sub = *synced_realm.subscriptions().find("foo-strings");
        CHECK_EQUALS(sub.name(), "foo-strings");
        CHECK_EQUALS(sub.object_class_name(), "AllTypesObject");
        CHECK_EQUALS(sub.query_string(), "str_col == \"bar\" and _id == 123");

        synced_realm.write([&synced_realm]() {
            synced_realm.add(AllTypesObject{._id=123, .str_col="bar"});
        });

        co_await test::wait_for_sync_uploads(user);
        co_await test::wait_for_sync_downloads(user);

        synced_realm.write([]() {}); // refresh realm
        objs = synced_realm.objects<AllTypesObject>();
        CHECK_EQUALS(objs.size(), 1);

        update_success = co_await synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet &subs) {
            subs.update_subscription<AllTypesObject>("foo-strings");
        });

        sub = *synced_realm.subscriptions().find("foo-strings");
        CHECK_EQUALS(sub.name(), "foo-strings");
        CHECK_EQUALS(sub.object_class_name(), "AllTypesObject");
        CHECK_EQUALS(sub.query_string(), "TRUEPREDICATE");

        co_await test::wait_for_sync_uploads(user);
        co_await test::wait_for_sync_downloads(user);

        synced_realm.write([]() {}); // refresh realm
        objs = synced_realm.objects<AllTypesObject>();
        CHECK_EQUALS(objs.size(), 2);
    } catch (const std::exception& err) {
        REALM_TERMINATE("Should not happen");
    }
    co_return;
}

TEST(tsr) {
    auto realm = realm::open<Person, Dog>({.path=path});

    auto person = Person { .name = "John", .age = 17 };
    person.dog = Dog {.name = "Fido"};

    realm.write([&realm, &person] {
        realm.add(person);
    });

    auto tsr = realm::thread_safe_reference<Person>(person);
    std::condition_variable cv;
    std::mutex cv_m;
    bool done;
    auto t = std::thread([&cv, &tsr, &done, &path]() {
        auto realm = realm::open<Person, Dog>({.path=path});
        auto person = realm.resolve(std::move(tsr));
        CHECK_EQUALS(*person.age, 17);
        realm.write([&] { realm.remove(person); });
    });
    t.join();
    co_return;
}

TEST(query_) {
    auto realm = realm::open<Person, Dog>({.path=path});

    auto person = Person { .name = "John", .age = 42 };
    realm.write([&realm, &person](){
        realm.add(person);
    });

    auto results = realm.objects<Person>().where("age > $0", {42});
    CHECK_EQUALS(results.size(), 0);
    results = realm.objects<Person>().where("age = $0", {42});
    CHECK_EQUALS(results.size(), 1);
    std::unique_ptr<Person> john = results[0];
    CHECK_EQUALS(john->age, 42);
    CHECK_EQUALS(john->name, "John");

    co_return;
}

TEST(results_notifications) {
    auto realm = realm::open<Person, Dog>({.path=path});

    auto person = Person { .name = "John", .age = 42 };
    realm.write([&realm, &person](){
        realm.add(person);
    });
    auto results = realm.objects<Person>();

    bool did_run = false;

    realm::results_change<Person> change;

    auto require_change = [&] {
        auto token = results.observe([&](realm::results_change<Person> c) {
            did_run = true;
            change = std::move(c);
        });
        return token;
    };

    auto token = require_change();
    auto person2 = Person { .name = "Jane", .age = 24 };
    realm.write([&realm, &person2](){
        realm.add(person2);
    });
    realm.write([] { });

    CHECK_EQUALS(change.insertions.size(), 1);
    CHECK_EQUALS(change.collection->size(), 2);
    CHECK_EQUALS(did_run, true);

    co_return;
}

TEST(results_notifications_insertions) {
    auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded, Dog>({.path=path});
    realm.write([&realm] {
        realm.add(AllTypesObject { ._id = 1 });
    });

    bool did_run = false;

    realm::results_change<AllTypesObject> change;

    int callback_count = 0;
    auto results = realm.objects<AllTypesObject>();
    auto require_change = [&] {
        auto token = results.observe([&](realm::results_change<AllTypesObject> c) {
            CHECK_EQUALS(c.collection , &results);
            callback_count++;
            change = std::move(c);
        });
        return token;
    };

    auto token = require_change();
    realm.write([&realm] {
        realm.add(AllTypesObject { ._id = 2 });
    });

    realm.write([] { });

    CHECK_EQUALS(change.insertions.size(), 1);
    CHECK_EQUALS(change.deletions.size(), 0);
    CHECK_EQUALS(change.modifications.size(), 0);

    realm.write([&realm] {
        realm.add(AllTypesObject { ._id = 3 });
        realm.add(AllTypesObject { ._id = 4 });
    });

    realm.write([] { });

    CHECK_EQUALS(change.insertions.size(), 2);
    CHECK_EQUALS(change.deletions.size(), 0);
    CHECK_EQUALS(change.modifications.size(), 0);
    CHECK_EQUALS(callback_count, 3);

    co_return;
}

TEST(results_notifications_deletions) {
    auto obj = AllTypesObject();

    auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded, Dog>({.path=path});
    realm.write([&realm, &obj] {
        realm.add(obj);
    });

    bool did_run = false;
    realm::results_change<AllTypesObject> change;
    auto results = realm.objects<AllTypesObject>();

    auto require_change = [&] {
        auto token = results.observe([&](realm::results_change<AllTypesObject> c) {
            did_run = true;
            change = std::move(c);
        });
        return token;
    };

    auto token = require_change();
    realm.write([&realm, &obj] {
        realm.remove(obj);
    });
    realm.write([] { });
    CHECK_EQUALS(change.deletions.size(), 1);
    CHECK_EQUALS(change.insertions.size(), 0);
    CHECK_EQUALS(change.modifications.size(), 0);
    CHECK_EQUALS(did_run, true);

    co_return;
}

TEST(results_notifications_modifications) {
    auto obj = AllTypesObject();

    auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded, Dog>({.path=path});
    realm.write([&realm, &obj] {
        realm.add(obj);
    });

    bool did_run = false;

    realm::results_change<AllTypesObject> change;
    auto results = realm.objects<AllTypesObject>();

    auto require_change = [&] {
        auto token = results.observe([&](realm::results_change<AllTypesObject> c) {
            did_run = true;
            change = std::move(c);
        });
        return token;
    };

    auto token = require_change();
    realm.write([&realm, &obj] {
        obj.str_col = "foobar";
    });
    realm.write([] { });
    CHECK_EQUALS(change.modifications.size(), 1);
    CHECK_EQUALS(change.insertions.size(), 0);
    CHECK_EQUALS(change.deletions.size(), 0);
    CHECK_EQUALS(did_run, true);

    co_return;
}

TEST(binary) {
    auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({.path=path});
    auto obj = AllTypesObject();
    obj.binary_col.push_back(1);
    obj.binary_col.push_back(2);
    obj.binary_col.push_back(3);
    realm.write([&realm, &obj] {
        realm.add(obj);
    });
    realm.write([&realm, &obj] {
        obj.binary_col.push_back(4);
    });
    CHECK_EQUALS(obj.binary_col[0], 1);
    CHECK_EQUALS(obj.binary_col[1], 2);
    CHECK_EQUALS(obj.binary_col[2], 3);
    CHECK_EQUALS(obj.binary_col[3], 4);
    co_return;
}

TEST(date) {
    auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({.path=path});
    auto obj = AllTypesObject();
    CHECK_EQUALS(obj.date_col, std::chrono::time_point<std::chrono::system_clock>{});
    auto now = std::chrono::system_clock::now();
    obj.date_col = now;
    CHECK_EQUALS(obj.date_col, now);
    realm.write([&realm, &obj] {
        realm.add(obj);
    });
    CHECK_EQUALS(obj.date_col, now);
    realm.write([&realm, &obj] {
        obj.date_col += std::chrono::seconds(42);
    });
    CHECK_EQUALS(obj.date_col, now + std::chrono::seconds(42));
    co_return;
}

TEST(login_username_password) {
    auto app_id = Admin::Session::shared.create_app();
    auto app = realm::App(app_id, "http://localhost:9090");
    try {
        co_await app.login(realm::App::credentials::apple("id_token"));
    } catch (const std::exception& err) {
        // TODO: Log error check
    } catch (...) {
    }

    co_await app.register_user("foo@mongodb.com", "foobar");
    auto user = co_await app.login(realm::App::credentials::username_password("foo@mongodb.com", "foobar"));
    CHECK(!user.access_token().empty())
    co_return;
}

TEST(embedded) {
    auto realm = realm::open<Foo, EmbeddedFoo>({.path=path});

    auto foo = Foo();
    foo.foo = EmbeddedFoo{.bar=42};

    realm.write([&foo, &realm](){
        realm.add(foo);
    });

    CHECK_EQUALS((*foo.foo).bar, 42)
    bool did_run;
    EmbeddedFoo e_foo = (*foo.foo);
    auto token = e_foo.observe<EmbeddedFoo>([&did_run](auto change){
        CHECK_EQUALS(change.object->bar, 84)
        did_run = true;
    });
    realm.write([&foo](){
        (*foo.foo).bar = 84;
    });
    CHECK_EQUALS((*foo.foo).bar, 84)
    realm.write([&foo, &realm]{
        realm.remove(foo);
    });
    CHECK(did_run)
    co_return;
}
