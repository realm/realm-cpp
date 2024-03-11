#include "../main.hpp"
#include "test_objects.hpp"
#include <functional>
#include <mutex>

#if __has_include(<uv.h>)
#include <cpprealm/schedulers/uv_scheduler.hpp>
#include <uv.h>
#define USE_UV
#endif

#if __APPLE__
#include <cpprealm/schedulers/apple_scheduler.hpp>

void run_until(CFRunLoopRef loop, std::function<bool()> predicate)
{
    auto callback = [](CFRunLoopObserverRef, CFRunLoopActivity, void* info) {
        if ((*static_cast<std::function<bool()>*>(info))()) {
            CFRunLoopStop(CFRunLoopGetCurrent());
        }
    };
    CFRunLoopObserverContext ctx{};
    ctx.info = &predicate;
    CFRunLoopObserverRef observer = CFRunLoopObserverCreate(kCFAllocatorDefault, kCFRunLoopAllActivities, true, 0, callback, &ctx);
    CFRunLoopTimerRef timer = CFRunLoopTimerCreateWithHandler(
            kCFAllocatorDefault, CFAbsoluteTimeGetCurrent(), 0.0005, 0, 0,
            ^(CFRunLoopTimerRef){
                    // Do nothing. The timer firing is sufficient to cause our runloop observer to run.
            });
    CFRunLoopAddObserver(CFRunLoopGetCurrent(), observer, kCFRunLoopCommonModes);
    CFRunLoopAddTimer(CFRunLoopGetCurrent(), timer, kCFRunLoopCommonModes);
    CFRunLoopRun();
    CFRunLoopRemoveTimer(CFRunLoopGetCurrent(), timer, kCFRunLoopCommonModes);
    CFRunLoopRemoveObserver(CFRunLoopGetCurrent(), observer, kCFRunLoopCommonModes);
    CFRelease(observer);
    CFRelease(timer);
}

TEST_CASE("CFRunLoop", "[run loops]") {
    realm_path path;

    SECTION("CFRunLoop threads", "[run loops]") {
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
                auto loop = CFRunLoopGetCurrent();

                auto obj = realm::AllTypesObject();
                auto config = realm::db_config(path, realm::make_apple_scheduler(loop));

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

                CFRunLoopRun();
            }).detach();
        }

        {
            std::thread([&](){
                // Each thread needs a run loop
                auto loop = CFRunLoopGetCurrent();

                auto obj = realm::AllTypesObject();

                auto config = realm::db_config(path, realm::make_apple_scheduler(loop));
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
                    auto c = realm::db_config(path, realm::make_apple_scheduler(loop));

                    auto r = realm::db(std::move(c));
                    auto o = r.objects<realm::AllTypesObject>().where([](auto& obj) { return obj._id == 123; })[0];

                    r.write([&r, &o] {
                        o.str_col = "123";
                    });
                }

                CFRunLoopRun();
            }).detach();
        }

        std::unique_lock<std::mutex> lock(m1);
        v1.wait(lock, [&] { return signal1; });
        std::unique_lock<std::mutex> lock2(m2);
        v2.wait(lock2, [&] { return signal2; });
    }

    SECTION("CFRunLoop main thread", "[run loops]") {
        realm::notification_token token;

        bool signal = false;

        auto loop = CFRunLoopGetCurrent();
        auto obj = realm::AllTypesObject();
        auto config = realm::db_config(path, realm::make_apple_scheduler(loop));

        auto realm = realm::db(std::move(config));
        auto managed_obj = realm.write([&realm, &obj] {
            return realm.add(std::move(obj));
        });
        token = managed_obj.observe([&](auto change) {
            signal = true;
        });

        realm.write([&realm, &managed_obj] {
            managed_obj.str_col = "456";
        });

        run_until(loop, [&]() {
            return signal;
        });
    }
}

#endif

#ifdef USE_UV

struct IdleHandler {
    uv_idle_t* idle = new uv_idle_t;

    IdleHandler(uv_loop_t* loop)
    {
        uv_idle_init(loop, idle);
    }
    ~IdleHandler()
    {
        uv_close(reinterpret_cast<uv_handle_t*>(idle), [](uv_handle_t* handle) {
            delete reinterpret_cast<uv_idle_t*>(handle);
        });
    }
};

inline void run_until(uv_loop_t *loop, std::function<bool()> predicate) {
    if (predicate())
        return;

    IdleHandler observer(loop);
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

TEST_CASE("UV run loop", "[run loops]") {
    realm_path path;

//    #ifndef _WIN32
//    SECTION("uv threads detached linux", "[run loops]") {
//        realm::notification_token t1;
//        realm::notification_token t2;
//
//        bool signal1;
//        std::mutex m1;
//        std::condition_variable v1;
//
//        bool signal2;
//        std::mutex m2;
//        std::condition_variable v2;
//
//        {
//            std::thread([&](){
//                // Each thread needs a run loop
//                auto loop = uv_loop_new();
//
//                auto obj = realm::AllTypesObject();
//                auto config = realm::db_config(path, std::make_shared<UvScheduler>(loop));
//
//                auto realm = realm::db(std::move(config));
//                auto managed_obj = realm.write([&realm, &obj] {
//                    return realm.add(std::move(obj));
//                });
//                auto token = managed_obj.observe([&](auto change) {
//                    std::unique_lock<std::mutex> lock(m1);
//                    signal1 = true;
//                    v1.notify_one();
//                });
//
//                t1 = std::move(token);
//
//                realm.write([&realm, &managed_obj] {
//                    managed_obj.str_col = "456";
//                });
//
//                uv_run(loop, UV_RUN_DEFAULT);
//            }).detach();
//        }
//
//        {
//            std::thread([&](){
//                // Each thread needs a run loop
//                auto loop = uv_loop_new();
//
//                auto obj = realm::AllTypesObject();
//
//                auto config = realm::db_config(path, std::make_shared<UvScheduler>(loop));
//                auto realm = realm::db(std::move(config));
//
//                {
//                    obj._id = 123;
//                    auto managed_obj = realm.write([&realm, &obj] {
//                        return realm.add(std::move(obj));
//                    });
//
//                    auto token = managed_obj.observe([&](auto change) {
//                        std::unique_lock<std::mutex> lock(m2);
//                        signal2 = true;
//                        v2.notify_one();
//                    });
//
//                    t2 = std::move(token);
//                }
//
//                {
//                    auto c = realm::db_config(path, std::make_shared<UvScheduler>(loop));
//
//                    auto r = realm::db(std::move(c));
//                    auto o = r.objects<realm::AllTypesObject>().where([](auto& obj) { return obj._id == 123; })[0];
//
//                    r.write([&r, &o] {
//                        o.str_col = "123";
//                    });
//                }
//
//                uv_run(loop, UV_RUN_DEFAULT);
//            }).detach();
//        }
//
//        std::unique_lock<std::mutex> lock(m1);
//        v1.wait(lock, [&] { return signal1; });
//        std::unique_lock<std::mutex> lock2(m2);
//        v2.wait(lock2, [&] { return signal2; });
//    }
//    #endif
    SECTION("uv main thread", "[run loops]") {
        realm::notification_token t1;
        realm::notification_token t2;
        bool signal = false;

        auto loop = uv_loop_new();
        auto obj = realm::AllTypesObject();
        auto config = realm::db_config(path, realm::make_uv_scheduler(loop));

        auto realm = realm::db(std::move(config));
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

        CHECK(signal);
    }
}
#endif
