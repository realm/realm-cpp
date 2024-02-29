#include "../main.hpp"
#include "test_objects.hpp"

namespace realm {

#define query_results_size(Cls, fn)  \
    realm.template objects<Cls>().where(fn).size()

#define query_string_results_size(Cls, str, args)  \
    realm.template objects<Cls>().where(str, args).size()

//
//    // Type trait to convert int to int64_t
//    template<typename T>
//    struct TypeConverter {
//        using Type = T;
//    };
//
////    template<>
////    struct TypeConverter<int> {
////        using Type = int64_t;
////    };
//
//    template<typename T>
//    struct TypeConverter<T*> {
//        using Type = managed<T>;
//    };
//
//    template<typename... Ts>
//    struct TypeConverter<std::variant<Ts...>> {
//        using Type = std::variant<typename std::conditional<std::is_pointer_v<Ts>, managed<Ts>, Ts>::type...>;
//    };
//
//#define EXTRACT(...) __VA_ARGS__
//
//// Macro to declare member with type conversion
//#define DECLARE_MEMBER(type, name) typename TypeConverter<type>::Type name
//
//#define DECLARE_CLS(name) \
//    struct MyClass { \
//        DECLARE_MEMBER(EXTRACT(int), value1); \
//        DECLARE_MEMBER(EXTRACT(double), value2); \
//        DECLARE_MEMBER(EXTRACT(float), value3); \
//        DECLARE_MEMBER(EXTRACT(std::variant<int, std::string, AllTypesObject*>), value4); \
//        MyClass() = default; \
//     void printValues() { \
//            std::cout << "Value 1: " << value1 << std::endl; \
//            std::cout << "Value 2: " << value2 << std::endl; \
//            std::cout << "Value 3: " << value3 << std::endl; \
//            std::visit([](auto&& arg) { \
//                std::cout << "Value 4: " << arg << std::endl; \
//            }, value4); \
//        } \
//    };              \
//
//    DECLARE_CLS(cls_name)

