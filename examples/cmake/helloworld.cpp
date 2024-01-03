#include <stdio.h>

#include <cpprealm/sdk.hpp>

#if __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#else
#include <uv.h>
#endif

namespace realm {
    struct Person {
        realm::primary_key<realm::object_id> _id;
        int64_t age;
        std::string name;
        std::string country;
    };
    REALM_SCHEMA(Person, _id, age, name, country)
}


int main() {
    auto app_config = realm::App::configuration();
    app_config.app_id = "MY_DEVICE_SYNC_APP_ID";
    auto app = realm::App(app_config);
    auto user = app.login(realm::App::credentials::anonymous()).get();
    auto config = user.flexible_sync_configuration();

    auto synced_realm = realm::db(std::move(config));

    auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                          subs.add<realm::Person>("country", [](auto &obj) {
                                                              return obj.country == "USA" || obj.country == "IE";
                                                          });
                                                      }).get();

    synced_realm.write([&synced_realm]() {
        realm::Person o;
        o._id = realm::object_id::generate();
        o.name = "foo";
        o.country = "USA";
        synced_realm.add(std::move(o));
    });

    synced_realm.get_sync_session()->wait_for_upload_completion().get();
    synced_realm.get_sync_session()->wait_for_download_completion().get();

    synced_realm.refresh();

    auto results = synced_realm.objects<realm::Person>();
    auto token = results.observe([](auto change) {
        std::cout << "collection changed " << change.collection->size() << " objects in collection" << "\n";
    });

    std::cout << "Enter a name:" << "\n";
    std::string name;
    std::cin >> name;

    synced_realm.write([&]() {
        realm::Person p;
        p._id = realm::object_id::generate();
        p.name = name;
        p.country = "USA";
        synced_realm.add(std::move(p));
    });

    std::cout << results.size() << std::endl;
    std::cout << "Make some changes to the data from Atlas and watch the callback in real time." << std::endl;

    // Instantiating the runloop here just ensures the program allows the sync thread
    // time to push the changes. If you prefer a synchronous approach you can do
    // `synced_realm.get_sync_session()->wait_for_upload_completion().get();`

#if REALM_HAVE_UV
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
#elif REALM_PLATFORM_APPLE
    CFRunLoopRun();
#else
#error "No EventLoop implementation selected"
#endif
    return 0;
}
