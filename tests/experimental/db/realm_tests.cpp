#include "test_objects.hpp"
#include "../../main.hpp"
#include <realm/object-store/shared_realm.hpp>

namespace realm::experimental {

    TEST_CASE("cached realm") {
        SECTION("cached realm transactions") {
            realm_path path;
            realm::db_config config1;
            config1.set_path(path);
            auto o = managed<AllTypesObject>();
            {
                auto realm = db(std::move(config1));
                o = realm.write([&] {
                    return realm.add(AllTypesObject());
                });
            }

            realm::db_config config2;
            config2.set_path(path);
            {
                auto realm = db(std::move(config2));
                // If the cached realm is not returned this write would fail with a wrong transaction state.
                realm.write([&] {
                    o.str_col = "foo";
                });
            }
            CHECK(o.str_col == "foo");
        }
    }

    TEST_CASE("tsr_object") {
        realm_path path;
        realm::db_config config;
        config.set_path(path);
        auto realm = db(std::move(config));

        AllTypesObject obj;
        obj.str_col = "John";
        obj._id = 1;
        AllTypesObjectLink obj_link;
        obj_link.str_col = "Fido";
        obj.opt_obj_col = &obj_link;

        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });

        auto tsr = thread_safe_reference<AllTypesObject>(managed_obj);
        std::promise<void> p;
        auto t = std::thread([&tsr, &p, &path]() {
            realm::db_config config2;
            config2.set_path(path);
            auto realm = db(std::move(config2));
            auto o = realm.resolve(std::move(tsr));
            CHECK(o._id == 1);
            realm.write([&] { realm.remove(o); });
            p.set_value();
        });
        t.join();
        p.get_future().get();
    }
}