#include "main.hpp"
#include <cpprealm/experimental/db.hpp>
#include <cpprealm/experimental/observation.hpp>
#include "admin_utils.hpp"
#include "sync_test_utils.hpp"

using namespace realm;

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

    struct AllTypesObjectEmbedded {
        int64_t _id;
        std::string str_col;
    };
    REALM_EMBEDDED_SCHEMA(AllTypesObjectEmbedded, _id, str_col)
    struct AllTypesObjectLink {
        primary_key<int64_t> _id;
        std::string str_col;
    };
    REALM_SCHEMA(AllTypesObjectLink, _id, str_col)

    struct AllTypes {
        enum class Enum {
            one, two
        };

        primary_key<int64_t> _id;
        double double_col;
        bool bool_col;
        std::string str_col;
        std::string str_col2;

        Enum enum_col = Enum::one;
        std::chrono::time_point<std::chrono::system_clock> date_col;
        realm::uuid uuid_col;
        realm::object_id object_id_col;
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
                std::vector<uint8_t>
                >;
        my_mixed mixed_col;

        std::optional<int64_t> opt_int_col;
        std::optional<double> opt_double_col;
        std::optional<std::string> opt_str_col;
        std::optional<bool> opt_bool_col;
        std::optional<Enum> opt_enum_col;
        std::optional<std::chrono::time_point<std::chrono::system_clock>> opt_date_col;
        std::optional<realm::uuid> opt_uuid_col;
        std::optional<realm::object_id> opt_object_id_col;
        std::optional<std::vector<uint8_t>> opt_binary_col;
        link<AllTypesObjectLink> opt_obj_col;
        link<AllTypesObjectEmbedded> opt_embedded_obj_col;

        std::vector<int64_t> list_int_col;
        std::vector<double> list_double_col;
        std::vector<bool> list_bool_col;
        std::vector<std::string> list_str_col;
        std::vector<realm::uuid> list_uuid_col;
        std::vector<realm::object_id> list_object_id_col;
        std::vector<std::vector<std::uint8_t>> list_binary_col;
        std::vector<std::chrono::time_point<std::chrono::system_clock>> list_date_col;
        std::vector<realm::mixed> list_mixed_col;
        std::vector<link<AllTypesObjectLink>> list_obj_col;
        std::vector<link<AllTypesObjectEmbedded>> list_embedded_obj_col;

//        std::vector<std::optional<int64_t>> list_opt_int_col;
//        std::vector<std::optional<double>> list_opt_double_col;
//        std::vector<std::optional<bool>> list_opt_bool_col;
//        std::vector<std::optional<std::string>> list_opt_str_col;
//        std::vector<std::optional<realm::uuid>> list_opt_uuid_col;
//        std::vector<std::optional<realm::object_id>> list_opt_object_id_col;
//        std::vector<std::optional<std::vector<std::uint8_t>>> list_opt_binary_col;
//        std::vector<std::optional<std::chrono::time_point<std::chrono::system_clock>>> list_opt_date_col;

        std::map<std::string, int64_t> map_int_col;
        std::map<std::string, double> map_double_col;
        std::map<std::string, bool> map_bool_col;
        std::map<std::string, std::string> map_str_col;
        std::map<std::string, realm::uuid> map_uuid_col;
        std::map<std::string, realm::object_id> map_object_id_col;
        std::map<std::string, std::vector<std::uint8_t>> map_binary_col;
        std::map<std::string, std::chrono::time_point<std::chrono::system_clock>> map_date_col;
        std::map<std::string, Enum> map_enum_col;
        std::map<std::string, realm::mixed> map_mixed_col;

//        std::map<std::string, std::optional<int64_t>> map_opt_int_col;
//        std::map<std::string, std::optional<double>> map_opt_double_col;
//        std::map<std::string, std::optional<bool>> map_opt_bool_col;
//        std::map<std::string, std::optional<std::string>> map_opt_str_col;
//        std::map<std::string, std::optional<realm::uuid>> map_opt_uuid_col;
//        std::map<std::string, std::optional<realm::object_id>> map_opt_object_id_col;
//        std::map<std::string, std::optional<std::vector<uint8_t>>> map_opt_binary_col;
//        std::map<std::string, std::optional<std::chrono::time_point<std::chrono::system_clock>>> map_opt_date_col;
//        std::map<std::string, std::optional<Enum>> map_opt_enum_col;

        std::map<std::string, std::optional<link<AllTypesObjectLink>>> map_link_col;
        std::map<std::string, std::optional<link<AllTypesObjectEmbedded>>> map_embedded_col;
    };

    REALM_SCHEMA(AllTypes,
                 _id, double_col, bool_col, str_col, enum_col, date_col, uuid_col, object_id_col, binary_col, mixed_col,
                 opt_int_col, opt_double_col, opt_str_col, opt_bool_col, opt_enum_col,
                 opt_date_col, opt_uuid_col, opt_object_id_col, opt_binary_col, opt_obj_col, opt_embedded_obj_col,
                 list_int_col, list_double_col, list_bool_col, list_str_col, list_uuid_col, list_object_id_col, list_binary_col,
                 list_date_col, list_mixed_col, list_obj_col, list_embedded_obj_col,
//                 list_opt_int_col, list_opt_double_col, list_opt_bool_col, list_opt_str_col,
//                 list_opt_uuid_col, list_opt_object_id_col, list_opt_binary_col, list_opt_date_col,
                 map_int_col, map_double_col, map_bool_col, map_str_col, map_uuid_col, map_object_id_col, map_binary_col,
                 map_date_col, map_enum_col, map_mixed_col, map_link_col, map_embedded_col

//                 map_opt_int_col, map_opt_double_col, map_opt_bool_col, map_opt_str_col, map_opt_uuid_col,
//                 map_opt_object_id_col, map_opt_binary_col, map_opt_date_col, map_opt_enum_col
                 )
}

