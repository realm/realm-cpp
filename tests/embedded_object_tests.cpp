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
        EmbeddedFoo e_foo = (*foo.foo);
        std::promise<bool> p;
        auto token = e_foo.observe<EmbeddedFoo>([&p](auto change) {
            CHECK(change.object->bar == 84);
            p.set_value(true);
        });
        realm.write([&foo]() {
            (*foo.foo).bar = 84;
        });
        CHECK((*foo.foo).bar == 84);
        realm.write([&foo, &realm] {
            realm.remove(foo);
        });
        auto future = p.get_future();
        switch (future.wait_for(std::chrono::seconds(5))) {
            case std::__1::future_status::ready:
                CHECK(future.get());
                break;
            default:
                FAIL("observation timed out");
        }
    }
}
