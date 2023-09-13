![Realm](./logo.png)

Realm is a mobile database that runs directly inside phones, tablets or wearables.
This repository holds the source code for the C++ SDK Preview.

## Why Use Realm

* **Intuitive to Developers:** Realm’s object-oriented data model is simple to learn, doesn’t need an ORM, and lets you write less code.
* **Designed for Offline Use:** Realm’s local database persists data on-disk, so apps work as well offline as they do online.
* **Built for Mobile:** Realm is fully-featured, lightweight, and efficiently uses memory, disk space, and battery life.
* **[Device Sync](https://www.mongodb.com/atlas/app-services/device-sync)**: Makes it simple to keep data in sync across users, devices, and your backend in real-time. [Get started for free](https://www.mongodb.com/docs/realm/sdk/cpp/sync/) and [create the cloud backend](http://mongodb.com/realm/register?utm_medium=github_atlas_CTA&utm_source=realm_cpp_github).

## Object-Oriented: Streamline Your Code

Realm was built for mobile developers, with simplicity in mind. The idiomatic, object-oriented data model can save you thousands of lines of code.

```cpp
#include <cpprealm/sdk.hpp>

using namespace realm::experimental;

// Define your models like regular structs.
struct Dog {
    std::string name;
    int64_t age;
};

REALM_SCHEMA(Dog, name, age)

struct Person {
    primary_key<std::string> _id;
    std::string name;
    int64_t age;
    // Create relationships by pointing an Object field to another Class
    Dog* dog;
};

REALM_SCHEMA(Person, _id, name, age, dog)

// Use them like regular objects.
auto dog = Dog { .name = "Rex", .age = 1 };
std::cout << "name of dog: " << dog.name << std::endl;
auto person = Person();
person._id = "something unique";
person.name = "John";
person.dog = &dog;

// Get the default Realm with automatic schema discovery.
realm::db_config config;
auto realm = db(std::move(config));
// Persist your data easily with a write transaction 
auto managed_person = realm.write([&realm, &person] {
    return realm.add(std::move(person));
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
realm::db_config config;
auto realm = db(std::move(config));

realm::notification_token token;

auto dog = Dog { .name = "Max" };

// Create a dog in the realm.
auto managed_dog = realm.write([&realm, &dog] {
    return realm.add(std::move(dog));
});

//  Set up the listener & observe object notifications.
token = managed_dog.observe([](auto&& change) {
    if (change.error) {
        std::cout<<"An error occurred: "<<error<<std::endl;
    } else if (change.is_deleted) {
        std::cout<<"The object was deleted."<<std::endl;
    } else {
        std::cout << "Property " << property.name << " changed to " << property.new_value << std::endl;
    }
}

// Update the dog's name to see the effect.
realm.write([&managed_dog] {
    managed_dog.name = "Wolfie"
});
```

## Data Sync
The [Atlas Device Sync](https://www.mongodb.com/atlas/app-services/device-sync) makes it simple to keep data in sync across users, devices, and your backend in real-time.
```cpp
auto app = realm::App("<app-id>");
auto user = app.login(realm::App::credentials::anonymous()).get();
auto realm = db(user.flexible_sync_configuration());

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

## Installing Realm

### MacOS / Linux / Windows

-DREALM_USE_SYSTEM_OPENSSL=ON & `stdc++fs` is required for GCC versions lower than GCC 10


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

<b>Note:</b> If your target is Windows make sure to add the `MSVC_RUNTIME_LIBRARY` property to your target like so:
```
set_property(TARGET My_Target PROPERTY
      MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
```
 
<img style="width: 0px; height: 0px;" src="https://3eaz4mshcd.execute-api.us-east-1.amazonaws.com/prod?s=https://github.com/realm/realm-cocoa#README.md">