TEST_CASE("experimental", "[experimental]") {
    SECTION("write return multiple") {
        realm_path path;
        experimental::db db = experimental::open(path);
        experimental::managed<experimental::Person> managed_person = db.write([&db]() {
            experimental::Person person;
            person.name = "Jack";
            person.age = 27;
            experimental::Dog dog;
            dog.name = "fido";
            person.dog = &dog;

            return db.add(std::move(person));
        });
        auto blah = std::string();
        db.write([&managed_person] {
            managed_person.name = "Jim";
        });
        CHECK(managed_person.name.value() == "Jim");

        db.write([&managed_person] {
            managed_person.name = "Meghna";
        });

        CHECK(managed_person.name.value() == "Meghna");
        CHECK(managed_person.dog->name == "fido");
        CHECK(managed_person.dog->owners[0].name == "Meghna");
        CHECK(managed_person.dog->owners.size() == 1);
    }
}

//TEST_CASE("experimental", "[experimental]") {
//    SECTION("write return multiple") {
//        auto lambda = []() -> ::task {
//            realm_path path;
//            experimental::db db = experimental::open(path);
//            auto managed_person = db.write([&db]() {
//                experimental::Person person;
//                person.name = "Jack";
//                person.age = 27;
//                person.dog->name = "fido";
//                return db.add(std::move(person));
//            });
//            auto blah = std::string();
//            db.write([&managed_person] {
//                managed_person.name = "Jim";
//            });
//            auto change = generator();
//            CHECK(!change.is_deleted);
//            CHECK(!change.error);
//            CHECK(std::get<std::string>(*change.property.old_value) == "Jack");
//            CHECK(std::get<std::string>(*change.property.new_value) == "Jim");
//            CHECK(change.property.name == "name");
//
//            db.write([&managed_person] {
//                managed_person.name = "Meghna";
//            });
//            change = generator();
//            CHECK(!change.is_deleted);
//            CHECK(!change.error);
//            CHECK(std::get<std::string>(*change.property.old_value) == "Jim");
//            CHECK(std::get<std::string>(*change.property.new_value) == "Meghna");
//            CHECK(change.property.name == "name");
//            co_return;
//        };
//        auto task = lambda();
//        task.h_.resume();
//        while (!task.h_.done()) std::this_thread::sleep_for(std::chrono::seconds(1));
//    }
//    using namespace realm::experimental;
//    {
//        realm_path path;
//        experimental::db db = experimental::open(path);
//        db.write([&db]() {
//            experimental::Person person;
//            person.name = "Jack";
//            person.age = 27;
//            person.dog->name = "fido";
//            managed<experimental::Person> managed_person = db.add(std::move(person));
//            CHECK(!managed_person.name.empty());
//            CHECK(managed_person.name.starts_with("Ja"));
//            CHECK(managed_person.name.ends_with("ck"));
//            CHECK(managed_person.name >= "Jack");
//            CHECK(managed_person.name == "Jack");
//            managed_person.name.pop_back();
//            CHECK(managed_person.name.ends_with("c"));
//            CHECK(managed_person.name.append("k") == "Jack");
//            CHECK(managed_person.age.value() == 27);
//            CHECK(managed_person.name.value() == "Jack");
//            CHECK(managed_person.dog->name == "fido");
//            return;
//        });
//    }
//    {
//        realm_path path;
//        experimental::db db2 = experimental::open(path);
//        db2.write([&db2] {
//            experimental::AllTypes person;
//            auto managed_person = db2.add(std::move(person));
//            CHECK(managed_person._id.value() == 0);
//            CHECK(managed_person.str_col.value().empty());
//        });
//    }
//    {
//        realm_path path;
//        experimental::dump_db db = experimental::dump_db(path);
//        std::vector<experimental::AllTypes> v;
//        for (size_t i = 0; i < 1; i++) {
//            v.emplace_back(experimental::AllTypes());
//        }
//        db.insert(v);
//    }
//}
namespace realm::experimental {

TEST_CASE("flex_sync_beta") {
        auto app = realm::App("typestest-ypkgr");
    SECTION("all") {
        app.get_sync_manager().set_log_level(logger::level::all);
        auto user = app.login(realm::App::credentials::anonymous()).get();
        auto flx_sync_config = user.flexible_sync_configuration();

        experimental::db synced_realm = experimental::db(std::move(flx_sync_config));

        auto update_success = synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                                    subs.add<AllTypes>("foo-strings", [](auto &obj) {
                                                        return obj.str_col == "foo";
                                                    });
                                                    subs.add<AllTypesObjectLink>("foo-link");
                                                          }).get();


        CHECK(update_success == true);

        auto sub = *synced_realm.subscriptions().find("foo-strings");
        CHECK(sub.name == "foo-strings");
        CHECK(sub.object_class_name == "AllTypes");
        CHECK(sub.query_string == "str_col == \"foo\"");

        auto non_existent_sub = synced_realm.subscriptions().find("non-existent");
        CHECK((non_existent_sub == std::nullopt) == true);

        synced_realm.write([&synced_realm]() {
            AllTypes o;
            o._id = 1;
            o.str_col = "foo";
            synced_realm.add(std::move(o));
        });

