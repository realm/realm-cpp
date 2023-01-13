#ifndef REALM_TEST_OBJECTS_HPP
#define REALM_TEST_OBJECTS_HPP

#include <cpprealm/sdk.hpp>

struct Dog: realm::object<Dog> {
    realm::persisted<std::string> name;
    realm::persisted<int64_t> age;

    static constexpr auto schema = realm::schema("Dog",
            realm::property<&Dog::name>("name"),
            realm::property<&Dog::age>("age")
    );
};

struct Person: realm::object<Person> {
    realm::persisted<std::string> name;
    realm::persisted<int64_t> age;
    realm::persisted<std::optional<Dog>> dog;

    static constexpr auto schema = realm::schema("Person",
            realm::property<&Person::name>("name"),
            realm::property<&Person::age>("age"),
            realm::property<&Person::dog>("dog")
    );
};
//
struct AllTypesObjectEmbedded: realm::embedded_object<AllTypesObjectEmbedded> {
    realm::persisted<std::string> str_col;

    static constexpr auto schema = realm::schema("AllTypesObjectEmbedded",
        realm::property<&AllTypesObjectEmbedded::str_col>("str_col"));
};

struct AllTypesObjectLink: realm::object<AllTypesObjectLink> {
    realm::persisted<int64_t> _id;
    realm::persisted<std::string> str_col;

    static constexpr auto schema =
            realm::schema("AllTypesObjectLink",
                            realm::property<&AllTypesObjectLink::_id, true>("_id"),
                            realm::property<&AllTypesObjectLink::str_col>("str_col"));
};

struct AllTypesObject: realm::object<AllTypesObject> {
    enum class Enum {
        one, two
    };

    realm::persisted<int64_t> _id;
    realm::persisted<bool> bool_col;
    realm::persisted<std::string> str_col;
    realm::persisted<Enum> enum_col;
    realm::persisted<std::chrono::time_point<std::chrono::system_clock>> date_col;
    realm::persisted<realm::uuid> uuid_col;
    realm::persisted<std::vector<std::uint8_t>> binary_col;
    realm::persisted<realm::mixed> mixed_col;

    realm::persisted<std::optional<int64_t>> opt_int_col;
    realm::persisted<std::optional<std::string>> opt_str_col;
    realm::persisted<std::optional<bool>> opt_bool_col;
    realm::persisted<std::optional<Enum>> opt_enum_col;
    realm::persisted<std::optional<std::chrono::time_point<std::chrono::system_clock>>> opt_date_col;
    realm::persisted<std::optional<realm::uuid>> opt_uuid_col;
    realm::persisted<std::optional<std::vector<uint8_t>>> opt_binary_col;
    realm::persisted<std::optional<AllTypesObjectLink>> opt_obj_col;
    realm::persisted<std::optional<AllTypesObjectEmbedded>> opt_embedded_obj_col;

    realm::persisted<std::vector<int64_t>> list_int_col;
    realm::persisted<std::vector<bool>> list_bool_col;
    realm::persisted<std::vector<std::string>> list_str_col;
    realm::persisted<std::vector<realm::uuid>> list_uuid_col;
    realm::persisted<std::vector<std::vector<std::uint8_t>>> list_binary_col;
    realm::persisted<std::vector<std::chrono::time_point<std::chrono::system_clock>>> list_date_col;
    realm::persisted<std::vector<realm::mixed>> list_mixed_col;
    realm::persisted<std::vector<AllTypesObjectLink>> list_obj_col;
    realm::persisted<std::vector<AllTypesObjectEmbedded>> list_embedded_obj_col;

