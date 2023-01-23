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

//#include <cpprealm/db.hpp>
#include <cpprealm/type_info.hpp>
#include <realm/object-store/thread_safe_reference.hpp>
#include <realm/object-store/shared_realm.hpp>

namespace realm {

template <typename ...>
struct db;

template <typename T, typename = void>
struct thread_safe_reference {
};

template <typename T>
struct thread_safe_reference<T, type_info::ObjectBasePersistable<T>> {
    thread_safe_reference(const T& object)
    : m_tsr(ThreadSafeReference(*object.m_object))
    {
    }
    thread_safe_reference(ThreadSafeReference&& tsr)
    : m_tsr(std::move(tsr))
    {
    }
    T resolve(std::shared_ptr<Realm> const&);
private:
    realm::ThreadSafeReference m_tsr;
    template <typename ...Ts>
    friend struct db;
};


//template <typename ...Ts>
//struct db;

template <typename ...Ts>
struct thread_safe_reference<db<Ts...>> {
    thread_safe_reference(ThreadSafeReference tsr)
    : m_tsr(std::move(tsr))
    {
    }
    thread_safe_reference() = default;
    thread_safe_reference(const thread_safe_reference&) = default;
    thread_safe_reference(thread_safe_reference&&) = default;
    thread_safe_reference& operator=(const thread_safe_reference&) = default;
    thread_safe_reference& operator=(thread_safe_reference&&) = default;
    ~thread_safe_reference() = default;

    db<Ts...> resolve()
    {
        return db<Ts...>(Realm::get_shared_realm(std::move(m_tsr)));
    }

    explicit operator bool() const noexcept
    {
        return m_tsr.operator bool();
    }
private:
    ThreadSafeReference m_tsr;
    template <typename ...>
    friend struct db;
};
}


#endif /* realm_thread_safe_reference.hpp */