        test::wait_for_sync_uploads(user).get();
        test::wait_for_sync_downloads(user).get();
        synced_realm.write([]() {}); // refresh realm
        auto objs = synced_realm.objects<AllTypes>();

        CHECK(objs[0]._id == 1);

        synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                        subs.update_subscription<AllTypes>("foo-strings", [](auto &obj) {
                                            return obj.str_col == "bar" && obj._id == 1230;
                                        });
                                    }).get();

        auto sub2 = *synced_realm.subscriptions().find("foo-strings");
        CHECK(sub2.name == "foo-strings");
        CHECK(sub2.object_class_name == "AllTypes");
        CHECK(sub2.query_string == "str_col == \"bar\" and _id == 1230");

        synced_realm.write([&synced_realm]() {
            AllTypes o;
            o._id = 1230;
            o.str_col = "bar";
            synced_realm.add(std::move(o));
        });

        test::wait_for_sync_uploads(user).get();
        test::wait_for_sync_downloads(user).get();

        synced_realm.refresh();
        objs = synced_realm.objects<AllTypes>();
        CHECK(objs.size() == 1);

        synced_realm.subscriptions().update([](realm::mutable_sync_subscription_set &subs) {
                                        subs.update_subscription<AllTypes>("foo-strings");
                                    }).get();
//
        auto sub3 = *synced_realm.subscriptions().find("foo-strings");
        CHECK(sub3.name == "foo-strings");
        CHECK(sub3.object_class_name == "AllTypes");
        CHECK(sub3.query_string == "TRUEPREDICATE");

        test::wait_for_sync_uploads(user).get();
        test::wait_for_sync_downloads(user).get();

        synced_realm.refresh();
        objs = synced_realm.objects<AllTypes>();
        CHECK(objs.size() == 2);

    }
}

