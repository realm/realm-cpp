#include <cpprealm/internal/bridge/thread_safe_reference.hpp>
#include <cpprealm/internal/bridge/dictionary.hpp>
#include <cpprealm/internal/bridge/object.hpp>

#include <realm/object-store/dictionary.hpp>
#include <realm/object-store/object.hpp>
#include <realm/object-store/thread_safe_reference.hpp>

#include <memory>

namespace realm::internal::bridge {
    thread_safe_reference::thread_safe_reference() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_thread_safe_reference) ThreadSafeReference();
#else
        m_thread_safe_reference = std::make_shared<ThreadSafeReference>();
#endif
    }

    thread_safe_reference::thread_safe_reference(thread_safe_reference&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_thread_safe_reference) ThreadSafeReference(std::move(*reinterpret_cast<ThreadSafeReference*>(&other.m_thread_safe_reference)));
#else
        m_thread_safe_reference = std::move(other.m_thread_safe_reference);
#endif
    }

    thread_safe_reference& thread_safe_reference::operator=(thread_safe_reference&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<ThreadSafeReference*>(&m_thread_safe_reference) = std::move(*reinterpret_cast<ThreadSafeReference*>(&other.m_thread_safe_reference));
        }
#else
        m_thread_safe_reference = other.m_thread_safe_reference;
#endif
        return *this;
    }

    thread_safe_reference::~thread_safe_reference() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<ThreadSafeReference*>(&m_thread_safe_reference)->~ThreadSafeReference();
#endif
    }
    thread_safe_reference::thread_safe_reference(const object &o) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_thread_safe_reference) ThreadSafeReference(static_cast<Object>(o));
#else
        m_thread_safe_reference = std::make_shared<ThreadSafeReference>(static_cast<Object>(o));
#endif
    }
    thread_safe_reference::thread_safe_reference(ThreadSafeReference &&v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_thread_safe_reference) ThreadSafeReference(std::move(v));
#else
        m_thread_safe_reference = std::make_shared<ThreadSafeReference>(std::move(v));
#endif
    }
    thread_safe_reference::operator bool() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const ThreadSafeReference*>(&m_thread_safe_reference)->operator bool();
#else
        return m_thread_safe_reference->operator bool();
#endif
    }
    thread_safe_reference::operator ThreadSafeReference&&() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return std::move(*reinterpret_cast<ThreadSafeReference*>(&m_thread_safe_reference));
#else
        return std::move(*m_thread_safe_reference);
#endif
    }
    thread_safe_reference::thread_safe_reference(const dictionary &o) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_thread_safe_reference) ThreadSafeReference(static_cast<Dictionary>(o));
#else
        m_thread_safe_reference = std::make_shared<ThreadSafeReference>(static_cast<Dictionary>(o));
#endif
    }
}