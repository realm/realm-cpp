#include "async_open_task.hpp"
#include <cpprealm/internal/bridge/thread_safe_reference.hpp>

#include <realm/object-store/sync/async_open_task.hpp>
#include <realm/object-store/thread_safe_reference.hpp>

namespace realm::internal::bridge {
    async_open_task::async_open_task(std::shared_ptr<AsyncOpenTask> &&v) {
        m_task = std::move(v);
    }

    void async_open_task::start(std::function<void(thread_safe_reference, std::exception_ptr)> &&fn) const {
        m_task->start([fn = std::move(fn)](ThreadSafeReference tsr, std::exception_ptr ptr) -> void {
            fn(thread_safe_reference(std::move(tsr)), ptr);
        });
    }
}
