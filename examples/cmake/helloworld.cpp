#include <chrono>
#include <semaphore>
#include <stdio.h>
#include <thread>
#include <cpprealm/sdk.hpp>
#include <realm/object-store/sync/sync_session.hpp>

struct FooObject: realm::object {

    realm::persisted<int> _id;
    realm::persisted<bool> bool_col;
    realm::persisted<std::string> str_col;
    realm::persisted<std::chrono::time_point<std::chrono::system_clock>> date_col;
    realm::persisted<realm::uuid> uuid_col;
    realm::persisted<std::vector<std::uint8_t>> binary_col;

    using schema = realm::schema<
    "AllTypesObject",
    realm::property<"_id", &FooObject::_id, true>,
    realm::property<"bool_col", &FooObject::bool_col>,
    realm::property<"str_col", &FooObject::str_col>,
    realm::property<"date_col", &FooObject::date_col>,
    realm::property<"uuid_col", &FooObject::uuid_col>,
    realm::property<"binary_col", &FooObject::binary_col>>;
};

realm::task<void> wait_for_sync_uploads(const realm::User& user) {
    auto sync_sessions = user.m_user->sync_manager()->get_all_sessions();
    auto session = sync_sessions[0];
    co_await realm::make_awaitable<std::error_code>([&] (auto cb) {
        session->wait_for_upload_completion(cb);
    });
    co_return;
}

realm::task<void> wait_for_sync_downloads(const realm::User& user) {
    auto sync_sessions = user.m_user->sync_manager()->get_all_sessions();
    auto session = sync_sessions[0];
    co_await realm::make_awaitable<std::error_code>([&] (auto cb) {
        session->wait_for_download_completion(cb);
    });
    co_return;
}

realm::task<realm::db_config> task;

realm::task<realm::db_config> run_realm() {
    auto app = realm::App("cpp-sdk-2-pgjiz");
    auto user = co_await app.login(realm::App::Credentials::anonymous());

    auto flx_sync_config = user.flexible_sync_configuration();
    std::cout << flx_sync_config.path << std::endl;

    auto tsr = co_await realm::async_open<FooObject>(flx_sync_config);
    auto synced_realm = tsr.resolve();

    auto update_success = co_await synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet& subs) {
        subs.clear();
    });

    update_success = co_await synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet& subs) {
        subs.add<FooObject>("foo-strings", [](auto& obj) {
            return obj.str_col != "alex"; // sync all objects where name does not equal 'alex'
        });
    });

    auto person = FooObject();
    person._id = 1;
    person.str_col = "john";

    synced_realm.write([&synced_realm, &person]() {
        synced_realm.add(person);
    });

    auto token = person.observe<FooObject>([](auto&& change) {
        std::cout << "property changed" << std::endl;
    });

    co_await wait_for_sync_uploads(user);
    co_await wait_for_sync_downloads(user);

    synced_realm.write([&synced_realm, &person]() {
        person.str_col = "sarah";
    });

    co_await wait_for_sync_uploads(user);
    co_await wait_for_sync_downloads(user);

    synced_realm.write([&synced_realm, &person]() {
        person.str_col = "bob";
    });

    co_await wait_for_sync_uploads(user);
    co_await wait_for_sync_downloads(user);

    co_return flx_sync_config;
}

int main() {
    task = run_realm();
    while (task.handle.done() == false) { }
    return 0;
}