TEST_CASE("object_initialization_beta") {
    realm_path path;
    auto date = std::chrono::time_point<std::chrono::system_clock>();
    auto uuid = realm::uuid();
    auto link1 = AllTypesObjectLink();
    link1._id = 1;
    link1.str_col = "link object";

    auto link2 = AllTypesObjectLink();
    link2._id = 2;
    link2.str_col = "link object 2";

    auto link3 = AllTypesObjectLink();
    link3._id = 3;
    link3.str_col = "link object 3";

    AllTypesObjectEmbedded embedded_obj1;
    embedded_obj1.str_col = "embedded obj1";
    AllTypesObjectEmbedded embedded_obj2;
    embedded_obj2.str_col = "embedded obj2";
    AllTypesObjectEmbedded embedded_obj3;
    embedded_obj3.str_col = "embedded obj3";

    auto object_id = realm::object_id::generate();

    AllTypes obj;
    obj._id = 123;
    obj.double_col = 12.34;
    obj.bool_col = true;
    obj.str_col = std::string("foo");
    obj.enum_col = AllTypes::Enum::two;
    obj.date_col = date;
    obj.uuid_col = uuid;
    obj.object_id_col = object_id;
    obj.binary_col = std::vector<uint8_t>({1});
    obj.mixed_col = AllTypes::my_mixed("mixed");
    //            .my_mixed_col = AllTypesObject::foo_mixed(o2),

    obj.opt_int_col = 2;
    obj.opt_double_col = 2.34;
    obj.opt_str_col = "opt string";
    obj.opt_bool_col = true;
    obj.opt_enum_col = AllTypes::Enum::two;
    obj.opt_date_col = date;
    obj.opt_uuid_col = uuid;
    obj.opt_object_id_col = object_id;
    obj.opt_binary_col = std::vector<uint8_t>({1});

    obj.opt_obj_col = link1,
    obj.opt_embedded_obj_col = embedded_obj1,

    obj.list_int_col = std::vector<int64_t>({1});
    obj.list_double_col = std::vector<double>({1.23});
    obj.list_bool_col = std::vector<bool>({true});
    obj.list_str_col = std::vector<std::string>({"bar"});
    obj.list_uuid_col = std::vector<realm::uuid>({uuid});
    obj.list_object_id_col = std::vector<realm::object_id>({object_id});
    obj.list_binary_col = std::vector<std::vector<uint8_t>>({{1}});
    obj.list_date_col = std::vector<std::chrono::time_point<std::chrono::system_clock>>({date});
    obj.list_mixed_col = std::vector<realm::mixed>({realm::mixed("mixed str")});
    obj.list_obj_col = std::vector<link<AllTypesObjectLink>>({std::move(link2)});
    obj.list_embedded_obj_col = std::vector<link<AllTypesObjectEmbedded>>({embedded_obj2});

//    obj.list_opt_int_col = std::vector<std::optional<int64_t>>({123, std::nullopt});
//    obj.list_opt_double_col = std::vector<std::optional<double>>({123.456, std::nullopt});
//    obj.list_opt_bool_col = std::vector<std::optional<bool>>({true, std::nullopt});
//    obj.list_opt_str_col = std::vector<std::optional<std::string>>({"hello", std::nullopt});
//    obj.list_opt_uuid_col = std::vector<std::optional<realm::uuid>>({uuid, std::nullopt});
//    obj.list_opt_object_id_col = std::vector<std::optional<realm::object_id>>({object_id, std::nullopt});
//    obj.list_opt_binary_col = std::vector<std::optional<std::vector<uint8_t>>>({std::vector<uint8_t>({1}), std::nullopt});
//    obj.list_opt_date_col = std::vector<std::optional<std::chrono::time_point<std::chrono::system_clock>>>({date, std::nullopt});

    obj.map_int_col = std::map<std::string, int64_t>({{"foo", 1}});
    obj.map_double_col = std::map<std::string, double>({{"foo", 1.23}});
    obj.map_bool_col = std::map<std::string, bool>({{"foo", true}});
    obj.map_str_col = std::map<std::string, std::string>({{"foo", "bar"}});
    obj.map_uuid_col = std::map<std::string, realm::uuid>({{"foo", uuid}});
    obj.map_object_id_col = std::map<std::string, realm::object_id>({{"foo", object_id}});
    obj.map_binary_col = std::map<std::string, std::vector<std::uint8_t>>({{"foo", {1}}});
    obj.map_date_col = std::map<std::string, std::chrono::time_point<std::chrono::system_clock>>({{"foo", date}});
    obj.map_enum_col = std::map<std::string, AllTypes::Enum>({{"foo", AllTypes::Enum::one}});
    obj.map_mixed_col = std::map<std::string, realm::mixed>({{"foo", realm::mixed("bar")}});
    obj.map_link_col = std::map<std::string, std::optional<link<AllTypesObjectLink>>>({{"foo", link3}});
    obj.map_embedded_col = std::map<std::string, std::optional<link<AllTypesObjectEmbedded>>>({{"foo", embedded_obj3}});

//    obj.map_opt_int_col = std::map<std::string, std::optional<int64_t>>({{"foo", 1}, {"bar", std::nullopt}});
//    obj.map_opt_double_col = std::map<std::string, std::optional<double>>({{"foo", 1.23}, {"bar", std::nullopt}});
//    obj.map_opt_bool_col = std::map<std::string, std::optional<bool>>({{"foo", true}, {"bar", std::nullopt}});
//    obj.map_opt_str_col = std::map<std::string, std::optional<std::string>>({{"foo", "bar"}, {"bar", std::nullopt}});
//    obj.map_opt_uuid_col = std::map<std::string, std::optional<realm::uuid>>({{"foo", uuid}, {"bar", std::nullopt}});
//    obj.map_opt_object_id_col = std::map<std::string, std::optional<realm::object_id>>({{"foo", object_id}, {"bar", std::nullopt}});
//    obj.map_opt_binary_col = std::map<std::string, std::optional<std::vector<uint8_t>>>({  {"foo", std::vector<uint8_t>({1})}, {"bar", std::nullopt }});
//    obj.map_opt_date_col = std::map<std::string, std::optional<std::chrono::time_point<std::chrono::system_clock>>>({{"foo", date}, {"bar", std::nullopt}});
//    obj.map_opt_enum_col = std::map<std::string, std::optional<AllTypes::Enum>>({{"foo", AllTypes::Enum::one}, {"bar", std::nullopt}});

    experimental::db db = experimental::open(path);
    Dog d;
    d.name = "foo";
    d.name2 = "foobar";
    auto managed_obj = db.write([&obj, &db]() {
        return db.add(std::move(obj));
    });

    auto managed_dog = db.write([&d, &db]() {
        return db.add(std::move(d));
    });

    CHECK(managed_obj._id == (int64_t)123);
    CHECK(managed_obj.double_col == 12.34);
    CHECK(managed_obj.str_col == "foo");
    CHECK(managed_obj.bool_col == true);
    CHECK(managed_obj.enum_col == AllTypes::Enum::two);
    CHECK(managed_obj.date_col == date);
    CHECK(managed_obj.uuid_col.value() == uuid);
    CHECK(managed_obj.binary_col.value() == std::vector<uint8_t>({1}));
    CHECK(managed_obj.mixed_col.value() == AllTypes::my_mixed("mixed"));

    CHECK(managed_obj.object_id_col.value() == object_id);

    CHECK(managed_obj.opt_int_col == 2);
    CHECK(managed_obj.opt_double_col == 2.34);
    CHECK(managed_obj.opt_str_col == "opt string");
    CHECK(managed_obj.opt_bool_col == true);
    CHECK(managed_obj.opt_enum_col == AllTypes::Enum::two);
    CHECK(managed_obj.opt_date_col == date);
    CHECK(managed_obj.opt_uuid_col == uuid);
    CHECK(managed_obj.opt_binary_col.value() == std::vector<uint8_t>({1}));
    CHECK(managed_obj.opt_object_id_col == object_id);

    CHECK(managed_obj.list_int_col[0] == 1);
    CHECK(managed_obj.list_double_col[0] == 1.23);
    CHECK(managed_obj.list_bool_col[0] == true);
    CHECK(managed_obj.list_str_col[0] == "bar");
    CHECK(managed_obj.list_uuid_col[0] == uuid);
    CHECK(managed_obj.list_date_col[0] == date);
    CHECK(managed_obj.list_uuid_col[0] == uuid);
    CHECK(managed_obj.list_mixed_col[0] == realm::mixed("mixed str"));
    auto other_obj = db.objects<realm::experimental::AllTypesObjectLink>()[1];
    CHECK(managed_obj.opt_obj_col->str_col == "link object");
    auto a = managed_obj.list_obj_col[0].str_col.value();
    CHECK(managed_obj.list_obj_col[0].str_col == "link object 2");
    CHECK(managed_obj.list_obj_col[0] == other_obj);
    auto b = managed_obj.list_embedded_obj_col[0].str_col.value();
    auto c = managed_obj.opt_embedded_obj_col->str_col.value();

    CHECK(managed_obj.list_embedded_obj_col[0].str_col == "embedded obj2");
    CHECK(managed_obj.opt_embedded_obj_col->str_col == "embedded obj1");
    CHECK(managed_obj.list_embedded_obj_col[0] == managed_obj.list_embedded_obj_col[0]);

//    CHECK(managed_obj.list_opt_int_col[0] == 123);
//    CHECK(managed_obj.list_opt_int_col[1] == std::nullopt);
//    CHECK(managed_obj.list_opt_double_col[0] == 123.456);
//    CHECK(managed_obj.list_opt_double_col[1] == std::nullopt);
//    CHECK(managed_obj.list_opt_bool_col[0] == true);
//    CHECK(managed_obj.list_opt_bool_col[1] == std::nullopt);
//    CHECK(managed_obj.list_opt_str_col[0] == "hello");
//    CHECK(managed_obj.list_opt_str_col[1] == std::nullopt);
//    CHECK(managed_obj.list_opt_uuid_col[0] == uuid);
//    CHECK(managed_obj.list_opt_uuid_col[1] == std::nullopt);
//    CHECK(managed_obj.list_opt_object_id_col[0] == object_id);
//    CHECK(managed_obj.list_opt_object_id_col[1] == std::nullopt);
//    CHECK(managed_obj.list_opt_binary_col[0] == std::vector<uint8_t>({1}));
//    CHECK(managed_obj.list_opt_binary_col[1] == std::nullopt);
//    CHECK(managed_obj.list_opt_date_col[0] == date);
//    CHECK(managed_obj.list_opt_date_col[1] == std::nullopt);

    CHECK(managed_obj.map_int_col["foo"] == 1);
    CHECK(managed_obj.map_double_col["foo"] == 1.23);
    CHECK(managed_obj.map_bool_col["foo"] == true);
    CHECK(managed_obj.map_str_col["foo"] == "bar");
    CHECK(managed_obj.map_uuid_col["foo"] == uuid);
    CHECK(managed_obj.map_date_col["foo"] == date);
    CHECK(managed_obj.map_uuid_col["foo"] == uuid);
    CHECK(managed_obj.map_mixed_col["foo"] == "bar");
    CHECK((*managed_obj.map_link_col["foo"]).value()->str_col == "link object 3");
    auto other_obj2 = db.objects<realm::experimental::AllTypesObjectLink>()[2];
    auto other_obj3 = db.objects<realm::experimental::AllTypes>()[0];
    CHECK(*managed_obj.map_link_col["foo"] == other_obj2);
    std::optional<link<AllTypesObjectEmbedded>> map_embedded_col = *other_obj3.map_embedded_col["foo"];
    CHECK(*managed_obj.map_embedded_col["foo"] == map_embedded_col);


//    CHECK(managed_obj.map_opt_int_col["foo"] == 1);
//    CHECK(managed_obj.map_opt_int_col["bar"] == std::nullopt);
//
//    CHECK(managed_obj.map_opt_double_col["foo"] == 1.23);
//    CHECK(managed_obj.map_opt_double_col["bar"] == std::nullopt);
//
//    CHECK(managed_obj.map_opt_bool_col["foo"] == true);
//    CHECK(managed_obj.map_opt_bool_col["bar"] == std::nullopt);
//
//    CHECK(managed_obj.map_opt_str_col["foo"] == "bar");
//    CHECK(managed_obj.map_opt_str_col["bar"] == std::nullopt);
//
//    CHECK(managed_obj.map_opt_uuid_col["foo"] == uuid);
//    CHECK(managed_obj.map_opt_uuid_col["bar"] == std::nullopt);
//
//    CHECK(managed_obj.map_opt_date_col["foo"] == date);
//    CHECK(managed_obj.map_opt_date_col["bar"] == std::nullopt);
//
//    CHECK(managed_obj.map_opt_object_id_col["foo"] == object_id);
//    CHECK(managed_obj.map_opt_object_id_col["bar"] == std::nullopt);
//
//    CHECK(managed_obj.map_opt_binary_col["foo"] == std::vector<uint8_t>({1}));
//    CHECK(managed_obj.map_opt_binary_col["bar"] == std::nullopt);
//
//    CHECK(managed_obj.map_opt_enum_col["foo"] == AllTypes::Enum::one);
//    CHECK(managed_obj.map_opt_enum_col["bar"] == std::nullopt);

    auto allTypeObjects = db.objects<AllTypes>();

    auto results_obj = allTypeObjects[0];
    CHECK(results_obj._id == 123);
    CHECK(results_obj.double_col == 12.34);
    CHECK(results_obj.str_col == "foo");
    CHECK(results_obj.bool_col == true);
    CHECK(results_obj.enum_col == AllTypes::Enum::two);
    CHECK(results_obj.date_col == date);
    CHECK(results_obj.uuid_col == uuid);
    CHECK(results_obj.binary_col == std::vector<uint8_t>({1}));
    CHECK(results_obj.mixed_col == "mixed");
    CHECK(results_obj.object_id_col == object_id);

    CHECK(results_obj.opt_int_col == 2);
    CHECK(results_obj.opt_double_col == 2.34);
    CHECK(results_obj.opt_str_col == "opt string");
    CHECK(results_obj.opt_bool_col == true);
    CHECK(results_obj.opt_enum_col == AllTypes::Enum::two);
    CHECK(results_obj.opt_date_col == date);
    CHECK(results_obj.opt_uuid_col == uuid);
    CHECK(results_obj.opt_binary_col == std::vector<uint8_t>({1}));
    CHECK(results_obj.opt_object_id_col == object_id);

//    CHECK(*results_obj.opt_obj_col == o);
//    CHECK(*results_obj.opt_embedded_obj_col == embedded_obj);

    CHECK(results_obj.list_int_col[0] == 1);
    CHECK(results_obj.list_double_col[0] == 1.23);
    CHECK(results_obj.list_bool_col[0] == true);
    CHECK(results_obj.list_str_col[0] == "bar");
    CHECK(results_obj.list_uuid_col[0] == uuid);
    CHECK(results_obj.list_date_col[0] == date);
    CHECK(results_obj.list_uuid_col[0] == uuid);
    CHECK(results_obj.list_mixed_col[0] == realm::mixed("mixed str"));
//    CHECK(results_obj.list_obj_col[0] == o);
//    CHECK(results_obj.list_embedded_obj_col[0] == embedded_obj);


//    CHECK(results_obj.list_opt_int_col[0] == 123);
//    CHECK(results_obj.list_opt_int_col[1] == std::nullopt);
//    CHECK(results_obj.list_opt_double_col[0] == 123.456);
//    CHECK(results_obj.list_opt_double_col[1] == std::nullopt);
//    CHECK(results_obj.list_opt_bool_col[0] == true);
//    CHECK(results_obj.list_opt_bool_col[1] == std::nullopt);
//    CHECK(results_obj.list_opt_str_col[0] == "hello");
//    CHECK(results_obj.list_opt_str_col[1] == std::nullopt);
//    CHECK(results_obj.list_opt_uuid_col[0] == uuid);
//    CHECK(results_obj.list_opt_uuid_col[1] == std::nullopt);
//    CHECK(results_obj.list_opt_object_id_col[0] == object_id);
//    CHECK(results_obj.list_opt_object_id_col[1] == std::nullopt);
//    CHECK(results_obj.list_opt_binary_col[0] == std::vector<uint8_t>({1}));
//    CHECK(results_obj.list_opt_binary_col[1] == std::nullopt);
//    CHECK(results_obj.list_opt_date_col[0] == date);
//    CHECK(results_obj.list_opt_date_col[1] == std::nullopt);

    CHECK(results_obj.map_int_col["foo"] == 1);
    CHECK(results_obj.map_double_col["foo"] == 1.23);
    CHECK(results_obj.map_bool_col["foo"] == true);
    CHECK(results_obj.map_str_col["foo"] == "bar");
    CHECK(results_obj.map_uuid_col["foo"] == uuid);
    CHECK(results_obj.map_date_col["foo"] == date);
    CHECK(results_obj.map_uuid_col["foo"] == uuid);
    CHECK(results_obj.map_mixed_col["foo"] == realm::mixed("bar"));
//    CHECK(results_obj.map_link_col["foo"] == o);
//    CHECK(results_obj.map_embedded_col["foo"] == embedded_obj);
}


