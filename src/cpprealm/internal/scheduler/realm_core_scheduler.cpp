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
}