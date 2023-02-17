#include <cpprealm/sdk.hpp>

struct Dog : realm::object<Dog> {
  realm::persisted<std::string> name;
  realm::persisted<int64_t> age;
  static constexpr auto schema = realm::schema("Dog",
                                               realm::property<&Dog::name>("name"),
                                               realm::property<&Dog::age>("age"));
};
struct Person : realm::object<Person> {
  realm::persisted<std::string> _id;
  realm::persisted<std::string> name;
  realm::persisted<int64_t> age;
  // Create relationships by pointing an Object field to another Class
  realm::persisted<std::optional<Dog>> dog;
  static constexpr auto schema = realm::schema("Person",
                                               realm::property<&Person::_id, true>("_id"), // primary key
                                               realm::property<&Person::name>("name"),
                                               realm::property<&Person::age>("age"),
                                               realm::property<&Person::dog>("dog"));
};

void run_realm() {
  // Use Realm objects like regular objects.
  auto dog = Dog { .name = "Rex", .age = 1 };
  std::cout << "dog: " << dog << "\n";
  // Get the default Realm with compile time schema checking.
  auto realm = realm::open<Person, Dog>();
  // Persist your data in a write transaction
  realm.write([&realm, &dog] {
    realm.add(dog);
  });
}

int main() {
  run_realm();
  return 0;
}

