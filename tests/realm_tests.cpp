#include "test_objects.hpp"
#include "main.hpp"
#include <realm/object-store/shared_realm.hpp>

TEST_CASE("cached realm") {
    SECTION("cached realm transactions") {
        realm_path path;
        auto o = AllTypesObject();
        {
            auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({path});
            realm.write([&] {
                realm.add(o);
            });
        }

        {
            CHECK(o.is_managed());
            auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({path});
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
    auto realm = realm::open<Person, Dog>({path});

    auto person = Person { .name = "John", .age = 17 };
    person.dog = Dog {.name = "Fido"};

    realm.write([&realm, &person] {
        realm.add(person);
    });

    auto tsr = realm::thread_safe_reference<Person>(person);
    std::promise<void> p;
    auto t = std::thread([&tsr, &p, &path]() {
        auto realm = realm::open<Person, Dog>({path});
        auto person = realm.resolve(std::move(tsr));
        CHECK(*person.age == 17);
        realm.write([&] { realm.remove(person); });
        p.set_value();
    });
    t.join();
    p.get_future().get();
}