    realm::persisted<std::map<std::string, int64_t>> map_int_col;
    realm::persisted<std::map<std::string, bool>> map_bool_col;
    realm::persisted<std::map<std::string, std::string>> map_str_col;
    realm::persisted<std::map<std::string, realm::uuid>> map_uuid_col;
    realm::persisted<std::map<std::string, std::vector<std::uint8_t>>> map_binary_col;
    realm::persisted<std::map<std::string, std::chrono::time_point<std::chrono::system_clock>>> map_date_col;
    realm::persisted<std::map<std::string, Enum>> map_enum_col;
    realm::persisted<std::map<std::string, realm::mixed>> map_mixed_col;
    realm::persisted<std::map<std::string, std::optional<AllTypesObjectLink>>> map_link_col;
    realm::persisted<std::map<std::string, std::optional<AllTypesObjectEmbedded>>> map_embedded_col;

    static constexpr auto schema = realm::schema("AllTypesObject",
        realm::property<&AllTypesObject::_id, true>("_id"),
        realm::property<&AllTypesObject::bool_col>("bool_col"),
        realm::property<&AllTypesObject::str_col>("str_col"),
        realm::property<&AllTypesObject::enum_col>("enum_col"),
        realm::property<&AllTypesObject::date_col>("date_col"),
        realm::property<&AllTypesObject::uuid_col>("uuid_col"),
        realm::property<&AllTypesObject::binary_col>("binary_col"),
        realm::property<&AllTypesObject::mixed_col>("mixed_col"),

        realm::property<&AllTypesObject::opt_int_col>("opt_int_col"),
        realm::property<&AllTypesObject::opt_str_col>("opt_str_col"),
        realm::property<&AllTypesObject::opt_bool_col>("opt_bool_col"),
        realm::property<&AllTypesObject::opt_binary_col>("opt_binary_col"),
        realm::property<&AllTypesObject::opt_date_col>("opt_date_col"),
        realm::property<&AllTypesObject::opt_enum_col>("opt_enum_col"),
        realm::property<&AllTypesObject::opt_obj_col>("opt_obj_col"),
        realm::property<&AllTypesObject::opt_embedded_obj_col>("opt_embedded_obj_col"),
        realm::property<&AllTypesObject::opt_uuid_col>("opt_uuid_col"),

        realm::property<&AllTypesObject::list_int_col>("list_int_col"),
        realm::property<&AllTypesObject::list_bool_col>("list_bool_col"),
        realm::property<&AllTypesObject::list_str_col>("list_str_col"),
        realm::property<&AllTypesObject::list_uuid_col>("list_uuid_col"),
        realm::property<&AllTypesObject::list_binary_col>("list_binary_col"),
        realm::property<&AllTypesObject::list_date_col>("list_date_col"),
        realm::property<&AllTypesObject::list_mixed_col>("list_mixed_col"),
        realm::property<&AllTypesObject::list_obj_col>("list_obj_col"),
        realm::property<&AllTypesObject::list_embedded_obj_col>("list_embedded_obj_col"),

        realm::property<&AllTypesObject::map_int_col>("map_int_col"),
        realm::property<&AllTypesObject::map_str_col>("map_str_col"),
        realm::property<&AllTypesObject::map_bool_col>("map_bool_col"),
        realm::property<&AllTypesObject::map_enum_col>("map_enum_col"),
        realm::property<&AllTypesObject::map_date_col>("map_date_col"),
        realm::property<&AllTypesObject::map_uuid_col>("map_uuid_col"),
        realm::property<&AllTypesObject::map_mixed_col>("map_mixed_col"),
        realm::property<&AllTypesObject::map_link_col>("map_link_col"),
        realm::property<&AllTypesObject::map_embedded_col>("map_embedded_col"));
};

