#ifndef CPP_REALM_LNKLST_HPP
#define CPP_REALM_LNKLST_HPP

#include <cstdlib>
#include <type_traits>
#include <memory>

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
        std::shared_ptr<LnkLst> m_lnk_lst;
    };

}

#endif //CPP_REALM_LNKLST_HPP
