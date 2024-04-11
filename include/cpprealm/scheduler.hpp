////////////////////////////////////////////////////////////////////////////
//
// Copyright 2022 Realm Inc.
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

#ifndef CPPREALM_SCHEDULER_HPP
#define CPPREALM_SCHEDULER_HPP

#include <functional>
#include <future>
#include <queue>

namespace realm {
    struct scheduler {
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
#endif //CPPREALM_SCHEDULER_HPP
