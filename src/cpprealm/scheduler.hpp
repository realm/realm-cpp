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

#ifndef CPPREALM_SCHEDULER_HPP
#define CPPREALM_SCHEDULER_HPP

#include <functional>
#include <future>
#include <queue>

namespace realm {
    namespace util {
        class Scheduler;
    }

    // A thread-safe queue of functions to invoke, used in the implemenation of
    // some of the schedulers
    class invocation_queue {
    public:
        void push(std::function<void()>&&);
        void invoke_all();

    private:
        std::mutex m_mutex;
        std::vector<std::function<void()>> m_functions;
    };

    struct scheduler {
        static std::shared_ptr<scheduler> make_platform_default();
        static std::shared_ptr<scheduler> make_dummy_scheduler();
//#ifdef CPPREALM_USE_UV
//        static std::shared_ptr<scheduler> make_UV_scheduler(uv_loop_t* loop);
//#endif
//#if __APPLE__
//        static std::shared_ptr<scheduler> make_CFRunLoop_scheduler(CFRunLoopRef run_loop);
//#endif
//#if QT_CORE_LIB
//        static std::shared_ptr<scheduler> make_Qt_scheduler();
//#endif
//#if __ANDROID__
//        static std::shared_ptr<scheduler> make_Android_ALooper_scheduler();
//#endif

        virtual ~scheduler() = default;

        // Invoke the given function on the scheduler's thread.
        //
        // This function can be called from any thread.
        virtual void invoke(std::function<void()> &&) = 0;

        // Check if the caller is currently running on the scheduler's thread.
        //
        // This function can be called from any thread.
        [[nodiscard]] virtual bool is_on_thread() const noexcept = 0;

        // Checks if this scheduler instance wraps the same underlying instance.
        // This is up to the platforms to define, but if this method returns true,
        // caching may occur.
        virtual bool is_same_as(const scheduler *other) const noexcept = 0;

        // Check if this scheduler actually can support invoke(). Invoking may be
        // either not implemented, not applicable to a scheduler type, or simply not
        // be possible currently (e.g. if the associated event loop is not actually
        // running).
        //
        // This function is not thread-safe.
        [[nodiscard]] virtual bool can_invoke() const noexcept = 0;
    };
}
#endif //CPPREALM_SCHEDULER_HPP
