#ifndef CPP_REALM_LNKLST_HPP
#define CPP_REALM_LNKLST_HPP

#include <cstdlib>
#include <memory>
#include <cpprealm/internal/bridge/utils.hpp>

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
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPESX
        storage::LnkLst m_lnk_lst[1];
#else
        std::shared_ptr<LnkLst> m_lnk_lst;
#endif
    };

}

#endif //CPP_REALM_LNKLST_HPP
