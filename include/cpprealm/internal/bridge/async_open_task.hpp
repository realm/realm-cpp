////////////////////////////////////////////////////////////////////////////
//
// Copyright 2024 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef CPPREALM_BRIDGE_ASYNC_OPEN_TASK_HPP
#define CPPREALM_BRIDGE_ASYNC_OPEN_TASK_HPP

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

#endif //CPPREALM_BRIDGE_ASYNC_OPEN_TASK_HPP
