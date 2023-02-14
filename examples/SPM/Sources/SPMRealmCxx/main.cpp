#include <stdio.h>
#include <cpprealm/sdk.hpp>

struct Person: realm::object<Person> {
    realm::persisted<realm::object_id> _id;
    realm::persisted<std::string> name;
    realm::persisted<int64_t> age;

    static constexpr auto schema = realm::schema("Person",
            realm::property<&Person::_id, true>("_id"),
            realm::property<&Person::name>("name"),
            realm::property<&Person::age>("age")
    );
};

int main() {
    auto app = realm::App("cpptest-mkuaa");
    auto user = app.login(realm::App::credentials::anonymous()).get_future().get();
    auto config = user.flexible_sync_configuration();

    auto realm = realm::open<Person>(std::move(config));
    realm.subscriptions().update([](realm::mutable_sync_subscription_set& subs) {
        if (!subs.find("all_people")) {
            subs.add<Person>("all_people");
        }
    }).get_future().get();

    auto results = realm.objects<Person>();
    auto token = results.observe([](realm::results_change<Person> change) {
        std::cout << "collection changed " << change.collection->size() << " objects in collection" << "\n";
    });

    std::cout << "Enter a name:" << "\n";
    std::string name;
    std::cin >> name;

    realm.write([&]() {
        Person p;
        p._id = realm::object_id::generate();
        p.name = name;
        realm.add(p);
    });

    CFRunLoopRun();
    return 0;
}
