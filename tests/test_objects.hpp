#ifndef REALM_TEST_OBJECTS_HPP
#define REALM_TEST_OBJECTS_HPP

#include <cpprealm/sdk.hpp>

struct Dog: realm::object {
    realm::persisted<std::string> name;
    realm::persisted<int> age;

    using schema = realm::schema<"Dog",
            realm::property<"name", &Dog::name>,
            realm::property<"age", &Dog::age>>;
};

struct Person: realm::object {
    realm::persisted<std::string> name;
    realm::persisted<int> age;
    realm::persisted<std::optional<Dog>> dog;

    using schema = realm::schema<"Person",
            realm::property<"name", &Person::name>,
            realm::property<"age", &Person::age>,
            realm::property<"dog", &Person::dog>>;
};

struct AllTypesObjectLink: realm::object {
    realm::persisted<int> _id;
    realm::persisted<std::string> str_col;

    using schema = realm::schema<"AllTypesObjectLink", realm::property<"_id", &AllTypesObjectLink::_id, true>, realm::property<"str_col", &AllTypesObjectLink::str_col>>;
};

struct AllTypesObject: realm::object {
    enum class Enum {
        one, two
    };

    realm::persisted<int> _id;
    realm::persisted<std::string> str_col;
    realm::persisted<Enum> enum_col;
    realm::persisted<std::chrono::time_point<std::chrono::system_clock>> date_col;
    realm::persisted<realm::uuid> uuid_col;
    realm::persisted<std::vector<std::uint8_t>> binary_col;

    realm::persisted<std::vector<int>> list_int_col;
    realm::persisted<std::vector<std::string>> list_str_col;
    realm::persisted<std::vector<realm::uuid>> list_uuid_col;
    realm::persisted<std::vector<std::vector<std::uint8_t>>> list_binary_col;
    realm::persisted<std::vector<std::chrono::time_point<std::chrono::system_clock>>> list_date_col;

    realm::persisted<std::vector<AllTypesObjectLink>> list_obj_col;

    using schema = realm::schema<
    "AllTypesObject",
    realm::property<"_id", &AllTypesObject::_id, true>,
    realm::property<"str_col", &AllTypesObject::str_col>,
    realm::property<"enum_col", &AllTypesObject::enum_col>,
    realm::property<"date_col", &AllTypesObject::date_col>,
    realm::property<"uuid_col", &AllTypesObject::uuid_col>,
    realm::property<"binary_col", &AllTypesObject::binary_col>,
    realm::property<"list_int_col", &AllTypesObject::list_int_col>,
    realm::property<"list_str_col", &AllTypesObject::list_str_col>,
    realm::property<"list_uuid_col", &AllTypesObject::list_uuid_col>,
    realm::property<"list_binary_col", &AllTypesObject::list_binary_col>,
    realm::property<"list_date_col", &AllTypesObject::list_date_col>,
    realm::property<"list_obj_col", &AllTypesObject::list_obj_col>>;
};


struct Foo: realm::object {
    realm::persisted<int> bar;
    Foo() = default;
    Foo(const Foo&) = delete;
    using schema = realm::schema<"Foo", realm::property<"bar", &Foo::bar>>;
};

#endif //REALM_TEST_OBJECTS_HPP
