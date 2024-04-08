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

#ifndef CPPREALM_THREAD_SAFE_REFERENCE_HPP
#define CPPREALM_THREAD_SAFE_REFERENCE_HPP

#include <cpprealm/internal/bridge/thread_safe_reference.hpp>

namespace realm {

    template <typename T>
    struct thread_safe_reference {
        explicit thread_safe_reference(const managed<T>& object)
            : m_tsr(internal::bridge::thread_safe_reference(internal::bridge::object(object.m_realm, object.m_obj))) { }
    private:
        internal::bridge::thread_safe_reference m_tsr;
        friend struct db;
    };
}

#endif /* CPPREALM_THREAD_SAFE_REFERENCE_HPP */
