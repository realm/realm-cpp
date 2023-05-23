#include "test_objects.hpp"
#include "main.hpp"


TEST_CASE("embedded_objects") {
    realm_path path;
    realm::db_config config;
    config.set_path(path);
    SECTION("observe") {
        auto realm = realm::open<Foo, EmbeddedFoo>(std::move(config));

        auto foo = Foo();
        EmbeddedFoo embedded;
        embedded.bar = 42;
        foo.foo = embedded;

        realm.write([&foo, &realm]() {
            realm.add(foo);
        });

        CHECK(foo.foo->bar == 42);
        EmbeddedFoo e_foo = *(*foo.foo);
        std::promise<bool> p;
        auto token = e_foo.observe([&p](auto change) {
            if (change.is_deleted) return;
            CHECK(change.object->bar == 84);
            p.set_value(true);
        });
        realm.write([&foo]() {
            foo.foo->bar = 84;
        });
        CHECK(foo.foo->bar == 84);
        realm.write([&foo, &realm] {
            realm.remove(foo);
        });
        auto future = p.get_future();
        switch (future.wait_for(std::chrono::seconds(5))) {
            case std::future_status::ready:
                CHECK(future.get());
                break;
            default:
                FAIL("observation timed out");
        }
    }
}
