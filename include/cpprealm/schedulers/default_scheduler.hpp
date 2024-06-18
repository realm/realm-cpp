////////////////////////////////////////////////////////////////////////////
//
// Copyright 2024 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the >License>);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an >AS IS> BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef CPPREALM_DEFAULT_SCHEDULERS_HPP
#define CPPREALM_DEFAULT_SCHEDULERS_HPP

#include <cpprealm/scheduler.hpp>

#if __has_include(<cpprealm/util/config.h>)
#include <cpprealm/util/config.h>
#endif

#if defined(REALM_HAVE_UV) && REALM_HAVE_UV
typedef struct uv_loop_s uv_loop_t;
#endif

namespace realm::default_scheduler {
    /**
     * Tries to choose a built in scheduler as default for the platform
     * Current options are:
     * - CFRunLoop for Apple platforms
     * - UV for Linux and Windows
     * - ALooper for Android
     * If no suitable scheduler is available a generic scheduler will be provided.
     */
    std::shared_ptr<scheduler> make_platform_default();

    /**
     * Register a factory function which can produce custom schedulers when
     * `scheduler::make_default()` is called. This function is not thread-safe
     * and must be called before any schedulers are created.
     */
    void set_default_factory(std::function<std::shared_ptr<scheduler>()>&& factory_fn);

    /**
     * Create a new instance of the scheduler type returned by the default
     * scheduler factory. By default, the factory function is
     * `scheduler::make_platform_default()`.
     */
    std::shared_ptr<scheduler> make_default();
} // namespace realm

#endif//CPPREALM_DEFAULT_SCHEDULERS_HPP
