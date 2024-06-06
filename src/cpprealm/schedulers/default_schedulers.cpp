#include <cpprealm/schedulers/default_schedulers.hpp>
#include <cpprealm/internal/scheduler/realm_core_scheduler.hpp>

#include <realm/object-store/util/scheduler.hpp>

#if defined(REALM_HAVE_UV) && REALM_HAVE_UV
#include <uv.h>
#include <realm/object-store/util/uv/scheduler.hpp>
#endif

namespace realm::default_schedulers {

    std::shared_ptr<scheduler> (*s_factory)() = make_platform_default;

    std::shared_ptr<scheduler> make_platform_default() {
#if REALM_PLATFORM_APPLE || REALM_ANDROID && !defined(REALM_AOSP_VENDOR)
        return std::make_shared<internal::realm_core_scheduler>(util::Scheduler::make_platform_default());
#elif defined(REALM_HAVE_UV) && REALM_HAVE_UV
        return make_uv(uv_default_loop());
#else
        return std::make_shared<realm_core_scheduler>(util::Scheduler::make_generic());
#endif
    }

#if defined(REALM_HAVE_UV) && REALM_HAVE_UV
    std::shared_ptr<scheduler> make_uv() {
        auto uv_scheduler = std::make_shared<util::UvMainLoopScheduler>();
        return std::make_shared<internal::realm_core_scheduler>(std::move(uv_scheduler));
    }
#endif

    void set_default_factory(std::shared_ptr<scheduler> (*factory)()) {
        s_factory = std::move(factory);
    }

    /** Create a new instance of the scheduler type returned by the default
    * scheduler factory. By default, the factory function is
    * `Scheduler::make_platform_default()`.
    */
    std::shared_ptr<scheduler> make_default()
    {
        return s_factory();
    }
}