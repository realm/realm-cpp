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

    namespace {
        struct looper {
            using task = std::packaged_task<void()>;
            std::queue<task> q;
            std::atomic<bool> is_running = true;
            bool is_moved = false;

            looper(const looper &) = delete;

            looper(looper &&l) noexcept
                    : q(std::move(l.q)), t(std::move(l.t)) {
                l.is_moved = true;
            }

            ~looper() {
                if (!is_moved) {
                    is_running = false;
                    t.join();
                }
            }

            looper() {
                t = std::thread([&] {
                    while (is_running) {
                        if (size()) {
                            front()();
                            pop();
                        }
                    }
                });
            }

            task &front() {
                std::lock_guard<std::mutex> lock(m_mutex);
                return q.front();
            }

            size_t size() {
                std::lock_guard<std::mutex> lock(m_mutex);
                return q.size();
            }

            task &push(task &&value) {
                std::lock_guard<std::mutex> lock(m_mutex);
                q.push(std::move(value));
                return q.front();
            }

            task &push(std::function<void()> &&value) {
                std::lock_guard<std::mutex> lock(m_mutex);
                q.push(std::packaged_task<void()>(std::move(value)));
                return q.front();
            }

            void pop() {
                std::lock_guard<std::mutex> lock(m_mutex);
                q.pop();
            }

            std::thread t;
            mutable std::mutex m_mutex;
        };
    }

    struct NoPlatformScheduler : public scheduler {
        static inline std::shared_ptr<scheduler> make() {
            return std::make_shared<NoPlatformScheduler>();
        };

        NoPlatformScheduler() = default;

        NoPlatformScheduler(const NoPlatformScheduler &) = delete;

        NoPlatformScheduler(NoPlatformScheduler &&l) noexcept
                : l(std::move(l.l)) {
        }

        looper l;

        void invoke(std::function<void()> &&f) override {
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
        bool is_same_as(const scheduler *other) const noexcept override {
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
}
#endif //CPP_REALM_SCHEDULER_HPP
