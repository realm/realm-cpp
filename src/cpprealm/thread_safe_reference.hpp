////////////////////////////////////////////////////////////////////////////
//
// Copyright 2022 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef realm_thread_safe_reference_hpp
#define realm_thread_safe_reference_hpp

#include <future>
#include <queue>

#include <realm/object-store/util/scheduler.hpp>
#include <realm/object-store/shared_realm.hpp>
#include <realm/object-store/thread_safe_reference.hpp>
#include <iostream>

namespace realm {

template <typename ...>
struct db;

template <typename T, typename = void>
struct thread_safe_reference;

template <typename T>
struct thread_safe_reference<T, type_info::ObjectBasePersistable<T>> {
    thread_safe_reference(const T& object)
    : m_tsr(ThreadSafeReference(*object.m_object))
    {
    }
    thread_safe_reference(ThreadSafeReference&& tsr)
    : m_tsr(std::move(tsr))
    {
    }
    T resolve(std::shared_ptr<Realm> const&);
private:
    realm::ThreadSafeReference m_tsr;
    template <typename ...Ts>
    friend struct db;
};

struct looper {
    using task = std::packaged_task<void()>;
    std::queue<task> q;
    std::atomic<bool> is_running = true;
    bool is_moved = false;
    looper(const looper&) = delete;
    looper(looper&& l) noexcept
    : q(std::move(l.q))
    , t(std::move(l.t))
    {
        l.is_moved = true;
    }
    ~looper() {
        if (!is_moved) {
            is_running = false;
            t.join();
        }
    }
    looper() {
        t = std::thread([&]{
            while (is_running) {
                if (size()) {
                    front()();
                    pop();
                }
            }
        });
    }

    task& front() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return q.front();
    }
    size_t size()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return q.size();
    }

    task& push(task&& value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        q.push(std::move(value));
        return q.front();
    }
    task& push(std::function<void()>&& value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        q.push(std::packaged_task<void()>(std::move(value)));
        return q.front();
    }
    void pop()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        q.pop();
    }

    std::thread t;
    mutable std::mutex m_mutex;
};
    struct LooperScheduler: public util::Scheduler {
        static inline std::shared_ptr<util::Scheduler> make() {
            return std::make_shared<LooperScheduler>();
        };
        LooperScheduler() = default;
        LooperScheduler(const LooperScheduler&) = delete;
        LooperScheduler(LooperScheduler&& l)
        : l(std::move(l.l))
        {
        }
        looper l;
        void invoke(util::UniqueFunction<void()>&& f) override {
            l.push(std::packaged_task<void()>([f = std::move(f)] {
                f();
            }));
        }

        // Check if the caller is currently running on the scheduler's thread.
        //
        // This function can be called from any thread.
        bool is_on_thread() const noexcept override {
//            if (!m_thread) {
//                return false;
//            }
            return l.t.get_id() == std::this_thread::get_id();
        }

        // Checks if this scheduler instance wraps the same underlying instance.
        // This is up to the platforms to define, but if this method returns true,
        // caching may occur.
        bool is_same_as(const Scheduler* other) const noexcept override {
            return true;
        }

        // Check if this scehduler actually can support invoke(). Invoking may be
        // either not implemented, not applicable to a scheduler type, or simply not
        // be possible currently (e.g. if the associated event loop is not actually
        // running).
        //
        // This function is not thread-safe.
        bool can_invoke() const noexcept override { return true; }
    };

template <typename ...Ts>
struct thread_safe_reference<db<Ts...>> {
    thread_safe_reference(ThreadSafeReference tsr)
    : m_tsr(std::move(tsr))
    {
    }
    thread_safe_reference(db<Ts...> db)
    : m_tsr(ThreadSafeReference(db.m_realm))
    {
    }
    thread_safe_reference() = default;
    thread_safe_reference(const thread_safe_reference&) = default;
    thread_safe_reference(thread_safe_reference&&) = default;
    thread_safe_reference& operator=(const thread_safe_reference&) = default;
    thread_safe_reference& operator=(thread_safe_reference&&) = default;
    ~thread_safe_reference() = default;

    db<Ts...> resolve()
    {
        return db<Ts...>(Realm::get_shared_realm(std::move(m_tsr)));
//                                                 util::Scheduler::make_dispatch(dispatch_get_current_queue())));
    }

    explicit operator bool() const noexcept
    {
        return m_tsr.operator bool();
    }
private:
    ThreadSafeReference m_tsr;
    template <typename ...>
    friend struct db;
};

// TODO: Renable async_open with task.
#if __cpp_coroutines
    //template <typename ...Ts>
//static inline task<thread_safe_reference<db<Ts...>>> async_open(const db_config& config) {
//    // TODO: Add these flags to core
//#if QT_CORE_LIB
//    util::Scheduler::set_default_factory(util::make_qt);
//#endif
//    std::vector<ObjectSchema> schema;
//    (schema.push_back(Ts::schema::to_core_schema()), ...);
//
//    auto tsr = co_await make_awaitable<ThreadSafeReference>([config, schema](auto cb) {
//        std::shared_ptr<AsyncOpenTask> async_open_task = Realm::get_synchronized_realm({
//                                                                                               .path = config.path,
//                                                                                               .schema_mode = SchemaMode::AdditiveExplicit,
//                                                                                               .schema = Schema(schema),
//                                                                                               .schema_version = 0,
//                                                                                               .sync_config = config.sync_config
//                                                                                       });
//        async_open_task->start(cb);
//    });
//    co_return thread_safe_reference<db<Ts...>>(std::move(tsr));
//}
#else

#endif
}


#endif /* realm_thread_safe_reference.hpp */
