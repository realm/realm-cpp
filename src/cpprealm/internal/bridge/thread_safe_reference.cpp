#include <cpprealm/internal/bridge/thread_safe_reference.hpp>
#include <cpprealm/internal/bridge/object.hpp>
#include <cpprealm/internal/bridge/dictionary.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <realm/object-store/object.hpp>
#include <realm/object-store/dictionary.hpp>
#include <realm/object-store/thread_safe_reference.hpp>

#include <memory>

namespace realm::internal::bridge {
#ifdef __i386__
    static_assert(SizeCheck<4, sizeof(ThreadSafeReference)>{});
    static_assert(SizeCheck<4, alignof(ThreadSafeReference)>{});
#elif __x86_64__
    static_assert(SizeCheck<8, sizeof(ThreadSafeReference)>{});
    static_assert(SizeCheck<8, alignof(ThreadSafeReference)>{});
#elif __arm__
    static_assert(SizeCheck<4, sizeof(ThreadSafeReference)>{});
    static_assert(SizeCheck<4, alignof(ThreadSafeReference)>{});
#elif __aarch64__
    static_assert(SizeCheck<8, sizeof(ThreadSafeReference)>{});
    static_assert(SizeCheck<8, alignof(ThreadSafeReference)>{});
#elif _WIN32
    static_assert(SizeCheck<8, sizeof(ThreadSafeReference)>{});
    static_assert(SizeCheck<8, alignof(ThreadSafeReference)>{});
#endif
    thread_safe_reference::thread_safe_reference() {
        new (&m_thread_safe_reference) ThreadSafeReference();
    }

    thread_safe_reference::thread_safe_reference(thread_safe_reference&& other) {
        new (&m_thread_safe_reference) ThreadSafeReference(std::move(*reinterpret_cast<ThreadSafeReference*>(&other.m_thread_safe_reference)));
    }

    thread_safe_reference& thread_safe_reference::operator=(thread_safe_reference&& other) {
        if (this != &other) {
            *reinterpret_cast<ThreadSafeReference*>(&m_thread_safe_reference) = std::move(*reinterpret_cast<ThreadSafeReference*>(&other.m_thread_safe_reference));
        }
        return *this;
    }

    thread_safe_reference::~thread_safe_reference() {
        reinterpret_cast<ThreadSafeReference*>(&m_thread_safe_reference)->~ThreadSafeReference();
    }
    thread_safe_reference::thread_safe_reference(const object &o) {
        new (&m_thread_safe_reference) ThreadSafeReference(static_cast<Object>(o));
    }
    thread_safe_reference::thread_safe_reference(ThreadSafeReference &&v) {
        new (&m_thread_safe_reference) ThreadSafeReference(std::move(v));
    }
    thread_safe_reference::operator bool() const {
        return reinterpret_cast<const ThreadSafeReference*>(&m_thread_safe_reference)->operator bool();
    }
    thread_safe_reference::operator ThreadSafeReference&&() {
        return std::move(*reinterpret_cast<ThreadSafeReference*>(&m_thread_safe_reference));
    }
    thread_safe_reference::thread_safe_reference(const dictionary &o) {
        new (&m_thread_safe_reference) ThreadSafeReference(static_cast<Dictionary>(o));
    }
}