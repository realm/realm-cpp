#include <stdio.h>

#include <cpprealm/sdk.hpp>
#include <cpprealm/experimental/sdk.hpp>

#include <realm/util/cf_ptr.hpp>
#include <CoreFoundation/CoreFoundation.h>

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
    auto app = realm::App("MY_DEVICE_SYNC_APP_ID");
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
    auto token = results.observe([](auto change) {
        std::cout << "collection changed " << change.collection->size() << " objects in collection" << "\n";
    });

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
    std::cout << "Make some changes to the data from Atlas and watch the callback in real time." << std::endl;

    // Instantiating the runloop here just ensures the program allows the sync thread
    // time to push the changes. If you prefer a synchronous approach you can do
    // `synced_realm.get_sync_session()->wait_for_upload_completion().get();`

    CFRunLoopRun();
    return 0;
}
