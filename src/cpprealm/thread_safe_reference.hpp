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

#ifndef realm_thread_safe_reference_hpp
#define realm_thread_safe_reference_hpp

#include <future>
#include <queue>
#include <iostream>

#include <cpprealm/alpha_support.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/thread_safe_reference.hpp>

namespace realm {
    template <typename>
    struct object;
    class ThreadSafeReference;
    class Realm;
template <typename ...>
struct db;

template <typename T, typename = void>
struct thread_safe_reference;

template <typename T>
struct thread_safe_reference<T, std::enable_if_t<std::is_base_of_v<object<T>, T>>> {
    explicit thread_safe_reference(const T& object)
    : m_tsr(internal::bridge::thread_safe_reference(*object.m_object))
    {
    }
    thread_safe_reference(internal::bridge::thread_safe_reference&& tsr)
    : m_tsr(std::move(tsr))
    {
    }
    T resolve(std::shared_ptr<Realm> const&);
private:
    internal::bridge::thread_safe_reference m_tsr;
    template <typename ...Ts>
    friend struct db;
};

template <typename ...Ts>
struct thread_safe_reference<db<Ts...>> {
    thread_safe_reference(internal::bridge::thread_safe_reference&& tsr)
            : m_tsr(std::move(tsr))
    {
    }
    thread_safe_reference() = default;
    thread_safe_reference(const thread_safe_reference&) = default;
    thread_safe_reference(thread_safe_reference&&) = default;
    thread_safe_reference& operator=(const thread_safe_reference&) = default;
    thread_safe_reference& operator=(thread_safe_reference&&) noexcept = default;
    ~thread_safe_reference() = default;

    db<Ts...> resolve(const std::optional<std::shared_ptr<scheduler>>& s = std::nullopt)
    {
        return db<Ts...>(internal::bridge::realm(std::move(m_tsr), s));
    }

    explicit operator bool() const noexcept
    {
        return m_tsr.operator bool();
    }
private:
    internal::bridge::thread_safe_reference m_tsr;
    template <typename ...>
    friend struct db;
};

}


#endif /* realm_thread_safe_reference.hpp */
