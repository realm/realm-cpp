#include "test_utils.hpp"
#include "test_objects.hpp"
#include <cpprealm/notifications.hpp>

using namespace realm;

TEST(list) {
    auto realm = realm::open<AllTypesObject, AllTypesObjectLink, Dog>({.path=path});
    auto obj = AllTypesObject{};
    obj.list_int_col.push_back(42);
    CHECK_EQUALS(obj.list_int_col[0], 42);

    obj.list_obj_col.push_back(AllTypesObjectLink{.str_col="Fido"});
    CHECK_EQUALS(obj.list_obj_col[0].str_col, "Fido");
    CHECK_EQUALS(obj.list_int_col.size(), 1);
    for (auto& i : obj.list_int_col) {
        CHECK_EQUALS(i, 42);
    }
    realm.write([&realm, &obj]() {
        realm.add(obj);
    });

    CHECK_EQUALS(obj.list_int_col[0], 42);
    CHECK_EQUALS(obj.list_obj_col[0].str_col, "Fido");

    realm.write([&obj]() {
        obj.list_int_col.push_back(84);
        obj.list_obj_col.push_back(AllTypesObjectLink{._id=1, .str_col="Rex"});
    });
    size_t idx = 0;
    for (auto& i : obj.list_int_col) {
        CHECK_EQUALS(i, obj.list_int_col[idx]);
        ++idx;
    }
    CHECK_EQUALS(obj.list_int_col.size(), 2);
    CHECK_EQUALS(obj.list_int_col[0], 42);
    CHECK_EQUALS(obj.list_int_col[1], 84);
    CHECK_EQUALS(obj.list_obj_col[0].str_col, "Fido");
    CHECK_EQUALS(obj.list_obj_col[1].str_col, "Rex");
    co_return;
}

TEST(list_insert_remove_primitive) {
    auto obj = AllTypesObject();
    CHECK_EQUALS(obj.is_managed(), false);

    // unmanaged
    obj.list_int_col.push_back(1);
    obj.list_int_col.push_back(2);
    obj.list_int_col.push_back(3);
    CHECK_EQUALS(obj.list_int_col.size(), 3);

    obj.list_int_col.pop_back();
    CHECK_EQUALS(obj.list_int_col.size(), 2);
    obj.list_int_col.erase(0);
    CHECK_EQUALS(obj.list_int_col.size(), 1);
    obj.list_int_col.clear();
    CHECK_EQUALS(obj.list_int_col.size(), 0);
    obj.list_int_col.push_back(2);
    obj.list_int_col.push_back(4);
    CHECK_EQUALS(obj.list_int_col.find(4), 1);
    CHECK_EQUALS(obj.list_int_col[1], 4);

    auto realm = realm::open<AllTypesObject, AllTypesObjectLink, Dog>({.path=path});
    realm.write([&realm, &obj] {
        realm.add(obj);
    });

    // ensure values exist
    CHECK_EQUALS(obj.is_managed(), true);
    CHECK_EQUALS(obj.list_int_col.size(), 2);

    CHECK_THROWS([&obj] { obj.list_int_col.push_back(1); });

    realm.write([&realm, &obj] {
        obj.list_int_col.push_back(1);
    });
    CHECK_EQUALS(obj.list_int_col.size(), 3);
    CHECK_EQUALS(obj.list_int_col.find(1), 2);
    CHECK_EQUALS(obj.list_int_col[2], 1);

    realm.write([&realm, &obj] {
        obj.list_int_col.pop_back();
    });
    CHECK_EQUALS(obj.list_int_col.size(), 2);
    CHECK_EQUALS(obj.list_int_col.find(1), realm::npos);

    realm.write([&realm, &obj] {
        obj.list_int_col.erase(0);
    });
    CHECK_EQUALS(obj.list_int_col.size(), 1);

    realm.write([&realm, &obj] {
        obj.list_int_col.clear();
    });
    CHECK_EQUALS(obj.list_int_col.size(), 0);

    co_return;
}

