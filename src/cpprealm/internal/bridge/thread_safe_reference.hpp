//
// Created by Jason Flax on 12/5/22.
//

#ifndef CPP_REALM_THREAD_SAFE_REFERENCE_HPP
#define CPP_REALM_THREAD_SAFE_REFERENCE_HPP

namespace realm::internal::bridge {
    struct object;

    struct thread_safe_reference {
        thread_safe_reference(const object&);
        operator bool() const;
    private:
        unsigned char m_thread_safe_reference[8]{};
    };

}


#endif //CPP_REALM_THREAD_SAFE_REFERENCE_HPP
