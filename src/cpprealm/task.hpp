////////////////////////////////////////////////////////////////////////////
//
// Copyright 2021 Realm Inc.
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

#ifndef realm_task_hpp
#define realm_task_hpp

// since C++ 20
#include <version>

#ifdef cpp_lib_coroutine
#include <coroutine>
using namespace std;
#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
using namespace std::experimental;
#else
#error "no coroutine support"
#endif

namespace realm {

template <typename T>
struct task {
    // The return type of a coroutine must contain a nested struct or type alias called `promise_type`
    struct promise_type {
        // Keep a coroutine handle referring to the parent coroutine if any. That is, if we
        // co_await a coroutine within another coroutine, this handle will be used to continue
        // working from where we left off.
        coroutine_handle<> precursor;

        // Place to hold the results produced by the coroutine
        T data;

        // Invoked when we first enter a coroutine. We initialize the precursor handle
        // with a resume point from where the task is ultimately suspended
        task get_return_object() noexcept
        {
            return {coroutine_handle<promise_type>::from_promise(*this)};
        }

        // When the caller enters the coroutine, we have the option to suspend immediately.
        // Let's choose not to do that here
        suspend_never initial_suspend() const noexcept { return {}; }

        // If an exception was thrown in the coroutine body, we would handle it here
        void unhandled_exception() {
            if (auto err = std::current_exception())
                std::rethrow_exception(err);
        }

        // The coroutine is about to complete (via co_return or reaching the end of the coroutine body).
        // The awaiter returned here defines what happens next
        auto final_suspend() const noexcept {
            struct awaiter {
                // Return false here to return control to the thread's event loop. Remember that we're
                // running on some async thread at this point.
                bool await_ready() const noexcept {
                    return false;
                }

                void await_resume() const noexcept {
                }

                // Returning a coroutine handle here resumes the coroutine it refers to (needed for
                // continuation handling). If we wanted, we could instead enqueue that coroutine handle
                // instead of immediately resuming it by enqueuing it and returning void.
                coroutine_handle<> await_suspend(coroutine_handle<promise_type> h) noexcept {
                    auto precursor = h.promise().precursor;
                    if (precursor) {
                        return precursor;
                    }
                    return noop_coroutine();
                }
            };
            return awaiter{};
        }
        // When the coroutine co_returns a value, this method is used to publish the result
        void return_value(T value) noexcept
        {
            data = std::move(value);
        }
    };

    // The following methods make our task type conform to the awaitable concept, so we can
    // co_await for a task to complete

    bool await_ready() const noexcept {
        // No need to suspend if this task has no outstanding work
        return handle.done();
    }

    T await_resume() const noexcept {
        // The returned value here is what `co_await our_task` evaluates to
        return std::move(handle.promise().data);
    }

    void await_suspend(coroutine_handle<> coroutine) const noexcept {
        // The coroutine itself is being suspended (async work can beget other async work)
        // Record the argument as the continuation point when this is resumed later. See
        // the final_suspend awaiter on the promise_type above for where this gets used
        handle.promise().precursor = coroutine;
    }

    // This handle is assigned to when the coroutine itself is suspended (see await_suspend above)
    coroutine_handle<promise_type> handle;
};

template <>
struct task<void> {
    // The return type of a coroutine must contain a nested struct or type alias called `promise_type`
    struct promise_type {
        // Keep a coroutine handle referring to the parent coroutine if any. That is, if we
        // co_await a coroutine within another coroutine, this handle will be used to continue
        // working from where we left off.
        coroutine_handle<> precursor;

        // Invoked when we first enter a coroutine. We initialize the precursor handle
        // with a resume point from where the task is ultimately suspended
        task get_return_object() noexcept
        {
            return {coroutine_handle<promise_type>::from_promise(*this)};
        }

        // When the caller enters the coroutine, we have the option to suspend immediately.
        // Let's choose not to do that here
        suspend_never initial_suspend() const noexcept {
            return {};
        }

        // If an exception was thrown in the coroutine body, we would handle it here
        void unhandled_exception() {
            if (auto err = std::current_exception())
                std::rethrow_exception(err);
        }

        // The coroutine is about to complete (via co_return or reaching the end of the coroutine body).
        // The awaiter returned here defines what happens next
        auto final_suspend() const noexcept {
            struct awaiter {
                // Return false here to return control to the thread's event loop. Remember that we're
                // running on some async thread at this point.
                bool await_ready() const noexcept {
                    return false;
                }

                void await_resume() const noexcept {
                }

                // Returning a coroutine handle here resumes the coroutine it refers to (needed for
                // continuation handling). If we wanted, we could instead enqueue that coroutine handle
                // instead of immediately resuming it by enqueuing it and returning void.
                coroutine_handle<> await_suspend(coroutine_handle<promise_type> h) noexcept {
                    auto precursor = h.promise().precursor;
                    if (precursor) {
                        return precursor;
                    }
                    return noop_coroutine();
                }
            };
            return awaiter{};
        }
        // When the coroutine co_returns a value, this method is used to publish the result
        void return_void() noexcept
        {
        }
    };

    // The following methods make our task type conform to the awaitable concept, so we can
    // co_await for a task to complete

    bool await_ready() const noexcept {
        // No need to suspend if this task has no outstanding work
        return handle.done();
    }

    void await_resume() const noexcept {
        // The returned value here is what `co_await our_task` evaluates to
    }

    void await_suspend(coroutine_handle<> coroutine) const noexcept {
        // The coroutine itself is being suspended (async work can beget other async work)
        // Record the argument as the continuation point when this is resumed later. See
        // the final_suspend awaiter on the promise_type above for where this gets used
        handle.promise().precursor = coroutine;
    }

    // This handle is assigned to when the coroutine itself is suspended (see await_suspend above)
    coroutine_handle<promise_type> handle;
};
#define FWD(x) static_cast<decltype(x)&&>(x)

inline void handle_result_args() {}

template <typename T>
inline T&& handle_result_args(T&& res) { return FWD(res); }

template <typename T>
inline T&& handle_result_args(T&& res, auto ec) {
    if (ec) throw std::system_error(ec->error_code);
    return FWD(res);
}
template <typename T>
inline T&& handle_result_args(T&& res, std::exception_ptr ec) {
    if (ec) throw ec;
    return FWD(res);
}
template <typename Res, typename F>
auto make_awaitable(F&& func) {
    struct Awaiter {
        Awaiter& operator=(Awaiter&&) = delete;
        bool await_ready() { return {}; }
        void await_suspend(coroutine_handle<> handle) {
            func([handle = std::move(handle), this] (auto&&... args) mutable
                 {
                try {
                    result.emplace(handle_result_args(FWD(args)...));
                } catch (...) {
                    error.emplace(std::current_exception());
                }
                handle.resume();
            });

        }
        Res await_resume() {
            if (result)
                return std::move(*result);
            std::rethrow_exception(*error);
        }
        std::optional<Res> result;
        std::optional<std::exception_ptr> error;
        F& func;
    };
    return Awaiter{.func = func};
}

}

#endif /* realm_task_hpp */
