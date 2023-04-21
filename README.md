![Realm](./logo.png)

Realm is a mobile database that runs directly inside phones, tablets or wearables.
This repository holds the source code for the C++ SDK Prototype.

## Why Use Realm

* **Intuitive to Developers:** Realm’s object-oriented data model is simple to learn, doesn’t need an ORM, and lets you write less code.
* **Designed for Offline Use:** Realm’s local database persists data on-disk, so apps work as well offline as they do online.
* **Built for Mobile:** Realm is fully-featured, lightweight, and efficiently uses memory, disk space, and battery life.
* **[Device Sync](https://www.mongodb.com/atlas/app-services/device-sync)**: Makes it simple to keep data in sync across users, devices, and your backend in real-time. [Get started for free](https://www.mongodb.com/docs/realm/sdk/cpp/sync/) and [create the cloud backend](http://mongodb.com/realm/register?utm_medium=github_atlas_CTA&utm_source=realm_cpp_github).

## Object-Oriented: Streamline Your Code

Realm was built for mobile developers, with simplicity in mind. The idiomatic, object-oriented data model can save you thousands of lines of code.

```cpp
#include <cpprealm/sdk.hpp>

// Define your models like regular structs.
struct Dog: realm::object {
    realm::persisted<std::string> name;
    realm::persisted<int> age;

    static constexpr auto schema = realm::schema("Dog",
                                                 realm::property<&Dog::name>("name"),
                                                 realm::property<&Dog::age>("age"));
};

struct Person: realm::object {
    realm::persisted<std::string> _id;
    realm::persisted<std::string> name;
    realm::persisted<int> age;
    // Create relationships by pointing an Object field to another Class
    realm::persisted<std::optional<Dog>> dog;

    static constexpr auto schema = realm::schema("Person",
                                                 realm::property<&Person::_id, true>("_id"), // primary key
                                                 realm::property<&Person::name>("name"),
                                                 realm::property<&Person::age>("age"),
                                                 realm::property<&Person::dog>("dog"));
};
// Use them like regular objects.
auto dog = Dog { .name = "Rex", .age = 1 };
std::cout<<"name of dog: "<<dog.name<<std::endl;

// Get the default Realm with compile time schema checking.
auto realm = realm::open<Person, Dog>();
// Persist your data easily with a write transaction 
realm.write([&realm, &dog] {
    realm.add(dog);
});

// Query with type safety.
auto dogs = realm.objects<Dog>();
auto adult_dogs = dogs.where([](auto& d) {
    return d.age > 2;
});
```
## Live Objects: Build Reactive Apps
Realm’s live objects mean data updated anywhere is automatically updated everywhere.
```cpp
// Open the default realm.
auto realm = realm::open<Person, Dog>();

realm::notification_token token;

auto dog = Dog { .name = "Max" };

// Create a dog in the realm.
realm.write([&realm, &dog] {
    realm.add(dog);
});

//  Set up the listener & observe object notifications.
token = dog.observe<Dog>([](auto&& change) {
    if (change.error) {
        std::cout<<"An error occurred: "<<error<<std::endl;
    } else if (change.is_deleted) {
        std::cout<<"The object was deleted."<<std::endl;
    } else {
        std::cout<<"Property "<<property.name<<" changed to "<<property.new_value<<std::endl;
    }
}

// Update the dog's name to see the effect.
realm.write([&dog] {
    dog.name = "Wolfie"
});
```

## Data Sync
The [Atlas Device Sync](https://www.mongodb.com/atlas/app-services/device-sync) makes it simple to keep data in sync across users, devices, and your backend in real-time.
```cpp
auto app = realm::App("<app-id>");
auto user = app.login(realm::App::credentials::anonymous()).get_future().get();
auto synced_realm_ref = user.realm<Car>("foo").get_future().get();
auto realm = synced_realm_ref.resolve();

auto cars = realm.results<Car>();
realm.write([&cars](){
    for (auto& car : cars) {
        car.accelerate();
    }
});
```

## Getting Started

See the detailed instructions in our [docs](https://www.mongodb.com/docs/realm/sdk/cpp/).

The API reference is located [here](https://www.mongodb.com/docs/realm-sdks/cpp/latest/).

## Building Realm

In case you don't want to use the precompiled version, you can build Realm yourself from source.

### MacOS

Prerequisites:

* Building Realm requires Xcode 11.x or newer.
```sh
git submodule update --init --recursive
open Package.swift
```

## Installing Realm

### MacOS / Linux

Prerequisites:

* git, cmake, cxx17

```sh
git submodule update --init --recursive
mkdir build.debug
cd build.debug
cmake -D CMAKE_BUILD_TYPE=debug ..
sudo cmake --build . --target install  
```

You can then link to your library with `-lcpprealm`.
 
<img style="width: 0px; height: 0px;" src="https://3eaz4mshcd.execute-api.us-east-1.amazonaws.com/prod?s=https://github.com/realm/realm-cocoa#README.md">
