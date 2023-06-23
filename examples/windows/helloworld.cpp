#include <stdio.h>
#include <cpprealm/sdk.hpp>
#include <cpprealm/experimental/sdk.hpp>

#if _WIN32
#include <uv.h>
#elif REALM_PLATFORM_APPLE
#include <realm/util/cf_ptr.hpp>
#include <CoreFoundation/CoreFoundation.h>
#else
#error "No EventLoop implementation selected"
#endif

namespace realm::experimental {
    struct Person {
        realm::experimental::primary_key<realm::object_id> _id;
        int64_t age;
        std::string name;
        std::string country;
    };
    REALM_SCHEMA(Person, _id, age, name, country)
}


int main() {
    auto app = realm::App("windowstest-cowqq");
    auto user = app.login(realm::App::credentials::anonymous()).get();
    auto config = user.flexible_sync_configuration();

    auto synced_realm = realm::experimental::db(std::move(config));

    auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                     subs.add<realm::experimental::Person>("foo-strings", [](auto &obj) {
                                                         return obj.country == "USA";
                                                     });
                                                 }).get();

    synced_realm.write([&synced_realm]() {
        realm::experimental::Person o;
        o._id = realm::object_id::generate();
        o.name = "foo";
        o.country = "USA";
        synced_realm.add(std::move(o));
    });

    synced_realm.get_sync_session()->wait_for_upload_completion().get();
    synced_realm.get_sync_session()->wait_for_download_completion().get();

    synced_realm.refresh();

    auto results = synced_realm.objects<realm::experimental::Person>();

    std::cout << "Enter a name:" << "\n";
    std::string name;
    std::cin >> name;

    synced_realm.write([&]() {
        realm::experimental::Person p;
        p._id = realm::object_id::generate();
        p.name = name;
        p.country = "USA";
        synced_realm.add(std::move(p));
    });

    std::cout << results.size() << std::endl;

    // Instantiating the runloop here just ensures the program allows the sync thread
    // time to push the changes. If you prefer a synchronous approach you can do
    // `synced_realm.get_sync_session()->wait_for_upload_completion().get();`

#if _WIN32
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
#elif REALM_PLATFORM_APPLE
    CFRunLoopRun();
#else
#error "No EventLoop implementation selected"
#endif

    return 0;
}
