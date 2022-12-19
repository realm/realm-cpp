#include "thread_safe_reference.hpp"
#include <cpprealm/internal/bridge/object.hpp>
#include <cpprealm/internal/bridge/dictionary.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <realm/object-store/object.hpp>
#include <realm/object-store/dictionary.hpp>
#include <realm/object-store/thread_safe_reference.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<8, sizeof(ThreadSafeReference)>{});

    thread_safe_reference::thread_safe_reference(const object &o) {
        new (&m_thread_safe_reference) ThreadSafeReference(static_cast<Object>(o));
    }
    thread_safe_reference::thread_safe_reference(ThreadSafeReference &&v) {
        new (&m_thread_safe_reference) ThreadSafeReference(std::move(v));
    }
    thread_safe_reference::operator bool() const {
        return reinterpret_cast<const ThreadSafeReference*>(m_thread_safe_reference)->operator bool();
    }
    thread_safe_reference::operator ThreadSafeReference&&() {
        return std::move(*reinterpret_cast<ThreadSafeReference*>(m_thread_safe_reference));
    }
    thread_safe_reference::thread_safe_reference(const dictionary &o) {
        new (&m_thread_safe_reference) ThreadSafeReference(static_cast<Dictionary>(o));
    }
}