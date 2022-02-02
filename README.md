![Realm](https://github.com/realm/realm-cocoa/raw/master/logo.png)

Realm is a mobile database that runs directly inside phones, tablets or wearables.
This repository holds the source code for the C++ SDK Prototype.

## Why Use Realm

* **Intuitive to Developers:** Realm’s object-oriented data model is simple to learn, doesn’t need an ORM, and lets you write less code.
* **Designed for Offline Use:** Realm’s local database persists data on-disk, so apps work as well offline as they do online.
* **Built for Mobile:** Realm is fully-featured, lightweight, and efficiently uses memory, disk space, and battery life.

## Object-Oriented: Streamline Your Code

Realm was built for mobile developers, with simplicity in mind. The idiomatic, object-oriented data model can save you thousands of lines of code.

```cpp
// Define your models like regular structs.
class Dog: realm::object {
public:
    realm::persisted<std::string> name;
    realm::persisted<int> age;
    
    using schema = realm::schema<"Dog",
                                 realm::property<"name", &Dog::name>,
                                 realm::property<"age", &Dog::age>>;
};

class Person: realm::object {
public:
    realm::peristed<std::string> var _id;
    realm::persisted<std::string> name;
    realm::persisted<int> age;
    // Create relationships by pointing an Object field to another Class
    realm::persisted<std::optional<Dog>> dog;
    
    using schema = realm::schema<"Person",
                                 realm::property<"_id", &Person::_id, true>, // primary key
                                 realm::property<"name", &Person::name>,
                                 realm::property<"age", &Person::age>,
                                 realm::property<"dog", &Person::dog>>;
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
The [MongoDB Realm Sync](https://www.mongodb.com/realm/mobile/sync) service makes it simple to keep data in sync across users, devices, and your backend in real-time.
```cpp
auto app = realm::App("<app-id>");
auto user = co_await app.login(realm::App::Credentials::anonymous());
auto synced_realm_ref = co_await user.realm<Car>("foo");
auto realm = synced_realm_ref.resolve();

auto car = realm.object_new<Car>(0);
realm.write([&car](){
    car.accelerate();
});
```

## Building Realm

In case you don't want to use the precompiled version, you can build Realm yourself from source.

### MacOS

Prerequisites:

* Building Realm requires Xcode 11.x or newer.
* Simply double-click the Package.swift file to build and develop.

## Installing Realm

### MacOS / Linux

Prerequisites:

* git, cmake, cxx20

```sh
git submodule update --init --recursive
mkdir build.debug
cd build.debug
cmake -D CMAKE_BUILD_TYPE=debug ..
sudo cmake --build . --target install  
```

You can then link to your library with `-lcpprealm`.
 
<img style="width: 0px; height: 0px;" src="https://3eaz4mshcd.execute-api.us-east-1.amazonaws.com/prod?s=https://github.com/realm/realm-cocoa#README.md">
