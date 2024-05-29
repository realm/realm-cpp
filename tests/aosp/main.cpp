#include <iostream>
#include "cpprealm/sdk.hpp"

namespace realm {
    struct Dog;
    struct Person {
        primary_key<int64_t> _id;
        std::string name;
        int64_t age = 0;
        Dog* dog;
    };
    REALM_SCHEMA(Person, _id, name, age, dog)
    struct Dog {
        primary_key<int64_t> _id;
        std::string name;
        std::string name2;
        std::string name3;
        int64_t foo2 = 0;
        std::string name4;

        int64_t age = 0;
        linking_objects<&Person::dog> owners;
    };
    REALM_SCHEMA(Dog,
                 _id,
                 name,
                 name2,
                 name3,
                 foo2,
                 name4,
                 age,
                 owners)
}

int main(int argc, char *argv[]) {
    realm::db_config config;
    auto realm = realm::db(std::move(config));
    realm::Person p;
    auto managed_obj = realm.write([&realm, &p] {
        return realm.add(std::move(p));
    });

    // Make sure app services symbols exist
    auto dummy_app = realm::App(realm::App::configuration({"NA"}));

    std::cout << "Realm C++ ran successfully." << std::endl;
    return 0;
}