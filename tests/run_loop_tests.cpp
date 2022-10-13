#include "main.hpp"
#include "test_objects.hpp"

using namespace realm;

TEST_CASE("run loop", "[results]") {
    realm_path path;
//    SECTION("observation_dispatch_queue") {
//        auto dog = Dog();
//        auto realm = realm::open<Dog>({path});
//        realm.write([&realm, &dog] {
//            realm.add(dog);
//        });
//
//        auto queue = dispatch_queue_create("test--", nullptr);
//        std::promise<bool> p;
//        auto token = dog.observe<Dog>([&p](realm::ObjectChange<Dog> const& change) {
//            std::cout << "Called back" << std::endl; // never hit
//            p.set_value(true);
//        }, queue);
//
//
//        realm.write([&dog] {
//            dog.name = "Wolfie2";
//        });
//
//        auto future = p.get_future();
//        switch (future.wait_for(std::chrono::seconds(5))) {
//            case std::__1::future_status::ready:
//                CHECK(future.get());
//                break;
//            default:
//                FAIL("observation timed out");
//        }
//    }

    SECTION("observation std::thread") {
        auto dog = Dog();
        auto realm = realm::open<Dog>({path});
        realm.write([&realm, &dog] {
            realm.add(dog);
        });

        std::promise<bool> p;
        auto token = dog.observe<Dog>([&p](realm::ObjectChange<Dog> const& change) {
            std::cout << "Called back" << std::endl; // never hit
            p.set_value(true);
        });

        realm.write([&dog] {
            dog.name = "Wolfie2";
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