#include "main.hpp"
#include "test_objects.hpp"

//#include <realm/object-store/util/event_loop_dispatcher.hpp>
//#include <realm/util/scope_exit.hpp>
//#include <realm/util/features.h>

using namespace realm;

namespace realm::util {

    struct EventLoop {
        // Returns if the current platform has an event loop implementation
        static bool has_implementation();

        // Returns the main event loop.
        static EventLoop& main();

        // Run the event loop until the given return predicate returns true
        void run_until(util::FunctionRef<bool()> predicate);

        // Schedule execution of the given function on the event loop.
        void perform(util::UniqueFunction<void()>);

        EventLoop(EventLoop&&) = default;
        EventLoop& operator=(EventLoop&&) = default;
        ~EventLoop();

    private:
        struct Impl;

        EventLoop(std::unique_ptr<Impl>);

        std::unique_ptr<Impl> m_impl;
    };

} // namespace realm::util



#include <mutex>
//#include <stdexcept>
//#include <vector>

#if REALM_USE_UV
#include <uv.h>
#elif REALM_PLATFORM_APPLE
#include <realm/util/cf_ptr.hpp>
#include <CoreFoundation/CoreFoundation.h>
#else
#error "No EventLoop implementation selected, tests will fail"
#endif

using namespace realm::util;

struct EventLoop::Impl {
    // Returns the main event loop.
    static std::unique_ptr<Impl> main();

    // Run the event loop until the given return predicate returns true
    void run_until(util::FunctionRef<bool()> predicate);

    // Schedule execution of the given function on the event loop.
    void perform(util::UniqueFunction<void()>);

    ~Impl();

private:
#if REALM_USE_UV
    Impl(uv_loop_t* loop);

    std::vector<util::UniqueFunction<void()>> m_pending_work;
    std::mutex m_mutex;
    uv_loop_t* m_loop;
    uv_async_t m_perform_work;
#elif REALM_PLATFORM_APPLE
    Impl(util::CFPtr<CFRunLoopRef> loop)
            : m_loop(std::move(loop))
    {
    }

    util::CFPtr<CFRunLoopRef> m_loop;
#endif
};

EventLoop& EventLoop::main()
{
    static EventLoop main(Impl::main());
    return main;
}

EventLoop::EventLoop(std::unique_ptr<Impl> impl)
        : m_impl(std::move(impl))
{
}

EventLoop::~EventLoop() = default;

void EventLoop::run_until(util::FunctionRef<bool()> predicate)
{
    return m_impl->run_until(predicate);
}

void EventLoop::perform(util::UniqueFunction<void()> function)
{
    return m_impl->perform(std::move(function));
}

#if REALM_USE_UV

bool EventLoop::has_implementation()
{
    return true;
}

std::unique_ptr<EventLoop::Impl> EventLoop::Impl::main()
{
    return std::unique_ptr<Impl>(new Impl(uv_default_loop()));
}

EventLoop::Impl::Impl(uv_loop_t* loop)
    : m_loop(loop)
{
    m_perform_work.data = this;
    uv_async_init(uv_default_loop(), &m_perform_work, [](uv_async_t* handle) {
        std::vector<util::UniqueFunction<void()>> pending_work;
        {
            Impl& self = *static_cast<Impl*>(handle->data);
            std::lock_guard<std::mutex> lock(self.m_mutex);
            std::swap(pending_work, self.m_pending_work);
        }

        for (auto& f : pending_work)
            f();
    });
}

EventLoop::Impl::~Impl()
{
    uv_close((uv_handle_t*)&m_perform_work, [](uv_handle_t*) {});
    uv_loop_close(m_loop);
}

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

void EventLoop::Impl::run_until(util::FunctionRef<bool()> predicate)
{
    if (predicate())
        return;

    IdleHandler observer(m_loop);
    observer.idle->data = &predicate;

    uv_idle_start(observer.idle, [](uv_idle_t* handle) {
        auto& predicate = *static_cast<util::FunctionRef<bool()>*>(handle->data);
        if (predicate()) {
            uv_stop(handle->loop);
        }
    });

    auto cleanup = make_scope_exit([&]() noexcept {
        uv_idle_stop(observer.idle);
    });
    uv_run(m_loop, UV_RUN_DEFAULT);
}

void EventLoop::Impl::perform(util::UniqueFunction<void()> f)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pending_work.push_back(std::move(f));
    }
    uv_async_send(&m_perform_work);
}

#elif REALM_PLATFORM_APPLE

bool EventLoop::has_implementation()
{
    return true;
}

