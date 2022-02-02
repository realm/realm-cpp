#ifndef REALM_TEST_UTILS_HPP
#define REALM_TEST_UTILS_HPP

#include <cpprealm/sdk.hpp>

struct harness {
    int success_count = 0;
    int fail_count = 0;

    template<typename T, typename V>
    bool assert_equals(const T &a, const V &b) {
        if (a == b) { success_count += 1; }
        else { fail_count += 1; }
        return a == b;
    }

    static harness shared;
};

#define CHECK(a) \
if (!a) {        \
harness::shared.fail_count += 1;                 \
std::cerr<<__FILE__<<" L"<<__LINE__<<": "<<#a<<" ("<<a<<")"<<" was not true"<<std::endl;\
} else { \
harness::shared.success_count += 1; \
}


#define CHECK_EQUALS(a, b) \
if (!harness::shared.assert_equals(a, b)) {\
std::cerr<<__FILE__<<" L"<<__LINE__<<": "<<#a<<" ("<<a<<")"<<" did not equal "<<#b<<std::endl;\
}

#define CHECK_THROWS(a) \
    auto did_throw = false;                    \
    try {               \
        a();            \
    } catch (...) {     \
        did_throw = true;                \
    }                   \
    if (did_throw) { harness::shared.success_count += 1; } \
    else { harness::shared.fail_count += 1; std::cerr<<__FILE__<<" L"<<__LINE__<<": "<<#a<<" did not throw "<<std::endl; }

namespace test {
    struct task_base {
        struct promise_type {
            coroutine_handle<> precursor;

            task_base get_return_object() noexcept {
                return task_base{coroutine_handle<promise_type>::from_promise(*this)};
            }

            suspend_never initial_suspend() const noexcept { return {}; }

            void unhandled_exception() {
                if (auto err = std::current_exception())
                    std::rethrow_exception(err);
            }

            auto final_suspend() const noexcept {
                struct awaiter {
                    bool await_ready() const noexcept {
                        return false;
                    }

                    void await_resume() const noexcept {
                    }
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

            void return_void() noexcept {
            }
        };
        coroutine_handle<promise_type> handle;
        std::string path;
    };
    template <realm::StringLiteral TestName>
    struct task : task_base {
        bool await_ready() const noexcept {
            return handle.done();
        }

        void await_resume() const noexcept {
            handle.promise();
        }

        void await_suspend(coroutine_handle<> coroutine) const noexcept {
            handle.promise().precursor = coroutine;
        }

        task() : task_base(TestName) {}
        task(task_base&& t) : task_base({t.handle, TestName}) {}
    };
}

using fun_t = test::task_base (*)(std::string);

std::vector<std::pair<std::string /* path */, fun_t>>& registered_functions();

void register_function(std::pair<std::string /* path */, fun_t> f);

#define TEST(fn) \
static test::task_base fn(std::string path = std::string(std::filesystem::current_path() / std::string(#fn)) + ".realm"); \
namespace { struct fn##0 { \
    fn##0() { register_function({std::string(std::filesystem::current_path() / std::string(#fn)) + ".realm", fn }); } } fn##1; } \
test::task_base fn(std::string path)

#endif //REALM_TEST_UTILS_HPP
