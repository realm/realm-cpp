#include "test_objects.hpp"
#include "main.hpp"


TEST_CASE("embedded_objects") {
    realm_path path;
    SECTION("observe") {
        auto realm = realm::open<Foo, EmbeddedFoo>({.path=path});

        auto foo = Foo();
        foo.foo = EmbeddedFoo{.bar=42};

        realm.write([&foo, &realm]() {
            realm.add(foo);
        });

        CHECK((*foo.foo).bar == 42);
        bool did_run;
        EmbeddedFoo e_foo = (*foo.foo);
        auto token = e_foo.observe<EmbeddedFoo>([&did_run](auto change) {
            CHECK(change.property_changes.size() == 1);
            CHECK(change.property_changes[0].old_value == std::nullopt);
            CHECK(std::get<int>(*(change.property_changes[0].new_value)) == 84);
            CHECK(change.object->bar == 84);
            did_run = true;
        });
        realm.write([&foo]() {
            (*foo.foo).bar = 84;
        });
        CHECK((*foo.foo).bar == 84);
        realm.write([&foo, &realm] {
            realm.remove(foo);
        });
        CHECK(did_run);
    }
}
