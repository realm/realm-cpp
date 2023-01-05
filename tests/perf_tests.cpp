#include "main.hpp"
#include "test_objects.hpp"
#include <cpprealm/experimental/db.hpp>
#include <cpprealm/experimental/observation.hpp>
#include <cpprealm/experimental/managed_string.hpp>

using namespace realm;

namespace realm::experimental {
    struct Dog {
        std::string name;
        int age = 0;
    };
    REALM_SCHEMA(Dog, name, age)

    struct Person {
        std::string name;
        int age = 0;
        link<Dog> dog;
    };
    REALM_SCHEMA(Person, name, age, dog)

    struct AllTypesObjectEmbedded {
        std::string str_col;
    };
    REALM_SCHEMA(AllTypesObjectEmbedded, str_col)
    struct AllTypesObjectLink {
        int64_t _id = 0;
        std::string str_col;
    };
    REALM_SCHEMA(AllTypesObjectLink, _id, str_col)

    struct AllTypes {
        enum class Enum {
            one, two
        };

        int _id{};
        bool bool_col{};
        std::string str_col;
        Enum enum_col = Enum::one;
        std::chrono::time_point<std::chrono::system_clock> date_col;
        realm::uuid uuid_col;
        std::vector<std::uint8_t> binary_col;
        std::variant<
            int64_t,
            bool,
            std::string,
            double,
            std::chrono::time_point<std::chrono::system_clock>,
            realm::uuid,
            std::vector<uint8_t>
        > mixed_col;

        std::optional<int64_t> opt_int_col;
        std::optional<std::string> opt_str_col;
        std::optional<bool> opt_bool_col;
        std::optional<Enum> opt_enum_col;
        std::optional<std::chrono::time_point<std::chrono::system_clock>> opt_date_col;
        std::optional<realm::uuid> opt_uuid_col;
        std::optional<std::vector<uint8_t>> opt_binary_col;
        link<AllTypesObjectLink> opt_obj_col;

        std::vector<int64_t> list_int_col;
        std::vector<bool> list_bool_col;
        std::vector<std::string> list_str_col;
        std::vector<realm::uuid> list_uuid_col;
        std::vector<std::vector<std::uint8_t>> list_binary_col;
        std::vector<std::chrono::time_point<std::chrono::system_clock>> list_date_col;
        std::vector<realm::mixed> list_mixed_col;
        std::vector<link<AllTypesObjectLink>> list_obj_col;
        std::vector<link<AllTypesObjectEmbedded>> list_embedded_obj_col;

        std::map<std::string, int64_t> map_int_col;
        std::map<std::string, bool> map_bool_col;
        std::map<std::string, std::string> map_str_col;
        std::map<std::string, realm::uuid> map_uuid_col;
        std::map<std::string, std::vector<std::uint8_t>> map_binary_col;
        std::map<std::string, std::chrono::time_point<std::chrono::system_clock>> map_date_col;
        std::map<std::string, Enum> map_enum_col;
        std::map<std::string, realm::mixed> map_mixed_col;
        std::map<std::string, link<AllTypesObjectLink>> map_link_col;
        std::map<std::string, link<AllTypesObjectEmbedded>> map_embedded_col;
    };

    REALM_SCHEMA(AllTypes,
                 _id, bool_col, str_col, enum_col, date_col, uuid_col, binary_col, mixed_col,
                 opt_int_col, opt_str_col, opt_bool_col, opt_enum_col,
                 opt_date_col, opt_uuid_col, opt_binary_col, opt_obj_col,
                 list_int_col, list_bool_col, list_str_col, list_uuid_col, list_binary_col,
                 list_date_col, list_mixed_col, list_obj_col, list_embedded_obj_col,
                 map_int_col, map_bool_col, map_str_col, map_uuid_col, map_binary_col,
                 map_date_col, map_enum_col, map_mixed_col, map_link_col, map_embedded_col)
}

struct task
{
    struct promise_type
    {
        task get_return_object() {
            return handle_type::from_promise(*this);
        }
        std::experimental::suspend_always initial_suspend() { return {}; }
        std::experimental::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
    using handle_type = std::experimental::coroutine_handle<promise_type>;
    handle_type h_;

