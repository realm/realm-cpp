#if REALM_HAVE_UV

#include "../main.hpp"
#include "test_objects.hpp"

#include <cpprealm/schedulers/default_scheduler.hpp>
#include <uv.h>

struct idle_handler {
    uv_idle_t* idle = new uv_idle_t;

    idle_handler(uv_loop_t* loop)
    {
        uv_idle_init(loop, idle);
    }
    ~idle_handler()
    {
        uv_close(reinterpret_cast<uv_handle_t*>(idle), [](uv_handle_t* handle) {
            delete reinterpret_cast<uv_idle_t*>(handle);
        });
    }
};

inline void run_until(uv_loop_t *loop, std::function<bool()> predicate) {
    if (predicate())
        return;

    idle_handler observer(loop);
    observer.idle->data = &predicate;

    uv_idle_start(observer.idle, [](uv_idle_t* handle) {
        auto& predicate = *static_cast<std::function<bool()>*>(handle->data);
        if (predicate()) {
            uv_stop(handle->loop);
        }
    });

    uv_run(loop, UV_RUN_DEFAULT);
    uv_idle_stop(observer.idle);
}

TEST_CASE("user defined uv_loop", "[scheduler]") {
    realm_path path;

    SECTION("threads detached", "[run loops]") {
        realm::notification_token t1;
        realm::notification_token t2;

        bool signal1;
        std::mutex m1;
        std::condition_variable v1;

        bool signal2;
        std::mutex m2;
        std::condition_variable v2;

        {
            std::thread([&](){
                // Each thread needs a run loop
                auto loop = uv_loop_new();

                auto obj = realm::AllTypesObject();
                auto config = realm::db_config(path, realm::default_scheduler::make_uv(loop));

                auto realm = realm::db(std::move(config));
                auto managed_obj = realm.write([&realm, &obj] {
                    return realm.add(std::move(obj));
                });
                auto token = managed_obj.observe([&](auto change) {
                    std::unique_lock<std::mutex> lock(m1);
                    signal1 = true;
                    v1.notify_one();
                });

                t1 = std::move(token);

                realm.write([&realm, &managed_obj] {
                    managed_obj.str_col = "456";
                });

                run_until(loop, [&]() {
                    return signal1;
                });
                uv_loop_close(loop);
                free(loop);
            }).detach();
        }

        {
            std::thread([&](){
                // Each thread needs a run loop
                auto loop = uv_loop_new();

                auto obj = realm::AllTypesObject();

                auto config = realm::db_config(path, realm::default_scheduler::make_uv(loop));
                auto realm = realm::db(std::move(config));

                {
                    obj._id = 123;
                    auto managed_obj = realm.write([&realm, &obj] {
                        return realm.add(std::move(obj));
                    });

                    auto token = managed_obj.observe([&](auto change) {
                        std::unique_lock<std::mutex> lock(m2);
                        signal2 = true;
                        v2.notify_one();
                    });

                    t2 = std::move(token);
                }

                {
                    auto c = realm::db_config(path, realm::default_scheduler::make_uv(loop));

                    auto r = realm::db(std::move(c));
                    auto o = r.objects<realm::AllTypesObject>().where([](auto& obj) { return obj._id == 123; })[0];

                    r.write([&r, &o] {
                        o.str_col = "123";
                    });
                }

                run_until(loop, [&]() {
                    return signal2;
                });
                uv_loop_close(loop);
                free(loop);
            }).detach();
        }

        std::unique_lock<std::mutex> lock(m1);
        v1.wait(lock, [&] { return signal1; });
        std::unique_lock<std::mutex> lock2(m2);
        v2.wait(lock2, [&] { return signal2; });
    }

    SECTION("main thread", "[scheduler]") {
        realm::notification_token t1;
        realm::notification_token t2;

        bool signal = false;

        auto loop = uv_loop_new();
        auto scheduler = realm::default_scheduler::make_uv(loop);
        auto obj = realm::AllTypesObject();
        auto config = realm::db_config();
        config.set_path(path);
        config.set_scheduler(scheduler);
        auto realm = realm::db(config);

        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        auto token = managed_obj.observe([&](auto change) {
            signal = true;
        });

        t1 = std::move(token);

        realm.write([&realm, &managed_obj] {
            managed_obj.str_col = "456";
        });

        run_until(loop, [&]() {
            return signal;
        });

        t1.unregister();
        uv_loop_close(loop);
        free(loop);
    }

    SECTION("set default factory", "[scheduler]") {
        auto loop = uv_loop_new();
        auto scheduler = realm::default_scheduler::make_uv(loop);

        realm::default_scheduler::set_default_factory([&]() { return scheduler; });
        auto default_scheduler = realm::default_scheduler::make_default();
        CHECK(scheduler->is_same_as(default_scheduler.get()));

        uv_loop_close(loop);
        free(loop);
    }
}

#endif