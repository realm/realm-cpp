#include "test_utils.hpp"
#include "test_objects.hpp"

#include <realm/object-store/impl/realm_coordinator.hpp>


TEST(all) {
    auto realm = realm::open<Person, Dog>({.path=path});

    auto person = Person();
    person.name = "John";
    person.age = 17;
    person.dog = Dog{.name = "Fido"};

    realm.write([&realm, &person] {
        realm.add(person);
    });

    CHECK_EQUALS(*person.name, "John");
    CHECK_EQUALS(*person.age, 17);
    auto dog = **person.dog;
    CHECK_EQUALS(*dog.name, "Fido");

    bool did_run = false;
    auto token = person.observe<Person>([&did_run](auto&& change) {
        CHECK_EQUALS(change.property.name, "age");
        CHECK_EQUALS(std::any_cast<int>(*change.property.new_value), 19);
        did_run = true;
    });

    realm.write([&person] {
        person.age += 2;
    });

    CHECK_EQUALS(*person.age, 19);

    auto persons = realm.objects<Person>();
    CHECK_EQUALS(persons.size(), 1);

    std::vector<Person> people;
    std::copy(persons.begin(), persons.end(), std::back_inserter(people));
    for (auto& person:people) {
        realm.write([&person, &realm]{
            realm.remove(person);
        });
    }
    CHECK_EQUALS(did_run, true);

    CHECK_EQUALS(persons.size(), 0);
    auto app = realm::App("car-wsney");
    auto user = co_await app.login(realm::App::Credentials::anonymous());

    auto tsr = co_await user.realm<AllTypesObject, AllTypesObjectLink>("foo");
    auto synced_realm = tsr.resolve();
    synced_realm.write([&synced_realm]() {
        synced_realm.add(AllTypesObject{._id=1});
    });

    CHECK_EQUALS(*synced_realm.object<AllTypesObject>(1)._id, 1);

    co_return;
}

TEST(thread_safe_reference) {
    auto realm = realm::open<Person, Dog>({.path=path});

    auto person = Person { .name = "John", .age = 17 };
    person.dog = Dog {.name = "Fido"};

    realm.write([&realm, &person] {
        realm.add(person);
    });

    auto tsr = realm::thread_safe_reference<Person>(person);
    std::condition_variable cv;
    std::mutex cv_m;
    bool done;
    auto t = std::thread([&cv, &tsr, &done, &path]() {
        auto realm = realm::open<Person, Dog>({.path=path});
        auto person = realm.resolve(std::move(tsr));
        CHECK_EQUALS(*person.age, 17);
        realm.write([&] { realm.remove(person); });
    });
    t.join();
    co_return;
}

TEST(query) {
    auto realm = realm::open<Person, Dog>({.path=path});

    auto person = Person { .name = "John", .age = 42 };
    realm.write([&realm, &person](){
        realm.add(person);
    });

    auto results = realm.objects<Person>().where("age > $0", {42});
    CHECK_EQUALS(results.size(), 0);
    results = realm.objects<Person>().where("age = $0", {42});
    CHECK_EQUALS(results.size(), 1);
    co_return;
}

TEST(binary) {
    auto realm = realm::open<AllTypesObject, AllTypesObjectLink>({.path=path});
    auto obj = AllTypesObject();
    obj.binary_col.push_back(1);
    obj.binary_col.push_back(2);
    obj.binary_col.push_back(3);
    realm.write([&realm, &obj] {
        realm.add(obj);
    });
    realm.write([&realm, &obj] {
        obj.binary_col.push_back(4);
    });
    CHECK_EQUALS(obj.binary_col[0], 1);
    CHECK_EQUALS(obj.binary_col[1], 2);
    CHECK_EQUALS(obj.binary_col[2], 3);
    CHECK_EQUALS(obj.binary_col[3], 4);
    co_return;
}

TEST(date) {
    auto realm = realm::open<AllTypesObject, AllTypesObjectLink>({.path=path});
    auto obj = AllTypesObject();
    CHECK_EQUALS(obj.date_col, std::chrono::time_point<std::chrono::system_clock>{});
    auto now = std::chrono::system_clock::now();
    obj.date_col = now;
    CHECK_EQUALS(obj.date_col, now);
    realm.write([&realm, &obj] {
        realm.add(obj);
    });
    CHECK_EQUALS(obj.date_col, now);
    realm.write([&realm, &obj] {
        obj.date_col += std::chrono::seconds(42);
    });
    CHECK_EQUALS(obj.date_col, now + std::chrono::seconds(42));
    co_return;
}

//@end
