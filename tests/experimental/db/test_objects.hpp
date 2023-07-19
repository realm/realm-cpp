#ifndef CPPREALM_EXPERIMENTAL_TEST_OBJECTS_HPP
#define CPPREALM_EXPERIMENTAL_TEST_OBJECTS_HPP

#include <cpprealm/experimental/sdk.hpp>

namespace realm::experimental {
    struct Dog;
    struct Person {
        primary_key<int64_t> _id;
        std::string name;
        int64_t age = 0;
        Dog* dog;
    };
    REALM_SCHEMA(Person, _id, name, age, dog)
    struct Dog {
        primary_key<int64_t> _id;
        std::string name;
        std::string name2;
        std::string name3;
        int64_t foo2 = 0;
        std::string name4;

        int64_t age = 0;
        linking_objects<&Person::dog> owners;
    };
    REALM_SCHEMA(Dog,
                 _id,
                 name,
                 name2,
                 name3,
                 foo2,
                 name4,
                 age,
                 owners)

    struct EmbeddedDepth3 {
        std::string str_col;
    };
    REALM_EMBEDDED_SCHEMA(EmbeddedDepth3, str_col)

    struct EmbeddedDepth2 {
        std::string str_col;
        EmbeddedDepth3* embedded_link;
    };
    REALM_EMBEDDED_SCHEMA(EmbeddedDepth2, str_col, embedded_link)

    struct EmbeddedDepth1 {
        std::string str_col;
        EmbeddedDepth2* embedded_link;
    };
    REALM_EMBEDDED_SCHEMA(EmbeddedDepth1, str_col, embedded_link)

    struct EmbeddedDepthObject {
        EmbeddedDepth1* embedded_link;
    };
    REALM_SCHEMA(EmbeddedDepthObject, embedded_link)

    struct AllTypesObjectEmbedded {
        int64_t _id;
        std::string str_col;
    };
    REALM_EMBEDDED_SCHEMA(AllTypesObjectEmbedded, _id, str_col)

    struct StringObject {
        primary_key<int64_t> _id;
        std::string str_col;
    };
    REALM_SCHEMA(StringObject, _id, str_col)

    struct AllTypesObjectLink {
        primary_key<int64_t> _id;
        std::string str_col;
        StringObject* str_link_col = nullptr;
    };
    REALM_SCHEMA(AllTypesObjectLink, _id, str_col, str_link_col)

    struct AllTypesObject {
        enum class Enum {
            one, two
        };

        primary_key<int64_t> _id;
        int64_t int_col;
        double double_col;
        bool bool_col;
        std::string str_col;
        std::string str_col2;

        Enum enum_col = Enum::one;
        std::chrono::time_point<std::chrono::system_clock> date_col;
        realm::uuid uuid_col;
        realm::object_id object_id_col;
        realm::decimal128 decimal_col;
        std::vector<std::uint8_t> binary_col;
        using my_mixed = std::variant<
                std::monostate,
                int64_t,
                bool,
                std::string,
                double,
                std::chrono::time_point<std::chrono::system_clock>,
                realm::uuid,
                realm::object_id,
                realm::decimal128,
                std::vector<uint8_t>
                >;
        my_mixed mixed_col;
        my_mixed my_mixed_col;

        std::optional<int64_t> opt_int_col;
        std::optional<double> opt_double_col;
        std::optional<std::string> opt_str_col;
        std::optional<bool> opt_bool_col;
        std::optional<Enum> opt_enum_col;
        std::optional<std::chrono::time_point<std::chrono::system_clock>> opt_date_col;
        std::optional<realm::uuid> opt_uuid_col;
        std::optional<realm::object_id> opt_object_id_col;
        std::optional<realm::decimal128> opt_decimal_col;
        std::optional<std::vector<uint8_t>> opt_binary_col;
        AllTypesObjectLink* opt_obj_col = nullptr;
        AllTypesObjectEmbedded* opt_embedded_obj_col = nullptr;

        std::vector<int64_t> list_int_col;
        std::vector<double> list_double_col;
        std::vector<bool> list_bool_col;
        std::vector<std::string> list_str_col;
        std::vector<realm::uuid> list_uuid_col;
        std::vector<realm::object_id> list_object_id_col;
        std::vector<realm::decimal128> list_decimal_col;
        std::vector<std::vector<std::uint8_t>> list_binary_col;
        std::vector<std::chrono::time_point<std::chrono::system_clock>> list_date_col;
        std::vector<realm::mixed> list_mixed_col;
        std::vector<AllTypesObjectLink*> list_obj_col;
        std::vector<AllTypesObjectEmbedded*> list_embedded_obj_col;

        std::map<std::string, int64_t> map_int_col;
        std::map<std::string, double> map_double_col;
        std::map<std::string, bool> map_bool_col;
        std::map<std::string, std::string> map_str_col;
        std::map<std::string, realm::uuid> map_uuid_col;
        std::map<std::string, realm::object_id> map_object_id_col;
        std::map<std::string, realm::decimal128> map_decimal_col;
        std::map<std::string, std::vector<std::uint8_t>> map_binary_col;
        std::map<std::string, std::chrono::time_point<std::chrono::system_clock>> map_date_col;
        std::map<std::string, Enum> map_enum_col;
        std::map<std::string, realm::mixed> map_mixed_col;

        std::map<std::string, AllTypesObjectLink*> map_link_col;
        std::map<std::string, AllTypesObjectEmbedded*> map_embedded_col;
    };

    REALM_SCHEMA(AllTypesObject,
                 _id, int_col, double_col, bool_col, str_col, enum_col, date_col, uuid_col, object_id_col, decimal_col, binary_col, mixed_col, my_mixed_col,
                 opt_int_col, opt_double_col, opt_str_col, opt_bool_col, opt_enum_col,
                 opt_date_col, opt_uuid_col, opt_object_id_col, opt_decimal_col, opt_binary_col, opt_obj_col, opt_embedded_obj_col,
                 list_int_col, list_double_col, list_bool_col, list_str_col, list_uuid_col, list_object_id_col, list_decimal_col, list_binary_col,
                 list_date_col, list_mixed_col, list_obj_col, list_embedded_obj_col,
                 map_int_col, map_double_col, map_bool_col, map_str_col, map_uuid_col, map_object_id_col, map_decimal_col, map_binary_col,
                 map_date_col, map_enum_col, map_mixed_col, map_link_col, map_embedded_col)
}


#endif//CPPREALM_EXPERIMENTAL_TEST_OBJECTS_HPP
