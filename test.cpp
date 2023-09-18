#include <array>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <cpprealm/sdk.hpp>
#include <cpprealm/experimental/sdk.hpp>
using namespace realm::experimental;
struct Dog {
    primary_key<realm::object_id> _id{realm::object_id::generate()};
    std::string name;
    int64_t age;
    void toString() {
        std::cout << "Dog { _id: " << this->_id <<  ",name: " << this->name << ", age: " << this->age << " }" << std::endl;
    }
};
REALM_SCHEMA(Dog, _id, name, age);
class DogRealm {
public:
    DogRealm() {
        auto realm = realm::experimental::open<Dog>(this->mRealmConfigPath);
        mRealm = std::make_unique<realm::experimental::db>(realm);
        realm::set_default_level_threshold(mLogLevel);
    }
    ~DogRealm() {
        refresh();
    }
    void write() {
        // Create a copy of stored Dog object
        auto dog = this->mDog;
        // Create an object in the realm.
        mRealm->write([&] {
            mRealm->add(std::move(dog));
        });
    }
    void observe() {
        // Observe changes to the current object in the collection
        auto dog = mRealm->objects<Dog>().where([&](auto const& dog){
            return (dog._id == this->mDog._id);
        })[0];
        auto token = dog.observe([&](auto&& change) {
            try {
                if (change.error) {
                    rethrow_exception(change.error);
                }
                if (change.is_deleted) {
                    std::cout << "The current Dog object was deleted." << std::endl;
                } else {
                    for (auto& propertyChange : change.property_changes) {
                        if (propertyChange.name == "name") {
                            auto newPropertyValue = std::get<std::string>(*propertyChange.new_value);
                            std::cout << "The dog name changed to " << newPropertyValue << "." << std::endl;
                        } else if (propertyChange.name == "age") {
                            auto newPropertyValue = std::get<std::int64_t>(*propertyChange.new_value);
                            std::cout << "The dog celebrated its birthday and is now " << newPropertyValue << " years old." << std::endl;
                        } else {
                            std::cerr << "Unrecognized object's property." << std::endl;
                        }
                    }
                }
            } catch (std::exception const& e) {
                std::cerr << "Error: " << e.what() << "." << std::endl;
            }
        });
    }
    void refresh() {
        // Refresh the realm after the change to trigger the notification.
        auto realm = realm::experimental::open<Dog>(this->mRealmConfigPath);
        realm.refresh();
    }
    void remove() {
        // Deleting the object triggers a delete notification.
        auto old_dogs = mRealm->objects<Dog>().where([](auto const& dog){
            return dog.age > 7;
        });
        auto nb_of_dogs = old_dogs.size();
        for (auto i = 0; i < nb_of_dogs; i++) {
            auto dog = old_dogs[nb_of_dogs-i-1];
            mRealm->write([&]{
                mRealm->remove(dog);
            });
        };
        refresh();
    }
    void update(const std::string& key) {
        // Update the recently created object in the collection
        auto dog = mRealm->objects<Dog>().where([&](auto const& dog){
            return (dog._id == this->mDog._id);
        })[0];
        if (key == "age") {
            // Update the dog's age to see the effect.
            mIncrementingAge++;
            mRealm->write([&] {
                dog.age = mIncrementingAge;
            });
        } else if (key == "name") {
            // Update the dog's name to see the effect.
            const realm::persisted<std::string> nameArray[4] = { "Scooby Doo", "Idefix", "Snoopy", "Beethoven"};
            auto idx = rand() % 4;
            auto name = nameArray[idx];
            mRealm->write([&] {
                dog.name = *name;
            });
        } else {
            std::cout << "Cannot update Realm collection, unrecognized key (" << key << ")" << std::endl;
        }
    }
    void unobserve() {
        mToken.unregister();
    }
    Dog getDog() const { return mDog; }
private:
    const int64_t mInitialAge{1};
    const std::string mRealmConfigPath { "dog.realm" };
    int64_t mIncrementingAge{mInitialAge};
    Dog mDog { .name = "Cerberus", .age = mInitialAge };
    std::unique_ptr<realm::experimental::db> mRealm;
    realm::notification_token mToken;
    realm::logger::level mLogLevel { realm::logger::level::all };
};
int main() {
    DogRealm dogRealm;
    dogRealm.getDog().toString();
    
    dogRealm.write();
    dogRealm.observe();
    
    std::random_device seed;
    std::mt19937 gen{seed()};
    std::uniform_int_distribution<> dist{1, 20};
    auto limit = dist(gen);
    auto cnt = 2;
    while (cnt < limit) {
        dogRealm.update("age");
        dogRealm.update("name");
        cnt++;
    }
    dogRealm.remove();
    dogRealm.refresh();
    dogRealm.unobserve();
    return 0;
}