/*
struct AllTypesAsymmetricObject: realm::asymmetric_object<AllTypesAsymmetricObject> {
    enum class Enum {
        one, two
    };

    realm::persisted<int64_t> _id;
    realm::persisted<bool> bool_col;
    realm::persisted<std::string> str_col;
    realm::persisted<Enum> enum_col;
    realm::persisted<std::chrono::time_point<std::chrono::system_clock>> date_col;
    realm::persisted<realm::uuid> uuid_col;
    realm::persisted<std::vector<std::uint8_t>> binary_col;
    realm::persisted<realm::mixed> mixed_col;

    realm::persisted<std::optional<int64_t>> opt_int_col;
    realm::persisted<std::optional<std::string>> opt_str_col;
    realm::persisted<std::optional<bool>> opt_bool_col;
    realm::persisted<std::optional<Enum>> opt_enum_col;
    realm::persisted<std::optional<std::chrono::time_point<std::chrono::system_clock>>> opt_date_col;
    realm::persisted<std::optional<realm::uuid>> opt_uuid_col;
    realm::persisted<std::optional<std::vector<uint8_t>>> opt_binary_col;
    realm::persisted<std::optional<AllTypesObjectEmbedded>> opt_embedded_obj_col;

    realm::persisted<std::vector<int64_t>> list_int_col;
    realm::persisted<std::vector<bool>> list_bool_col;
    realm::persisted<std::vector<std::string>> list_str_col;
    realm::persisted<std::vector<realm::uuid>> list_uuid_col;
    realm::persisted<std::vector<std::vector<std::uint8_t>>> list_binary_col;
    realm::persisted<std::vector<std::chrono::time_point<std::chrono::system_clock>>> list_date_col;
    realm::persisted<std::vector<realm::mixed>> list_mixed_col;
    realm::persisted<std::vector<AllTypesObjectEmbedded>> list_embedded_obj_col;

    realm::persisted<std::map<std::string, int64_t>> map_int_col;
    realm::persisted<std::map<std::string, bool>> map_bool_col;
    realm::persisted<std::map<std::string, std::string>> map_str_col;
    realm::persisted<std::map<std::string, realm::uuid>> map_uuid_col;
    realm::persisted<std::map<std::string, std::vector<std::uint8_t>>> map_binary_col;
    realm::persisted<std::map<std::string, std::chrono::time_point<std::chrono::system_clock>>> map_date_col;
    realm::persisted<std::map<std::string, Enum>> map_enum_col;
    realm::persisted<std::map<std::string, realm::mixed>> map_mixed_col;
    realm::persisted<std::map<std::string, std::optional<AllTypesObjectEmbedded>>> map_embedded_col;

    static constexpr auto schema = realm::schema("AllTypesAsymmetricObject",
                                                 realm::property<&AllTypesAsymmetricObject::_id, true>("_id"),
                                                 realm::property<&AllTypesAsymmetricObject::bool_col>("bool_col"),
                                                 realm::property<&AllTypesAsymmetricObject::str_col>("str_col"),
                                                 realm::property<&AllTypesAsymmetricObject::enum_col>("enum_col"),
                                                 realm::property<&AllTypesAsymmetricObject::date_col>("date_col"),
                                                 realm::property<&AllTypesAsymmetricObject::uuid_col>("uuid_col"),
                                                 realm::property<&AllTypesAsymmetricObject::binary_col>("binary_col"),
                                                 realm::property<&AllTypesAsymmetricObject::mixed_col>("mixed_col"),

                                                 realm::property<&AllTypesAsymmetricObject::opt_int_col>("opt_int_col"),
                                                 realm::property<&AllTypesAsymmetricObject::opt_str_col>("opt_str_col"),
                                                 realm::property<&AllTypesAsymmetricObject::opt_bool_col>("opt_bool_col"),
                                                 realm::property<&AllTypesAsymmetricObject::opt_binary_col>("opt_binary_col"),
                                                 realm::property<&AllTypesAsymmetricObject::opt_date_col>("opt_date_col"),
                                                 realm::property<&AllTypesAsymmetricObject::opt_enum_col>("opt_enum_col"),
                                                 realm::property<&AllTypesAsymmetricObject::opt_embedded_obj_col>("opt_embedded_obj_col"),
                                                 realm::property<&AllTypesAsymmetricObject::opt_uuid_col>("opt_uuid_col"),

                                                 realm::property<&AllTypesAsymmetricObject::list_int_col>("list_int_col"),
                                                 realm::property<&AllTypesAsymmetricObject::list_bool_col>("list_bool_col"),
                                                 realm::property<&AllTypesAsymmetricObject::list_str_col>("list_str_col"),
                                                 realm::property<&AllTypesAsymmetricObject::list_uuid_col>("list_uuid_col"),
                                                 realm::property<&AllTypesAsymmetricObject::list_binary_col>("list_binary_col"),
                                                 realm::property<&AllTypesAsymmetricObject::list_date_col>("list_date_col"),
                                                 realm::property<&AllTypesAsymmetricObject::list_mixed_col>("list_mixed_col"),
                                                 realm::property<&AllTypesAsymmetricObject::list_embedded_obj_col>("list_embedded_obj_col"),

                                                 realm::property<&AllTypesAsymmetricObject::map_int_col>("map_int_col"),
                                                 realm::property<&AllTypesAsymmetricObject::map_str_col>("map_str_col"),
                                                 realm::property<&AllTypesAsymmetricObject::map_bool_col>("map_bool_col"),
                                                 realm::property<&AllTypesAsymmetricObject::map_enum_col>("map_enum_col"),
                                                 realm::property<&AllTypesAsymmetricObject::map_date_col>("map_date_col"),
                                                 realm::property<&AllTypesAsymmetricObject::map_uuid_col>("map_uuid_col"),
                                                 realm::property<&AllTypesAsymmetricObject::map_mixed_col>("map_mixed_col"),
                                                 realm::property<&AllTypesAsymmetricObject::map_embedded_col>("map_embedded_col"));
};
*/