    TEST_CASE("query") {
        realm_path path;
        realm::db_config config;
        config.set_path(path);
        SECTION("tsq_basic_comparison", "[query]") {
            auto realm = db(std::move(config));
            auto date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            auto object_id = realm::object_id::generate();

            auto create_obj = [&]() {
                AllTypesObject obj;
                AllTypesObjectLink link;
                link._id = 1;
                link.str_col = "foo";

                obj._id = 123;
                obj.opt_obj_col = &link;
                obj.str_col = "foo bar";
                obj.enum_col = AllTypesObject::Enum::two;
                obj.date_col = std::chrono::system_clock::from_time_t(date);
                obj.uuid_col = realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002");
                obj.object_id_col = object_id;

                obj.binary_col.push_back(0);
                obj.binary_col.push_back(1);
                obj.binary_col.push_back(2);
                obj.binary_col.push_back(3);

                auto managed_obj = realm.write([&]() {
                    return realm.add(std::move(obj));
                });

                return managed_obj;
            };

            auto obj = create_obj();
            auto obj2 = create_obj();

            // With literal as RHS.
            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id == 123; }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "_id == $0", {(int64_t)123}) == 1);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id != 123; }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "_id != $0", {(int64_t)123}) == 0);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.str_col == "foo bar"; }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "str_col == $0", {std::string("foo bar")}) == 1);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.str_col != "foo bar"; }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "str_col != $0", {std::string("foo bar")}) == 0);

            CHECK(query_results_size(AllTypesObject, [](auto &o) {
                      return o.binary_col == std::vector<uint8_t>({0, 1, 2, 3});
                  }) == 1);

            CHECK(query_string_results_size(AllTypesObject, "binary_col == $0", {std::vector<uint8_t>({0, 1, 2, 3})}) == 1);

            CHECK(query_results_size(AllTypesObject,
                                     [](auto &o) { return o.binary_col != std::vector<uint8_t>({0, 1, 2, 3}); }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "binary_col != $0", {std::vector<uint8_t>({0, 1, 2, 3})}) == 0);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col == AllTypesObject::Enum::two; }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "enum_col == $0", {static_cast<int64_t>(AllTypesObject::Enum::two)}) == 1);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col != AllTypesObject::Enum::two; }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "enum_col != $0", {static_cast<int64_t>(AllTypesObject::Enum::two)}) == 0);

            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col == std::chrono::system_clock::from_time_t(date);
                  }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "date_col == $0", {std::chrono::system_clock::from_time_t(date)}) == 1);

            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col != std::chrono::system_clock::from_time_t(date);
                  }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "date_col != $0", {std::chrono::system_clock::from_time_t(date)}) == 0);

            CHECK(query_results_size(AllTypesObject, [](auto &o) {
                      return o.uuid_col == realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002");
                  }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "uuid_col == $0", {realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002")}) == 1);

            CHECK(query_results_size(AllTypesObject, [](auto &o) {
                      return o.uuid_col != realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002");
                  }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "uuid_col != $0", {realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002")}) == 0);

            auto objs = realm.objects<AllTypesObject>().where([](auto& o) { return o.opt_obj_col->str_col == "foo"; }).size();
            CHECK(objs == 1);
            objs = realm.objects<AllTypesObject>().where("opt_obj_col.str_col == $0", {std::string("foo")}).size();
            CHECK(objs == 1);
        }

        SECTION("tsq_greater_less_than", "[query]") {
            auto realm = db(std::move(config));
            auto date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

            auto create_obj = [&]() {
                AllTypesObject obj;
                obj._id = 123;
                obj.enum_col = AllTypesObject::Enum::two;
                obj.date_col = std::chrono::system_clock::from_time_t(date);

                return realm.write([&]() {
                    return realm.add(std::move(obj));
                });
            };

            auto obj = create_obj();
            auto obj2 = create_obj();

            // With literal as RHS.
            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id > 123; }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "_id > $0", {(int64_t)123}) == 0);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id >= 123; }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "_id >= $0", {(int64_t) 123}) == 1);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id < 123; }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "_id < $0", {(int64_t) 123}) == 0);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o._id <= 123; }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "_id <= $0", {(int64_t) 123}) == 1);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col > AllTypesObject::Enum::two; }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "enum_col > $0", {static_cast<int64_t>(AllTypesObject::Enum::two)}) == 0);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col >= AllTypesObject::Enum::two; }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "enum_col >= $0", {static_cast<int64_t>(AllTypesObject::Enum::two)}) == 1);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col < AllTypesObject::Enum::two; }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "enum_col < $0", {static_cast<int64_t>(AllTypesObject::Enum::two)}) == 0);

            CHECK(query_results_size(AllTypesObject, [](auto &o) { return o.enum_col <= AllTypesObject::Enum::two; }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "enum_col <= $0", {static_cast<int64_t>(AllTypesObject::Enum::two)}) == 1);

            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col > std::chrono::system_clock::from_time_t(date);
                  }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "date_col > $0", {std::chrono::system_clock::from_time_t(date)}) == 0);

            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col >= std::chrono::system_clock::from_time_t(date);
                  }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "date_col >= $0", {std::chrono::system_clock::from_time_t(date)}) == 1);

            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col < std::chrono::system_clock::from_time_t(date);
                  }) == 0);
            CHECK(query_string_results_size(AllTypesObject, "date_col < $0", {std::chrono::system_clock::from_time_t(date)}) == 0);

            CHECK(query_results_size(AllTypesObject, [&date](auto &o) {
                      return o.date_col <= std::chrono::system_clock::from_time_t(date);
                  }) == 1);
            CHECK(query_string_results_size(AllTypesObject, "date_col <= $0", {std::chrono::system_clock::from_time_t(date)}) == 1);
        }

        SECTION("tsq_compound", "[query]") {
            auto realm = db(std::move(config));

            AllTypesObject obj;
            obj._id = 123;
            obj.str_col = "John";
            obj.bool_col = true;

            realm.write([&]() {
                realm.add(std::move(obj));
            });

            auto results = realm.objects<AllTypesObject>().where([](auto &o) {
                return o._id == 123 && o.str_col != "John";
            });
            CHECK(results.size() == 0);
            CHECK(query_string_results_size(AllTypesObject, "_id == $0 && str_col != $1", std::vector<realm::mixed>({(int64_t)123, std::string("John")})) == 0);

            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return o._id == 123 || o.str_col != "John";
            });
            CHECK(results.size() == 1);
            CHECK(query_string_results_size(AllTypesObject, "_id == $0 || str_col != $1", std::vector<realm::mixed>({(int64_t)123, std::string("John")})) == 1);

            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return o._id == 123 && o.str_col == "John";
            });
            CHECK(results.size() == 1);
            CHECK(query_string_results_size(AllTypesObject, "_id == $0 && str_col == $1", std::vector<realm::mixed>({(int64_t)123, std::string("John")})) == 1);

            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return o._id == 123 && o.str_col == "John" || o.bool_col == true;
            });
            CHECK(results.size() == 1);
            CHECK(query_string_results_size(AllTypesObject, "_id == $0 && str_col == $1  || bool_col == $2", std::vector<realm::mixed>({(int64_t)123, std::string("John"), true})) == 1);

            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return o._id == 123 && o.str_col.contains("Jo") || o.bool_col == true;
            });
            CHECK(results.size() == 1);
            CHECK(query_string_results_size(AllTypesObject, "_id == $0 && str_col CONTAINS $1  || bool_col == $2", std::vector<realm::mixed>({(int64_t)123, std::string("John"), true})) == 1);

            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return o.str_col.empty();
            });
            CHECK(results.size() == 0);
            CHECK(query_string_results_size(AllTypesObject, "str_col == ''", {}) == 0);

            results = realm.objects<AllTypesObject>().where([](auto &o) {
                return !o.str_col.empty();
            });
            CHECK(results.size() == 1);
            CHECK(query_string_results_size(AllTypesObject, "NOT str_col == ''", {}) == 1);
        }

        SECTION("optionals") {
            auto realm = db(std::move(config));

            auto obj = AllTypesObject();
            auto managed_obj = realm.write([&]() {
                return realm.add(std::move(obj));
            });
            auto res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_str_col == std::nullopt;
            });
            CHECK(res.size() == 1);
            CHECK(query_string_results_size(AllTypesObject, "opt_str_col == NULL", {}) == 1);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_str_col != std::nullopt;
            });
            CHECK(res.size() == 0);
            CHECK(query_string_results_size(AllTypesObject, "opt_str_col != NULL", {}) == 0);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.mixed_col == std::nullopt;
            });
            CHECK(res.size() == 1);
            CHECK(query_string_results_size(AllTypesObject, "mixed_col == NULL", {}) == 1);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.mixed_col != std::nullopt;
            });
            CHECK(res.size() == 0);
            CHECK(query_string_results_size(AllTypesObject, "mixed_col != NULL", {}) == 0);
        }

        SECTION("link column") {
            auto realm = db(std::move(config));

            auto obj = AllTypesObject();
            auto obj_link = AllTypesObjectLink();
            obj_link.str_col = "foo";
            auto obj_link2 = StringObject();
            obj_link2.str_col = "bar";

            obj.opt_obj_col = &obj_link;
            obj_link.str_link_col = &obj_link2;

            auto managed_obj = realm.write([&]() {
                return realm.add(std::move(obj));
            });

            auto res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_obj_col->str_col == "foo";
            });
            CHECK(res.size() == 1);
            CHECK(query_string_results_size(AllTypesObject, "opt_obj_col.str_col == $0", {std::string("foo")}) == 1);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_obj_col->str_col == "bar";
            });
            CHECK(res.size() == 0);
            CHECK(query_string_results_size(AllTypesObject, "opt_obj_col.str_col == $0", {std::string("bar")}) == 0);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_obj_col->str_col != "bar";
            });
            CHECK(res.size() == 1);
            CHECK(query_string_results_size(AllTypesObject, "str_col != $0", {std::string("bar")}) == 1);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_obj_col->str_link_col->str_col == "bar";
            });
            CHECK(res.size() == 1);
            CHECK(query_string_results_size(AllTypesObject, "opt_obj_col.str_link_col.str_col == $0", {std::string("bar")}) == 1);

            res = realm.objects<AllTypesObject>().where([](auto &obj) {
                return obj.opt_obj_col->str_link_col->str_col != "bar";
            });
            CHECK(res.size() == 0);
            CHECK(query_string_results_size(AllTypesObject, "opt_obj_col.str_link_col.str_col != $0", {std::string("bar")}) == 0);
        }
    }
}