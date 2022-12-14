#ifndef CPP_REALM_LNKLST_HPP
#define CPP_REALM_LNKLST_HPP

#include <cstdlib>

namespace realm {
    class LnkLst;
}
namespace realm::internal::bridge {
    struct obj;
    struct obj_key;

    struct lnklst {
        lnklst(const LnkLst&); // NOLINT
        operator LnkLst() const; //NOLINT
        obj create_and_insert_linked_object(size_t idx);
        void add(const obj_key&);
    private:
        unsigned char m_lnk_lst[160]{};
    };

}

#endif //CPP_REALM_LNKLST_HPP