    task(handle_type h)
            : h_(h)
    {
    }
    ~task() {
        h_.destroy();
    }
};

TEST_CASE("experimental", "[experimental]") {
    SECTION("write return multiple") {
        auto lambda = []() -> ::task {
            realm_path path;
            experimental::db db = experimental::open<experimental::Person, experimental::Dog>(path);
            auto managed_person = db.write([&db]() {
                experimental::Person person;
                person.name = "Jack";
                person.age = 27;
                person.dog->name = "fido";
                return db.add(std::move(person));
            });
            CHECK(managed_person.name.starts_with("Ja"));
            CHECK(managed_person.name.ends_with("ck"));
            auto blah = std::string();
            auto generator = co_await managed_person.observe();
            db.write([&managed_person] {
                managed_person.name = "Jim";
            });
            auto change = generator();
            CHECK(!change.is_deleted);
            CHECK(!change.error);
            CHECK(std::get<std::string>(*change.property.old_value) == "Jack");
            CHECK(std::get<std::string>(*change.property.new_value) == "Jim");
            CHECK(change.property.name == "name");

            db.write([&managed_person] {
                managed_person.name = "Meghna";
            });
            change = generator();
            CHECK(!change.is_deleted);
            CHECK(!change.error);
            CHECK(std::get<std::string>(*change.property.old_value) == "Jim");
            CHECK(std::get<std::string>(*change.property.new_value) == "Meghna");
            CHECK(change.property.name == "name");
            co_return;
        };
        auto task = lambda();
        task.h_.resume();
        while (!task.h_.done()) std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    using namespace realm::experimental;
    {
        realm_path path;
        experimental::db db = experimental::open<experimental::Person, experimental::Dog>(path);
        db.write([&db]() {
            experimental::Person person;
            person.name = "Jack";
            person.age = 27;
            person.dog->name = "fido";
            managed<experimental::Person> managed_person = db.add(std::move(person));
            CHECK(!managed_person.name.empty());
            CHECK(managed_person.name.starts_with("Ja"));
            CHECK(managed_person.name.ends_with("ck"));
            CHECK(managed_person.name >= "Jack");
            CHECK(managed_person.name == "Jack");
            managed_person.name.pop_back();
            CHECK(managed_person.name.ends_with("c"));
            CHECK(managed_person.name.append("k") == "Jack");
            CHECK(managed_person.age.value() == 27);
            CHECK(managed_person.name.value() == "Jack");
            CHECK(managed_person.dog->name == "fido");
            return;
        });
    }
    {
        realm_path path;
        experimental::db db2 = experimental::open<experimental::AllTypes,
            realm::experimental::AllTypesObjectLink,
            realm::experimental::AllTypesObjectEmbedded>(path);
        db2.write([&db2] {
            experimental::AllTypes person;
            auto managed_person = db2.add(std::move(person));
            CHECK(managed_person._id.value() == 0);
            CHECK(managed_person.str_col.value().empty());
        });
    }
    {
          realm_path path;
          experimental::dump_db db = experimental::dump_db(path);
          std::vector<experimental::AllTypes> v;
          for (size_t i = 0; i < 1; i++) {
              v.emplace_back(experimental::AllTypes());
          }
          db.insert(v);
    }
}

TEST_CASE("bulk_insert", "[performance]") {
//    BENCHMARK("old sdk insert 10K") {
//        realm_path path;
//
//        auto realm = open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(path);
//        std::vector<AllTypesObject> v;
//        for (size_t i = 0; i < 10000; i++) {
//            v.emplace_back(AllTypesObject());
//        }
//        realm.write([&v, &realm] {
//            for (auto& o : v) {
//                realm.add(o);
//            }
//        });
//        return path;
//    };
//    BENCHMARK("old sdk vector insert 10K") {
//         realm_path path;
//
//         auto realm = open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(path);
//         std::vector<AllTypesObject> v;
//         for (size_t i = 0; i < 10000; i++) {
//             v.emplace_back(AllTypesObject());
//         }
//         realm.write([&v, &realm] {
//             realm.add(v);
//         });
//         return path;
//    };
    BENCHMARK("experimental sdk insert 10K") {
        realm_path path;
        experimental::db db2 = experimental::open<
                experimental::AllTypes,
                experimental::AllTypesObjectLink,
                experimental::AllTypesObjectEmbedded>(path);
        std::vector<experimental::AllTypes> v;
        for (size_t i = 0; i < 10000; i++) {
            v.emplace_back(experimental::AllTypes());
        }
        db2.write([&db2, &v] {
            db2.insert(v);
        });
        return path;
    };
    BENCHMARK("experimental sdk insert no replication 10K") {
         realm_path path;
         experimental::dump_db db = experimental::dump_db(path);
         std::vector<experimental::AllTypes> v;
         for (size_t i = 0; i < 10000; i++) {
             v.emplace_back(experimental::AllTypes());
         }
         db.insert(v);
         return path;
     };
}
