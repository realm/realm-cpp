#include "observation.hpp"

// since C++ 20
#if __cpp_coroutines
#include <version>
#ifdef __cpp_lib_coroutine
#include <coroutine>
namespace coro = std;
#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
namespace coro = std::experimental;
#else
#error "no coroutine support"
#endif
#endif