struct EmbeddedFoo: realm::embedded_object<EmbeddedFoo> {
    realm::persisted<int64_t> bar;

    static constexpr auto schema = realm::schema("EmbeddedFoo", realm::property<&EmbeddedFoo::bar>("bar"));
};

struct Foo: realm::object<Foo> {
    realm::persisted<int64_t> bar;
    realm::persisted<std::optional<EmbeddedFoo>> foo;

    realm::persisted<int64_t> bar2;
    realm::persisted<std::optional<EmbeddedFoo>> foo2;

    Foo() = default;
    Foo(const Foo&) = delete;
    static constexpr auto schema = realm::schema("Foo",
        realm::property<&Foo::bar>("bar"),
        realm::property<&Foo::foo>("foo"),
        realm::property<&Foo::bar2>("bar2"),
        realm::property<&Foo::foo2>("foo2"));
};

struct UppercaseString {
    UppercaseString() = default;
    UppercaseString(const char* str) // NOLINT(google*)
    : m_str(str)
    {
        std::transform(m_str.begin(), m_str.end(), m_str.begin(), ::toupper);
    }
    UppercaseString(const std::string& str) // NOLINT(google*)
    : UppercaseString(str.data())
    {
    }

    operator std::string() const { // NOLINT(google-explicit-constructor)
        return m_str;
    }
private:
    std::string m_str;
};

inline bool operator==(const UppercaseString& lhs, const char* rhs) {
    return static_cast<std::string>(lhs) == rhs;
}

//struct CustomStringObject : realm::object<CustomStringObject> {
//    realm::persisted<UppercaseString> str_col;
//    static constexpr auto schema = realm::schema("CustomStringObject",
//                                                    realm::property<&CustomStringObject::str_col>("str_col"));
//};
struct IntTypesObject: realm::object<IntTypesObject> {
    realm::persisted<int64_t> int64_col;

    static constexpr auto schema = realm::schema("IntTypesObject",
                                                    realm::property<&Foo::bar>("bar"),
                                                    realm::property<&Foo::foo>("foo"));
};
#endif //REALM_TEST_OBJECTS_HPP
