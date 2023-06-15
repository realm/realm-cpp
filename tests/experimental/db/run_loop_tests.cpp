#include "../../main.hpp"
#include "test_objects.hpp"
#include <functional>
#include <mutex>

#if REALM_HAVE_UV
#include <uv.h>
#elif REALM_PLATFORM_APPLE
#include <realm/util/cf_ptr.hpp>
#include <CoreFoundation/CoreFoundation.h>
#else
#error "No EventLoop implementation selected, tests will fail"
#endif

class InvocationQueue {
public:
    void push(realm::Function<void()>&&);
    void invoke_all();

private:
    std::mutex m_mutex;
    std::vector<realm::Function<void()>> m_functions;
};

void InvocationQueue::push(realm::Function<void()>&& fn)
{
    std::lock_guard lock(m_mutex);
    m_functions.push_back(std::move(fn));
}

void InvocationQueue::invoke_all()
{
    std::vector<realm::Function<void()>> functions;
    {
        std::lock_guard lock(m_mutex);
        functions.swap(m_functions);
    }
    for (auto&& fn : functions) {
        fn();
    }
}

#if REALM_PLATFORM_APPLE

struct RefCountedInvocationQueue {
    InvocationQueue queue;
    std::atomic<size_t> ref_count = {0};
};

class RunLoopScheduler : public realm::scheduler {
public:
    RunLoopScheduler(CFRunLoopRef run_loop = nullptr);
    ~RunLoopScheduler();

    void invoke(realm::Function<void()>&&) override;

    bool is_on_thread() const noexcept override;
    bool is_same_as(const scheduler* other) const noexcept override;
    bool can_invoke() const noexcept override;

private:
    CFRunLoopRef m_runloop;
    CFRunLoopSourceRef m_notify_signal = nullptr;
    RefCountedInvocationQueue& m_queue;

    void release(CFRunLoopSourceRef&);
    void set_callback(CFRunLoopSourceRef&, std::function<void()>);
};

RunLoopScheduler::RunLoopScheduler(CFRunLoopRef run_loop)
    : m_runloop(run_loop ?: CFRunLoopGetCurrent())
      , m_queue(*new RefCountedInvocationQueue)
{
    CFRetain(m_runloop);

    CFRunLoopSourceContext ctx{};
    ctx.info = &m_queue;
    ctx.perform = [](void* info) {
        static_cast<RefCountedInvocationQueue*>(info)->queue.invoke_all();
    };
    ctx.retain = [](const void* info) {
        static_cast<RefCountedInvocationQueue*>(const_cast<void*>(info))
                ->ref_count.fetch_add(1, std::memory_order_relaxed);
        return info;
    };
    ctx.release = [](const void* info) {
        auto ptr = static_cast<RefCountedInvocationQueue*>(const_cast<void*>(info));
        if (ptr->ref_count.fetch_add(-1, std::memory_order_acq_rel) == 1) {
            delete ptr;
        }
    };

    m_notify_signal = CFRunLoopSourceCreate(kCFAllocatorDefault, 0, &ctx);
    CFRunLoopAddSource(m_runloop, m_notify_signal, kCFRunLoopDefaultMode);
}

RunLoopScheduler::~RunLoopScheduler()
{
    CFRunLoopSourceInvalidate(m_notify_signal);
    CFRelease(m_notify_signal);
    CFRelease(m_runloop);
}

void RunLoopScheduler::invoke(realm::Function<void()>&& fn)
{
    m_queue.queue.push(std::move(fn));

    CFRunLoopSourceSignal(m_notify_signal);
    // Signalling the source makes it run the next time the runloop gets
    // to it, but doesn't make the runloop start if it's currently idle
    // waiting for events
    CFRunLoopWakeUp(m_runloop);
}

bool RunLoopScheduler::is_on_thread() const noexcept
{
    return CFRunLoopGetCurrent() == m_runloop;
}

bool RunLoopScheduler::is_same_as(const scheduler* other) const noexcept
{
    auto o = dynamic_cast<const RunLoopScheduler*>(other);
    return (o && (o->m_runloop == m_runloop));
}

bool RunLoopScheduler::can_invoke() const noexcept
{
    if (pthread_main_np())
        return true;

    if (auto mode = CFRunLoopCopyCurrentMode(CFRunLoopGetCurrent())) {
        CFRelease(mode);
        return true;
    }
    return false;
}

