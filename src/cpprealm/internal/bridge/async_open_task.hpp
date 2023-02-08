#ifndef CPP_REALM_BRIDGE_ASYNC_OPEN_TASK_HPP
#define CPP_REALM_BRIDGE_ASYNC_OPEN_TASK_HPP

#include <functional>
#include <memory>

namespace realm {
    class AsyncOpenTask;

    namespace internal::bridge {
        struct thread_safe_reference;

        struct async_open_task {
            async_open_task(std::shared_ptr<AsyncOpenTask>&&); //NOLINT(google-explicit-constructor)
            void start(std::function<void(thread_safe_reference, std::exception_ptr)> &&fn) const;
        private:
            std::shared_ptr<AsyncOpenTask> m_task;
        };
    }
}

#endif //CPP_REALM_BRIDGE_ASYNC_OPEN_TASK_HPP
