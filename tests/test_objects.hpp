#ifndef REALM_TEST_OBJECTS_HPP
#define REALM_TEST_OBJECTS_HPP

#include <cpprealm/sdk.hpp>

struct Dog: realm::object {
    realm::persisted<std::string> name;
    realm::persisted<int> age;

    static constexpr auto schema = realm::schemagen("Dog",
            realm::property<&Dog::name>("name"),
            realm::property<&Dog::age>("age")
    );
};

struct Person: realm::object {
    realm::persisted<std::string> name;
    realm::persisted<int> age;
    realm::persisted<std::optional<Dog>> dog;

    static constexpr auto schema = realm::schemagen("Person",
            realm::property<&Person::name>("name"),
            realm::property<&Person::age>("age"),
            realm::property<&Person::dog>("dog")
    );
};
//
struct AllTypesObjectEmbedded: realm::embedded_object {
    realm::persisted<std::string> str_col;

    static constexpr auto schema = realm::schemagen("AllTypesObjectEmbedded",
        realm::property<&AllTypesObjectEmbedded::str_col>("str_col"));
};

struct AllTypesObjectLink: realm::object {
    realm::persisted<int> _id;
    realm::persisted<std::string> str_col;

    static constexpr auto schema =
            realm::schemagen("AllTypesObjectLink",
                            realm::property<&AllTypesObjectLink::_id, true>("_id"),
                            realm::property<&AllTypesObjectLink::str_col>("str_col"));
};

struct AllTypesObject: realm::object {
    enum class Enum {
        one, two
    };

    realm::persisted<int> _id;
    realm::persisted<bool> bool_col;
    realm::persisted<std::string> str_col;
    realm::persisted<Enum> enum_col;
    realm::persisted<std::chrono::time_point<std::chrono::system_clock>> date_col;
    realm::persisted<realm::uuid> uuid_col;
    realm::persisted<std::vector<std::uint8_t>> binary_col;
    realm::persisted<std::variant<
        int64_t,
        bool,
        std::string,
        double,
        std::chrono::time_point<std::chrono::system_clock>,
        realm::uuid,
        std::vector<uint8_t>
    >> mixed_col;

    realm::persisted<std::vector<int>> list_int_col;
    realm::persisted<std::vector<bool>> list_bool_col;
    realm::persisted<std::vector<std::string>> list_str_col;
    realm::persisted<std::vector<realm::uuid>> list_uuid_col;
    realm::persisted<std::vector<std::vector<std::uint8_t>>> list_binary_col;
    realm::persisted<std::vector<std::chrono::time_point<std::chrono::system_clock>>> list_date_col;

    realm::persisted<std::vector<AllTypesObjectLink>> list_obj_col;
    realm::persisted<std::vector<AllTypesObjectEmbedded>> list_embedded_obj_col;

    static constexpr auto schema = realm::schemagen("AllTypesObject",
        realm::property<&AllTypesObject::_id, true>("_id"),
        realm::property<&AllTypesObject::bool_col>("bool_col"),
        realm::property<&AllTypesObject::str_col>("str_col"),
        realm::property<&AllTypesObject::enum_col>("enum_col"),
        realm::property<&AllTypesObject::date_col>("date_col"),
        realm::property<&AllTypesObject::uuid_col>("uuid_col"),
        realm::property<&AllTypesObject::binary_col>("binary_col"),
        realm::property<&AllTypesObject::mixed_col>("mixed_col"),
        realm::property<&AllTypesObject::list_int_col>("list_int_col"),
        realm::property<&AllTypesObject::list_bool_col>("list_bool_col"),
        realm::property<&AllTypesObject::list_str_col>("list_str_col"),
        realm::property<&AllTypesObject::list_uuid_col>("list_uuid_col"),
        realm::property<&AllTypesObject::list_binary_col>("list_binary_col"),
        realm::property<&AllTypesObject::list_date_col>("list_date_col"),
        realm::property<&AllTypesObject::list_obj_col>("list_obj_col"),
        realm::property<&AllTypesObject::list_embedded_obj_col>("list_embedded_obj_col"));
};

struct EmbeddedFoo: realm::embedded_object {
    realm::persisted<int> bar;

    static constexpr auto schema = realm::schemagen("EmbeddedFoo", realm::property<&EmbeddedFoo::bar>("bar"));
};

struct Foo: realm::object {
    realm::persisted<int> bar;
    realm::persisted<EmbeddedFoo> foo;

    Foo() = default;
    Foo(const Foo&) = delete;
    static constexpr auto schema = realm::schemagen("Foo",
        realm::property<&Foo::bar>("bar"),
        realm::property<&Foo::foo>("foo"));
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

    operator std::string_view() const { // NOLINT(google-explicit-constructor)
        return m_str;
    }
private:
    std::string m_str;
};
struct CustomStringObject : realm::object {
    realm::persisted<UppercaseString> str_col;
    static constexpr auto schema = realm::schemagen("CustomStringObject",
                                                    realm::property<&CustomStringObject::str_col>("str_col"));
};
struct IntTypesObject: realm::object {
    realm::persisted<int8_t> int8_col;
    realm::persisted<int16_t> int16_col;
    realm::persisted<int32_t> int32_col;
    realm::persisted<int64_t> int64_col;
    realm::persisted<size_t> size_col;

    static constexpr auto schema = realm::schemagen("IntTypesObject",
                                                    realm::property<&Foo::bar>("bar"),
                                                    realm::property<&Foo::foo>("foo"));
};
#endif //REALM_TEST_OBJECTS_HPP
