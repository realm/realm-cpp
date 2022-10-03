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
if (!harness::shared.assert_equals(a, b)) { \
std::cerr<<__FILE__<<" L"<<__LINE__<<": "<<#a<<" ("<<a<<")"<<" did not equal "<<#b<<std::endl;\
} \
static_cast<void>(0)
#define CHECK_THROWS(a) \
    auto did_throw = false;                    \
    try {               \
        a();            \
    } catch (...) {     \
        did_throw = true;                \
    }                   \
    if (did_throw) { harness::shared.success_count += 1; } \
    else { harness::shared.fail_count += 1; std::cerr<<__FILE__<<" L"<<__LINE__<<": "<<#a<<" did not throw "<<std::endl; }

using fun_t = void (*)(std::string);

std::vector<std::pair<std::string /* path */, fun_t>>& registered_functions();

void register_function(const std::pair<std::string /* path */, fun_t>& f);

#if __cpp_coroutines
#define TEST(fn) \
static test::task_base fn(std::string path = std::string(std::filesystem::current_path() / std::string(#fn)) + ".realm"); \
namespace { struct fn##0 { \
    fn##0() { register_function({std::string(std::filesystem::current_path() / std::string(#fn)) + ".realm", fn }); } } fn##1; } \
test::task_base fn(std::string path)
#else
template <typename Fn>
constexpr auto make_awaitable(Fn&& f) {
    f();
}
#define TEST(fn) \
static void fn(std::string path = std::string(std::filesystem::current_path() / std::string(#fn)) + ".realm"); \
namespace { struct fn##0 { \
    fn##0() { register_function({std::string(std::filesystem::current_path() / std::string(#fn)) + ".realm", fn }); } } fn##1; } \
void fn(std::string path)
#endif
#endif //REALM_TEST_UTILS_HPP
