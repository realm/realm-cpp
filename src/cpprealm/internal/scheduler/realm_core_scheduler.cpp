#include <cpprealm/internal/scheduler/realm_core_scheduler.hpp>

#include <realm/object-store/util/scheduler.hpp>

namespace realm::internal {
    void realm_core_scheduler::invoke(std::function<void()> &&fn) {
        s->invoke(std::move(fn));
    }

    [[nodiscard]] bool realm_core_scheduler::is_on_thread() const noexcept {
        return s->is_on_thread();
    }

    bool realm_core_scheduler::is_same_as(const scheduler *other) const noexcept {
        if (auto o = dynamic_cast<const realm_core_scheduler *>(other)) {
            return o->s->is_same_as(this->s.get());
            ;
        }
        return false;
    }

    [[nodiscard]] bool realm_core_scheduler::can_invoke() const noexcept {
        return s->can_invoke();
    }

    realm_core_scheduler::operator std::shared_ptr<util::Scheduler>() {
        return s;
    }

    std::shared_ptr<util::Scheduler> create_scheduler_shim(const std::shared_ptr<scheduler>& s) {
        struct internal_scheduler : util::Scheduler {
            internal_scheduler(const std::shared_ptr<scheduler>& s)
                : m_scheduler(s)
            {
            }

            ~internal_scheduler() override = default;
            void invoke(util::UniqueFunction<void()> &&fn) override {
                m_scheduler->invoke([fn = std::move(fn.release())]() {
                    auto f = util::UniqueFunction<void()>(std::move(fn));
                    f();
                });
            }

            bool is_on_thread() const noexcept override {
                return m_scheduler->is_on_thread();
            }
            bool is_same_as(const util::Scheduler *other) const noexcept override {
                if (auto o = dynamic_cast<const internal_scheduler *>(other)) {
                    return m_scheduler->is_same_as(o->m_scheduler.get());
                }
                return false;
            }

            bool can_invoke() const noexcept override {
                return m_scheduler->can_invoke();
            }
        private:
            std::shared_ptr<scheduler> m_scheduler;
        };

        return std::make_shared<internal_scheduler>(s);
    }
}