#else
class UvScheduler final : public realm::scheduler {
public:
    UvScheduler(uv_loop_t* loop)
        : m_handle(std::make_unique<uv_async_t>()) {
        int err = uv_async_init(loop, m_handle.get(), [](uv_async_t *handle) {
            if (!handle->data) {
                return;
            }
            auto &data = *static_cast<Data *>(handle->data);
            if (data.close_requested) {
                uv_close(reinterpret_cast<uv_handle_t *>(handle), [](uv_handle_t *handle) {
                    delete reinterpret_cast<Data *>(handle->data);
                    delete reinterpret_cast<uv_async_t *>(handle);
                });
            } else {
                data.queue.invoke_all();
            }
        });
        if (err < 0) {
            throw std::runtime_error(realm::util::format("uv_async_init failed: %1", uv_strerror(err)));
        }
        m_handle->data = new Data;
    }

    ~UvScheduler() {
        if (m_handle && m_handle->data) {
            static_cast<Data *>(m_handle->data)->close_requested = true;
            uv_async_send(m_handle.get());
            // Don't delete anything here as we need to delete it from within the event loop instead
            m_handle.release();
        }
    }

    bool is_on_thread() const noexcept override {
        return m_id == std::this_thread::get_id();
    }
    bool is_same_as(const scheduler *other) const noexcept override {
        auto o = dynamic_cast<const UvScheduler *>(other);
        return (o && (o->m_id == m_id));
    }
    bool can_invoke() const noexcept override {
        return true;
    }

    void invoke(realm::Function<void()> &&fn) override {
        auto &data = *static_cast<Data *>(m_handle->data);
        data.queue.push(std::move(fn));
        uv_async_send(m_handle.get());
    }

private:
    struct Data {
        InvocationQueue queue;
        std::atomic<bool> close_requested = {false};
    };
    std::unique_ptr<uv_async_t> m_handle;
    std::thread::id m_id = std::this_thread::get_id();
};
#endif

#if REALM_PLATFORM_APPLE

void run_until(CFRunLoopRef loop, std::function<bool()> predicate)
{
    REALM_ASSERT(loop == CFRunLoopGetCurrent());

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

TEST_CASE("run loops", "[run loops]") {
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

                auto obj = realm::experimental::AllTypesObject();
                auto config = realm::db_config(path, std::make_shared<RunLoopScheduler>(loop));

                auto realm = realm::experimental::db(std::move(config));
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

                auto obj = realm::experimental::AllTypesObject();

                auto config = realm::db_config(path, std::make_shared<RunLoopScheduler>(loop));
                auto realm = realm::experimental::db(std::move(config));
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
                    auto c = realm::db_config(path, std::make_shared<RunLoopScheduler>(loop));

                    auto r = realm::experimental::db(std::move(c));
                    auto o = r.objects<realm::experimental::AllTypesObject>().where([](auto& obj) { return obj._id == 123; })[0];

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
        auto obj = realm::experimental::AllTypesObject();
        auto config = realm::db_config(path, std::make_shared<RunLoopScheduler>(loop));

        auto realm = realm::experimental::db(std::move(config));
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

#else

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

TEST_CASE("run loops", "[run loops]") {
    realm_path path;

    #ifndef _WIN32
    SECTION("uv threads detached linux", "[run loops]") {
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

                auto obj = realm::experimental::AllTypesObject();
                auto config = realm::db_config(path, std::make_shared<UvScheduler>(loop));

                auto realm = realm::experimental::db(std::move(config));
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

                uv_run(loop, UV_RUN_DEFAULT);
            }).detach();
        }

        {
            std::thread([&](){
                // Each thread needs a run loop
                auto loop = uv_loop_new();

                auto obj = realm::experimental::AllTypesObject();

                auto config = realm::db_config(path, std::make_shared<UvScheduler>(loop));
                auto realm = realm::experimental::db(std::move(config));

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
                    auto c = realm::db_config(path, std::make_shared<UvScheduler>(loop));

                    auto r = realm::experimental::db(std::move(c));
                    auto o = r.objects<realm::experimental::AllTypesObject>().where([](auto& obj) { return obj._id == 123; })[0];

                    r.write([&r, &o] {
                        o.str_col = "123";
                    });
                }

                uv_run(loop, UV_RUN_DEFAULT);
            }).detach();
        }

        std::unique_lock<std::mutex> lock(m1);
        v1.wait(lock, [&] { return signal1; });
        std::unique_lock<std::mutex> lock2(m2);
        v2.wait(lock2, [&] { return signal2; });
    }
    #endif
    SECTION("uv main thread", "[run loops]") {
        realm::notification_token t1;
        realm::notification_token t2;

        bool signal = false;

        auto loop = uv_loop_new();
        auto obj = realm::experimental::AllTypesObject();
        auto config = realm::db_config(path, std::make_shared<UvScheduler>(loop));

        auto realm = realm::experimental::db(std::move(config));
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
    }
}
#endif
