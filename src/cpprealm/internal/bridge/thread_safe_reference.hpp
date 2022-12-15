//
// Created by Jason Flax on 12/5/22.
//

#ifndef CPP_REALM_THREAD_SAFE_REFERENCE_HPP
#define CPP_REALM_THREAD_SAFE_REFERENCE_HPP

namespace realm {
    class ThreadSafeReference;
}
namespace realm::internal::bridge {
    struct object;
    struct dictionary;

    struct thread_safe_reference {
        thread_safe_reference(const object&);
        thread_safe_reference(const dictionary&);
        operator ThreadSafeReference&&();
        operator bool() const; //NOLINT(google-explicit-constructor)
    private:
        friend struct realm;
        unsigned char m_thread_safe_reference[8]{};
    };

}


#endif //CPP_REALM_THREAD_SAFE_REFERENCE_HPP
