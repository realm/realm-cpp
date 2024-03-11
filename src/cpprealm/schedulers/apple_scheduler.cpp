#include <cpprealm/schedulers/apple_scheduler.hpp>

#include <realm/object-store/util/scheduler.hpp>

namespace realm {

    struct CFRunLoop_scheduler_wrapper final : public scheduler {
        explicit CFRunLoop_scheduler_wrapper(const std::shared_ptr<util::Scheduler>& s) : m_scheduler(s) { };
        CFRunLoop_scheduler_wrapper() = delete;
        ~CFRunLoop_scheduler_wrapper() final = default;
        void invoke(std::function<void()> &&fn) override {
            m_scheduler->invoke([f = std::move(fn)]() {
                f();
            });
        }
        [[nodiscard]] bool is_on_thread() const noexcept override {
            return m_scheduler->is_on_thread();
        }
        bool is_same_as(const realm::scheduler *other) const noexcept override {
            if (auto o = dynamic_cast<const CFRunLoop_scheduler_wrapper *>(other)) {
                return m_scheduler->is_same_as(o->m_scheduler.get());
            }
            return false;
        }
        [[nodiscard]] bool can_invoke() const noexcept override {
            return m_scheduler->can_invoke();
        }
        std::shared_ptr<util::Scheduler> m_scheduler;
    };

    std::shared_ptr<scheduler> make_apple_scheduler(CFRunLoopRef run_loop) {
        return std::make_shared<CFRunLoop_scheduler_wrapper>(CFRunLoop_scheduler_wrapper(util::Scheduler::make_runloop(run_loop)));
    }
} // namespace realm