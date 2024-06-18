#include "../main.hpp"
#include "test_objects.hpp"

#include <cpprealm/schedulers/default_scheduler.hpp>

class test_scheduler final : public realm::scheduler {
public:

    ~test_scheduler() = default;

    bool is_on_thread() const noexcept override {
        return m_id == std::this_thread::get_id();
    }
    bool is_same_as(const scheduler *other) const noexcept override {
        auto o = dynamic_cast<const test_scheduler *>(other);
        return (o && (o->m_id == m_id));
    }
    bool can_invoke() const noexcept override {
        return true;
    }

    void invoke(std::function<void()> &&fn) override {
        fn();
    }

private:
    std::thread::id m_id = std::this_thread::get_id();
};

TEST_CASE("user defined scheduler", "[scheduler]") {
    SECTION("set default factory", "[scheduler]") {
        auto scheduler = test_scheduler();
        realm::default_scheduler::set_default_factory([]() {
            return std::make_shared<test_scheduler>();
        });
        auto default_scheduler = realm::default_scheduler::make_default();
        CHECK(scheduler.is_same_as(default_scheduler.get()));
        // Set back to platform default so other tests are no affected.
        realm::default_scheduler::set_default_factory([]() {
            return realm::default_scheduler::make_platform_default();
        });
    }
}