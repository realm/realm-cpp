#include "test_objects.hpp"
#include "main.hpp"
#include <realm/object-store/shared_realm.hpp>

TEST_CASE("cached realm") {
    SECTION("cached realm transactions") {
        realm_path path;
        realm::db_config config1;
        config1.set_path(path);
        auto o = AllTypesObject();
        {
            auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(std::move(config1));
            realm.write([&] {
                realm.add(o);
            });
        }

        realm::db_config config2;
        config2.set_path(path);
        {
            CHECK(o.is_managed());
            auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(std::move(config2));
            // If the cached realm is not returned this write would fail with a wrong transaction state.
            realm.write([&] {
                o.str_col = "foo";
            });
        }
        CHECK(*o.str_col == "foo");
    }
}

TEST_CASE("tsr") {
    realm_path path;
    realm::db_config config;
    config.set_path(path);
    auto realm = realm::open<Person, Dog>(std::move(config));

    Person person;
    person.name = "John";
    person.age = 17;
    Dog dog;
    dog.name = "Fido";
    person.dog = dog;

    realm.write([&realm, &person] {
        realm.add(person);
    });

    auto tsr = realm::thread_safe_reference<Person>(person);
    std::promise<void> p;
    auto t = std::thread([&tsr, &p, &path]() {
        realm::db_config config2;
        config2.set_path(path);
        auto realm = realm::open<Person, Dog>(std::move(config2));
        auto person = realm.resolve(std::move(tsr));
        CHECK(*person.age == 17);
        realm.write([&] { realm.remove(person); });
        p.set_value();
    });
    t.join();
    p.get_future().get();
}