TEST(list_insert_remove_object) {
    auto obj = AllTypesObject();
    CHECK_EQUALS(obj.is_managed(), false);

    auto o1 = AllTypesObjectLink();
    o1._id = 1;
    o1.str_col = "foo";
    auto o2 = AllTypesObjectLink();
    o2._id = 2;
    o2.str_col = "bar";
    auto o3 = AllTypesObjectLink();
    o3._id = 3;
    o3.str_col = "baz";
    auto o4 = AllTypesObjectLink();
    o4._id = 4;
    o4.str_col = "foo baz";
    auto o5 = AllTypesObjectLink();
    o5._id = 5;
    o5.str_col = "foo bar";

    // unmanaged
    obj.list_obj_col.push_back(o1);
    obj.list_obj_col.push_back(o2);
    obj.list_obj_col.push_back(o3);
    CHECK_EQUALS(obj.list_obj_col.size(), 3);

    obj.list_obj_col.pop_back();
    CHECK_EQUALS(obj.list_obj_col.size(), 2);
    obj.list_obj_col.erase(0);
    CHECK_EQUALS(obj.list_obj_col.size(), 1);
    obj.list_obj_col.clear();
    CHECK_EQUALS(obj.list_obj_col.size(), 0);
    obj.list_obj_col.push_back(o1);
    obj.list_obj_col.push_back(o2);
    obj.list_obj_col.push_back(o3);
    obj.list_obj_col.push_back(o4);

    CHECK_EQUALS(obj.list_obj_col.find(o4), realm::npos);
    CHECK_EQUALS(obj.list_obj_col[3].str_col, o4.str_col);

    auto realm = realm::open<AllTypesObject, AllTypesObjectLink, Dog>({.path=path});
    realm.write([&realm, &obj] {
        realm.add(obj);
    });

    // ensure values exist
    CHECK_EQUALS(obj.is_managed(), true);
    CHECK_EQUALS(obj.list_obj_col.size(), 4);

    CHECK_THROWS([&] { obj.list_obj_col.push_back(o5); });

    CHECK_EQUALS(o5.is_managed(), false);
    realm.write([&obj, &o5] {
        obj.list_obj_col.push_back(o5);
    });
    CHECK_EQUALS(o5.is_managed(), true);

    CHECK_EQUALS(obj.list_obj_col.size(), 5);
    CHECK_EQUALS(obj.list_obj_col.find(o5), 4);
    CHECK_EQUALS(obj.list_obj_col[4], o5);

    realm.write([&obj] {
        obj.list_obj_col.pop_back();
    });
    CHECK_EQUALS(obj.list_obj_col.size(), 4);
    CHECK_EQUALS(obj.list_obj_col.find(o5), realm::npos);

    realm.write([&realm, &obj] {
        obj.list_obj_col.erase(0);
    });
    CHECK_EQUALS(obj.list_obj_col.size(), 3);

    realm.write([&realm, &obj] {
        obj.list_obj_col.clear();
    });
    CHECK_EQUALS(obj.list_obj_col.size(), 0);

    co_return;
}

TEST(notifications_insertions) {
    auto obj = AllTypesObject();

    auto realm = realm::open<AllTypesObject, AllTypesObjectLink, Dog>({.path=path});
    realm.write([&realm, &obj] {
        realm.add(obj);
    });

    bool did_run = false;

    CollectionChange<std::vector<int>> change;

    int callback_count = 0;

    auto require_change = [&] {
        auto token = obj.list_int_col.observe([&](CollectionChange<std::vector<int>> c, std::exception_ptr) {
            CHECK_EQUALS(c.collection , &obj.list_int_col);
            callback_count++;
            change = std::move(c);
        });
        return token;
    };

    auto token = require_change();
    realm.write([&realm, &obj] {
        obj.list_int_col.push_back(456);
    });

    realm.write([] { });

    CHECK_EQUALS(change.insertions.size(), 1);

    realm.write([&realm, &obj] {
        obj.list_int_col.push_back(456);
    });

    realm.write([] { });

    CHECK_EQUALS(change.insertions.size(), 1);
    CHECK_EQUALS(callback_count, 3);

    co_return;
}

