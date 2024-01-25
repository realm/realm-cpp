#include <stdio.h>

#include <cpprealm/sdk.hpp>

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
    auto config = realm::db_config();
    auto realm = realm::db(std::move(config));
    std::cout << "ok" << std::endl;
    return 0;
}