TEST_CASE("object_notifications_beta") {
    realm_path path;
    SECTION("observe") {
        experimental::db realm = experimental::open(path);

        auto date = std::chrono::time_point<std::chrono::system_clock>();
        auto uuid = realm::uuid();
        auto foo = AllTypes();
        auto o = AllTypesObjectLink();
        AllTypesObjectLink o2;
        AllTypesObjectEmbedded o3;
        AllTypesObjectEmbedded o4;

        auto managed_foo = realm.write([&foo, &realm]() {
            return realm.add(std::move(foo));
        });

        int run_count = 0;
        auto token = managed_foo.observe([&](auto change) {
            if (run_count == 1) {
                CHECK(change.is_deleted);
            } else {
                CHECK(change.property_changes.size() == 24);
                for (auto& prop_change : change.property_changes) {
                    if (prop_change.name == "str_col" && prop_change.new_value) {
                        CHECK(std::get<std::string>(*prop_change.new_value) == "foo");
                    } else if (prop_change.name == "bool_col" && prop_change.new_value) {
                        CHECK(std::get<bool>(*prop_change.new_value) == true);
                    } else if (prop_change.name == "enum_col" && prop_change.new_value) {
                        CHECK(std::get<AllTypes::Enum>(*prop_change.new_value) == AllTypes::Enum::two);
                    } else if (prop_change.name == "date_col" && prop_change.new_value) {
                        CHECK(std::get<std::chrono::time_point<std::chrono::system_clock>>(*prop_change.new_value) == date);
                    } else if (prop_change.name == "uuid_col" && prop_change.new_value) {
                        CHECK(std::get<realm::uuid>(*prop_change.new_value) == uuid);
                    } else if (prop_change.name == "binary_col" && prop_change.new_value) {
                        CHECK(std::get<std::vector<uint8_t>>(*prop_change.new_value) == std::vector<uint8_t>({1}));
                    } else if (prop_change.name == "mixed_col" && prop_change.new_value) {
                        auto val = std::get<AllTypes::my_mixed>(*prop_change.new_value);
                        CHECK(std::get<std::string>(val) == "mixed");
                    } else if (prop_change.name == "opt_str_col" && prop_change.new_value) {
                        CHECK(std::get<std::optional<std::string>>(*prop_change.new_value) == "opt string");
                    } else if (prop_change.name == "opt_bool_col" && prop_change.new_value) {
                        CHECK(std::get<std::optional<bool>>(*prop_change.new_value) == true);
                    } else if (prop_change.name == "opt_enum_col" && prop_change.new_value) {
                        CHECK(std::get<std::optional<AllTypes::Enum>>(*prop_change.new_value) == AllTypes::Enum::two);
                    } else if (prop_change.name == "opt_date_col" && prop_change.new_value) {
                        CHECK(std::get<std::optional<std::chrono::time_point<std::chrono::system_clock>>>(*prop_change.new_value) == date);
                    } else if (prop_change.name == "opt_uuid_col" && prop_change.new_value) {
                        CHECK(std::get<std::optional<realm::uuid>>(*prop_change.new_value) == uuid);
                    } else if (prop_change.name == "opt_binary_col" && prop_change.new_value) {
                        CHECK(std::get<std::optional<std::vector<uint8_t>>>(*prop_change.new_value) == std::vector<uint8_t>({1}));
                    } else if (prop_change.name == "opt_obj_col" && prop_change.new_value) {
//                        auto obj = std::get<std::optional<AllTypesObjectLink>>(*prop_change.new_value);
//                        CHECK(*(obj->str_col) == "link object");
                    } else if (prop_change.name == "opt_embedded_obj_col" && prop_change.new_value) {
//                        auto obj = std::get<std::optional<AllTypesObjectEmbedded>>(*prop_change.new_value);
//                        CHECK(*(obj->str_col) == "embedded obj");
                    } else if (prop_change.name == "list_int_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<int64_t>>(*prop_change.new_value);
                        // Cocoa does not populate collection changes for an object and neither should we for perforamnce reasons.
                        CHECK(obj.size() == 0);
                    } else if (prop_change.name == "list_bool_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<bool>>(*prop_change.new_value);
                        CHECK(obj.size() == 0);
                    } else if (prop_change.name == "list_str_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<std::string>>(*prop_change.new_value);
                        CHECK(obj.size() == 0);
                    } else if (prop_change.name == "list_uuid_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<realm::uuid>>(*prop_change.new_value);
                        CHECK(obj.size() == 0);
                    } else if (prop_change.name == "list_binary_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<std::vector<uint8_t>>>(*prop_change.new_value);
                        CHECK(obj.size() == 0);
                    } else if (prop_change.name == "list_date_col" && prop_change.new_value) {
                        auto obj = std::get<std::vector<std::chrono::time_point<std::chrono::system_clock>>>(*prop_change.new_value);
                        CHECK(obj.size() == 0);
                    } else if (prop_change.name == "list_mixed_col" && prop_change.new_value) {
//                        auto obj = std::get<std::vector<realm::mixed>>(*prop_change.new_value);
//                        CHECK(obj.size() == 0);
                    } else if (prop_change.name == "list_obj_col" && prop_change.new_value) {
//                        auto obj = std::get<std::vector<AllTypesObjectLink>>(*prop_change.new_value);
//                        CHECK(obj.size() == 0);
                    } else if (prop_change.name == "list_embedded_obj_col" && prop_change.new_value) {
//                        auto obj = std::get<std::vector<AllTypesObjectEmbedded>>(*prop_change.new_value);
//                        CHECK(obj.size() == 0);
                    }
                }
            }

            run_count++;
        });

        realm.write([&]() {
            managed_foo.str_col = "foo";
            managed_foo.bool_col = true;
            managed_foo.enum_col = AllTypes::Enum::two;
            managed_foo.date_col = date;
            managed_foo.uuid_col = realm::uuid();
            managed_foo.binary_col.push_back(1);
            managed_foo.mixed_col = std::string("mixed");

            managed_foo.opt_int_col = 2;
            managed_foo.opt_double_col = 1.23;
            managed_foo.opt_str_col = "opt string";
            managed_foo.opt_bool_col = true;
            managed_foo.opt_enum_col = AllTypes::Enum::two;
            managed_foo.opt_date_col = date;
            managed_foo.opt_uuid_col = uuid;
            managed_foo.opt_binary_col = std::vector<uint8_t>({1});
            o._id = 1;
            o.str_col = "link object";
            managed_foo.opt_obj_col = std::move(o);
            o3.str_col = "embedded obj";
            managed_foo.opt_embedded_obj_col = std::move(o3);

            managed_foo.list_int_col.push_back(1);
            managed_foo.list_bool_col.push_back(true);
            managed_foo.list_str_col.push_back("bar");
            managed_foo.list_uuid_col.push_back(uuid);
            managed_foo.list_binary_col.push_back({1});
            managed_foo.list_date_col.push_back(date);
            managed_foo.list_mixed_col.push_back("mixed str");
            o2._id = 2;
            o2.str_col = "link object 2";
            foo.list_obj_col.push_back(o2);
            o4.str_col = "embedded obj 2";
            foo.list_embedded_obj_col.push_back(o4);
        });
        realm.refresh();

//        CHECK(o.is_managed());
//        CHECK(o.str_col == "link object");
//        CHECK(o2.is_managed());
//        CHECK(o2.str_col == "link object 2");
//        CHECK(o3.is_managed());
//        CHECK(o3.str_col == "embedded obj");
//        CHECK(o4.is_managed());
//        CHECK(o4.str_col == "embedded obj 2");

//        CHECK(foo.opt_obj_col->is_managed());
//        CHECK(*(foo.opt_obj_col->str_col) == "link object");
//        CHECK(foo.list_obj_col[0].is_managed());
//        CHECK(foo.list_obj_col[0].str_col == "link object 2");
//        CHECK(foo.opt_embedded_obj_col->is_managed());
//        CHECK(foo.opt_embedded_obj_col->str_col == "embedded obj");
//        CHECK(foo.list_embedded_obj_col[0].is_managed());
//        CHECK(foo.list_embedded_obj_col[0].str_col == "embedded obj 2");

        realm.write([&managed_foo, &realm] {
            realm.remove(managed_foo);
        });
        realm.refresh();

        CHECK(run_count == 2);
    }

//    SECTION("optional objects") {
//        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({path});
//
//        auto foo = AllTypesObject();
//        auto o = AllTypesObjectLink();
//        o.str_col = "bar";
//        AllTypesObjectEmbedded o2;
//        o2.str_col = "embedded bar";
//        foo.opt_obj_col = o;
//        foo.opt_embedded_obj_col = o2;
//
//        realm.write([&foo, &realm] {
//            realm.add(foo);
//        });
//
//        realm.write([&] {
//            foo.opt_obj_col = std::nullopt;
//            foo.opt_embedded_obj_col = std::nullopt;
//        });
//
//        CHECK(*(foo.opt_obj_col) == std::nullopt);
//        CHECK(*(foo.opt_embedded_obj_col) == std::nullopt);
//    }
}