TEST(notifications_deletions) {
    auto obj = AllTypesObject();

    auto realm = realm::open<AllTypesObject, AllTypesObjectLink, Dog>({.path=path});
    realm.write([&realm, &obj] {
        realm.add(obj);
        obj.list_int_col.push_back(456);
    });

    bool did_run = false;

    CollectionChange<std::vector<int>> change;

    auto require_change = [&] {
        auto token = obj.list_int_col.observe([&](CollectionChange<std::vector<int>> c, std::exception_ptr) {
            did_run = true;
            change = std::move(c);
        });
        return token;
    };

    auto token = require_change();
    realm.write([&realm, &obj] {
        obj.list_int_col.erase(0);
    });
    realm.write([] { });
    CHECK_EQUALS(change.deletions.size(), 1);
    CHECK_EQUALS(did_run, true);

    co_return;
}

TEST(notifications_modifications) {
    auto obj = AllTypesObject();

    auto realm = realm::open<AllTypesObject, AllTypesObjectLink, Dog>({.path=path});
    realm.write([&realm, &obj] {
        realm.add(obj);
        obj.list_int_col.push_back(123);
        obj.list_int_col.push_back(456);
    });

    bool did_run = false;

    CollectionChange<std::vector<int>> change;

    auto require_change = [&] {
        auto token = obj.list_int_col.observe([&](CollectionChange<std::vector<int>> c, std::exception_ptr) {
            did_run = true;
            change = std::move(c);
        });
        return token;
    };

    auto token = require_change();
    realm.write([&realm, &obj] {
        obj.list_int_col.set(1, 345);
    });
    realm.write([] { });

    CHECK_EQUALS(change.modifications.size(), 1);
    CHECK_EQUALS(change.modifications[0], 1);
    CHECK_EQUALS(did_run, true);

    co_return;
}

template<typename Col, typename Value>
void test_list(Col& list, std::vector<Value> values, auto& realm, auto& obj) {
    CHECK_EQUALS(obj.is_managed(), false);

    // unmanaged
    list.push_back(values[0]);
    list.push_back(values[1]);
    CHECK_EQUALS(list.size(), 2);

    list.pop_back();
    CHECK_EQUALS(list.size(), 1);
    list.erase(0);
    CHECK_EQUALS(list.size(), 0);
    list.clear();
    CHECK_EQUALS(list.size(), 0);
    list.push_back(values[0]);
    list.push_back(values[1]);
    CHECK_EQUALS(list.find(values[1]), 1);
    CHECK_EQUALS(list[1], values[1]);

    realm.write([&realm, &obj] {
        realm.add(obj);
    });

    // ensure values exist
    CHECK_EQUALS(obj.is_managed(), true);
    CHECK_EQUALS(list.size(), 2);

    realm.write([&] {
        list.push_back(values[0]);
    });
    CHECK_EQUALS(list.size(), 3);
    CHECK_EQUALS(list.find(values[1]), 1);
    CHECK_EQUALS(list[2], values[0]);

    realm.write([&list] {
        list.pop_back();
    });
    CHECK_EQUALS(list.size(), 2);

    realm.write([&list] {
        list.erase(0);
    });
    CHECK_EQUALS(list.size(), 1);

    realm.write([&list] {
        list.clear();
    });
    CHECK_EQUALS(list.size(), 0);
};

TEST(list_all_primitive_types) {
    auto realm = realm::open<AllTypesObject, AllTypesObjectLink, Dog>({.path=path});

    auto int_list_obj = AllTypesObject();
    test_list(int_list_obj.list_int_col, std::vector<int>({1, 2}), realm, int_list_obj);
    auto str_list_obj = AllTypesObject();
    test_list(str_list_obj.list_str_col, std::vector<std::string>({"foo", "bar"}), realm, str_list_obj);
    auto uuid_list_obj = AllTypesObject();
    test_list(uuid_list_obj.list_uuid_col, std::vector<realm::uuid>({realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120000"),
        realm::uuid("18de7916-7f84-11ec-a8a3-0242ac120002")}), realm, uuid_list_obj);
    auto binary_list_obj = AllTypesObject();
    test_list(binary_list_obj.list_binary_col, std::vector<std::vector<uint8_t>>({{0}, {1}}), realm, binary_list_obj);
    auto date_list_obj = AllTypesObject();
    auto date1 = std::chrono::system_clock::now();
    auto date2 = date1 + std::chrono::hours(24);
    test_list(date_list_obj.list_date_col, std::vector<std::chrono::time_point<std::chrono::system_clock>>({date1, date2}), realm, date_list_obj);
    co_return;
}
