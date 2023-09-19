#ifndef CPP_REALM_THREAD_SAFE_REFERENCE_HPP
#define CPP_REALM_THREAD_SAFE_REFERENCE_HPP

#include <type_traits>

namespace realm {
    class ThreadSafeReference;
}
namespace realm::internal::bridge {
    struct object;
    struct dictionary;
    struct realm;

    struct thread_safe_reference {
        thread_safe_reference();
        thread_safe_reference(const thread_safe_reference& other) = delete;
        thread_safe_reference& operator=(const thread_safe_reference& other) = delete;
        thread_safe_reference(thread_safe_reference&& other);
        thread_safe_reference& operator=(thread_safe_reference&& other);
        ~thread_safe_reference();
        thread_safe_reference(const object&);
        thread_safe_reference(const dictionary&);
        thread_safe_reference(ThreadSafeReference&&);
        operator ThreadSafeReference&&();
        operator bool() const; //NOLINT(google-explicit-constructor)
    private:
        friend struct realm;
        template <typename T>
        friend T resolve(const realm&, thread_safe_reference&& tsr);
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::ThreadSafeReference m_thread_safe_reference[1];
#elif __i386__
        std::aligned_storage<4, 4>::type m_thread_safe_reference[1];
#elif __x86_64__
        std::aligned_storage<8, 8>::type m_thread_safe_reference[1];
#elif __arm__
        std::aligned_storage<4, 4>::type m_thread_safe_reference[1];
#elif __aarch64__
        std::aligned_storage<8, 8>::type m_thread_safe_reference[1];
#elif _WIN32
        std::aligned_storage<8, 8>::type m_thread_safe_reference[1];
#endif
    };

}


#endif //CPP_REALM_THREAD_SAFE_REFERENCE_HPP