TEST_CASE("map_beta", "[map]") {
    realm_path path;
    SECTION("unmanaged_managed_map_get_set", "[mixed]") {
        auto obj = AllTypes();
        obj.map_int_col = {
                {"a", 42}
        };
        obj.map_str_col = {
                {"a", std::string("foo")}
        };
        obj.map_bool_col = {
                {"a", true}
        };
        obj.map_enum_col = {
                {"a", AllTypes::Enum::one}
        };
        obj.map_uuid_col = {
                {"a", realm::uuid()}
        };
        obj.map_binary_col = {
                {"a", {0, 1, 2}}
        };
        obj.map_date_col = {
                {"a", std::chrono::system_clock::now()}
        };
        obj.map_link_col = {
                {"a", AllTypesObjectLink()}
        };
        obj.map_embedded_col = {
                {"a", AllTypesObjectEmbedded()}
        };
        CHECK(obj.map_int_col["a"] == 42);
        CHECK(obj.map_str_col["a"] == "foo");
        CHECK(obj.map_bool_col["a"] == true);
        CHECK(obj.map_enum_col["a"] == AllTypes::Enum::one);
        CHECK(obj.map_uuid_col["a"] == realm::uuid());
        CHECK(obj.map_binary_col["a"] == std::vector<uint8_t>{0, 1, 2});
//        CHECK(obj.map_link_col["a"] == AllTypesObjectLink());
//        CHECK(obj.map_embedded_col["a"] == AllTypesObjectEmbedded());
        experimental::db realm = experimental::open(path);
        auto managed_obj = realm.write([&obj, &realm]() {
            return realm.add(std::move(obj));
        });
        CHECK(managed_obj.map_int_col["a"] == 42);
        CHECK(managed_obj.map_str_col["a"] == "foo");
        CHECK(managed_obj.map_bool_col["a"] == true);
        CHECK(managed_obj.map_enum_col["a"] == AllTypes::Enum::one);
        CHECK(managed_obj.map_uuid_col["a"] == realm::uuid());
        CHECK(managed_obj.map_binary_col["a"] == std::vector<uint8_t>{0, 1, 2});
//        CHECK(managed_obj.map_link_col["a"] == AllTypesObjectLink());
//        CHECK(managed_obj.map_embedded_col["a"] == AllTypesObjectEmbedded());

        realm.write([&managed_obj] {
            managed_obj.map_int_col["b"] = 84;
            managed_obj.map_str_col["b"] = "bar";
            managed_obj.map_bool_col["b"] = true;
            managed_obj.map_enum_col["b"] = AllTypes::Enum::two;
            managed_obj.map_uuid_col["b"] = realm::uuid();
            managed_obj.map_binary_col["b"] = std::vector<uint8_t>{3,4,5};
            auto o = AllTypesObjectLink();
            o.str_col = "test";
            managed_obj.map_link_col["b"] = o;
            CHECK((*managed_obj.map_link_col["b"]).value()->str_col == "test");
            auto eo = AllTypesObjectEmbedded();
            eo.str_col = "test embedded";
            managed_obj.map_embedded_col["b"] = eo;
            CHECK((*managed_obj.map_embedded_col["b"]).value()->str_col == "test embedded");
        });
        CHECK(managed_obj.map_int_col["a"] == 42);
        CHECK(managed_obj.map_int_col["b"] == 84);
        CHECK(managed_obj.map_str_col["a"] == "foo");
        CHECK(managed_obj.map_str_col["b"] == "bar");
        CHECK(managed_obj.map_bool_col["a"] == true);
        CHECK(managed_obj.map_bool_col["b"] == true);
        CHECK(managed_obj.map_enum_col["a"] == AllTypes::Enum::one);
        CHECK(managed_obj.map_enum_col["b"] == AllTypes::Enum::two);
        CHECK(managed_obj.map_uuid_col["a"] == realm::uuid());
        CHECK(managed_obj.map_uuid_col["b"] == realm::uuid());
        CHECK(managed_obj.map_binary_col["a"] == std::vector<uint8_t>{0, 1, 2});
        CHECK(managed_obj.map_binary_col["b"] == std::vector<uint8_t>{3, 4, 5});
//        CHECK(obj.map_link_col["b"] == AllTypesObjectLink());
//        CHECK(obj.map_embedded_col["b"] == AllTypesObjectEmbedded());

        realm.write([&managed_obj] {
            managed_obj.map_link_col["b"] = std::nullopt;
            managed_obj.map_embedded_col["b"] = std::nullopt;
        });

        CHECK(obj.map_link_col["b"] == std::nullopt);
        CHECK(obj.map_embedded_col["b"] == std::nullopt);
    }


}

} // namespace experimental
