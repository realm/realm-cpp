#ifndef CPP_REALM_SCHEDULER_HPP
#define CPP_REALM_SCHEDULER_HPP

#include <functional>
#include <future>
#include <queue>

namespace realm {
    struct scheduler {
        static std::shared_ptr<scheduler> make_default();

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
#endif //CPP_REALM_SCHEDULER_HPP
