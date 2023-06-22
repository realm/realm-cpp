#ifndef CPP_REALM_LNKLST_HPP
#define CPP_REALM_LNKLST_HPP

#include <cstdlib>
#include <type_traits>

namespace realm {
    class LnkLst;
}
namespace realm::internal::bridge {
    struct obj;
    struct obj_key;

    struct lnklst {
        lnklst() ;
        lnklst(const lnklst& other) ;
        lnklst& operator=(const lnklst& other) ;
        lnklst(lnklst&& other);
        lnklst& operator=(lnklst&& other);
        ~lnklst();
        lnklst(const LnkLst&); // NOLINT
        operator LnkLst() const; //NOLINT
        obj create_and_insert_linked_object(size_t idx);
        void add(const obj_key&);
    private:
#ifdef __i386__
        std::aligned_storage<160, 8>::type m_lnk_lst[1];
#elif __x86_64__
        std::aligned_storage<160, 8>::type m_lnk_lst[1];
#elif __arm__
        std::aligned_storage<128, 8>::type m_lnk_lst[1];
#elif __aarch64__
        std::aligned_storage<160, 8>::type m_lnk_lst[1];
#elif _WIN32
        std::aligned_storage<168, 8>::type m_lnk_lst[1];
#endif
    };

}

#endif //CPP_REALM_LNKLST_HPP
