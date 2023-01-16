#include "main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("run loop", "[results]") {
    realm_path path;
#if __APPLE__
    SECTION("observation cfrunloop") {
        auto dog = Dog();
        auto realm = realm::open<Dog>({path});
        realm.write([&realm, &dog] {
            realm.add(dog);
        });

        std::promise<bool> p;
        auto token = dog.observe([&p](realm::ObjectChange<Dog> const& change) {
            std::cout << "Called back" << std::endl; // never hit
            p.set_value(true);
            CFRunLoopStop(CFRunLoopGetCurrent());
        }, true);

        realm.write([&dog] {
            dog.name = "Wolfie2";
        });

        auto future = p.get_future();
        CFRunLoopRun();
        switch (future.wait_for(std::chrono::seconds(5))) {
            case std::future_status::ready:
                CHECK(future.get());
                break;
            default:
                FAIL("observation timed out");
        }
    }
#endif
}