std::unique_ptr<EventLoop::Impl> EventLoop::Impl::main()
{
    return std::unique_ptr<Impl>(new Impl(retainCF(CFRunLoopGetCurrent())));
}

EventLoop::Impl::~Impl() = default;

void EventLoop::Impl::run_until(util::FunctionRef<bool()> predicate)
{
    auto callback = [](CFRunLoopObserverRef, CFRunLoopActivity, void* info) {
        if ((*static_cast<util::FunctionRef<bool()>*>(info))()) {
            CFRunLoopStop(CFRunLoopGetCurrent());
        }
    };
    CFRunLoopObserverContext ctx{};
    ctx.info = &predicate;
    auto observer =
            adoptCF(CFRunLoopObserverCreate(kCFAllocatorDefault, kCFRunLoopAllActivities, true, 0, callback, &ctx));
    auto timer = adoptCF(CFRunLoopTimerCreateWithHandler(
            kCFAllocatorDefault, CFAbsoluteTimeGetCurrent(), 0.0005, 0, 0,
            ^(CFRunLoopTimerRef){
                // Do nothing. The timer firing is sufficient to cause our runloop observer to run.
            }));
    CFRunLoopAddObserver(CFRunLoopGetCurrent(), observer.get(), kCFRunLoopCommonModes);
    CFRunLoopAddTimer(CFRunLoopGetCurrent(), timer.get(), kCFRunLoopCommonModes);
    auto cleanup = make_scope_exit([&]() noexcept {
        CFRunLoopRemoveTimer(CFRunLoopGetCurrent(), timer.get(), kCFRunLoopCommonModes);
        CFRunLoopRemoveObserver(CFRunLoopGetCurrent(), observer.get(), kCFRunLoopCommonModes);
    });
    CFRunLoopRun();
}

void EventLoop::Impl::perform(util::UniqueFunction<void()> func)
{
    __block auto f = std::move(func);
    CFRunLoopPerformBlock(m_loop.get(), kCFRunLoopDefaultMode, ^{
        f();
    });
    CFRunLoopWakeUp(m_loop.get());
}

#else

bool EventLoop::has_implementation()
{
    return false;
}
std::unique_ptr<EventLoop::Impl> EventLoop::Impl::main()
{
    return nullptr;
}
EventLoop::Impl::~Impl() = default;
void EventLoop::Impl::run_until(util::FunctionRef<bool()>)
{
    printf("WARNING: there is no event loop implementation and nothing is happening.\n");
}
void EventLoop::Impl::perform(util::UniqueFunction<void()>)
{
    printf("WARNING: there is no event loop implementation and nothing is happening.\n");
}

#endif


TEST_CASE("threads", "[threads]") {
    realm_path path;

    SECTION("threads", "[thread]") {
        notification_token t1;
        notification_token t2;

        bool signal1;
        std::mutex m1;
        std::condition_variable v1;

        bool signal2;
        std::mutex m2;
        std::condition_variable v2;

        {
            std::thread([&](){
                auto obj = AllTypesObject();
                auto realm = open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({.path=path});
                realm.write([&realm, &obj] {
                    realm.add(obj);
                });
                auto token = obj.observe<AllTypesObject>([&](auto change) {
                    std::unique_lock<std::mutex> lock(m1);
                    signal1 = true;
                    v1.notify_one();
                });

                t1 = std::move(token);

                realm.write([&realm, &obj] {
                    obj.str_col = "456";
                });
                realm.refresh();

                EventLoop::main().run_until([&](){
                    return signal1;
                });
            }).join();
        }

        {
            std::thread([&](){
                auto obj = AllTypesObject();
                auto realm = open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({.path=path});

                {
                    obj._id = 123;
                    realm.write([&realm, &obj] {
                        realm.add(obj);
                    });

                    auto token = obj.observe<AllTypesObject>([&](auto change) {
                        std::unique_lock<std::mutex> lock(m2);
                        signal2 = true;
                        v2.notify_one();
                    });

                    t2 = std::move(token);
                }

                {
                    auto r = open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({.path=path});
                    auto o = r.object<AllTypesObject>(123);

                    r.write([&r, &o] {
                        o.str_col = "123";
                    });
                    r.refresh();
                }

                EventLoop::main().run_until([&](){
                    return signal2;
                });
            }).join();
        }

        std::unique_lock<std::mutex> lock(m1);
        v1.wait(lock, [&] { return signal1; });

        std::unique_lock<std::mutex> lock2(m2);
        v2.wait(lock, [&] { return signal2; });
    }
}
