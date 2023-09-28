#include <cpprealm/internal/bridge/lnklst.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/obj_key.hpp>

#include <realm/list.hpp>

namespace realm::internal::bridge {
    
    lnklst::lnklst() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPESX
        new (&m_lnk_lst) LnkLst();
#else
        m_lnk_lst = std::make_shared<LnkLst>();
#endif
    }
    
    lnklst::lnklst(const lnklst& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPESX
        new (&m_lnk_lst) LnkLst(*reinterpret_cast<const LnkLst*>(&other.m_lnk_lst));
#else
        m_lnk_lst = other.m_lnk_lst;
#endif
    }

    lnklst& lnklst::operator=(const lnklst& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPESX
        if (this != &other) {
            *reinterpret_cast<LnkLst*>(&m_lnk_lst) = *reinterpret_cast<const LnkLst*>(&other.m_lnk_lst);
        }
#else
        m_lnk_lst = other.m_lnk_lst;
#endif
        return *this;
    }

    lnklst::lnklst(lnklst&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPESX
        new (&m_lnk_lst) LnkLst(std::move(*reinterpret_cast<LnkLst*>(&other.m_lnk_lst)));
#else
        m_lnk_lst = std::move(other.m_lnk_lst);
#endif
    }

    lnklst& lnklst::operator=(lnklst&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPESX
        if (this != &other) {
            *reinterpret_cast<LnkLst*>(&m_lnk_lst) = std::move(*reinterpret_cast<LnkLst*>(&other.m_lnk_lst));
        }
#else
        m_lnk_lst = std::move(other.m_lnk_lst);
#endif
        return *this;
    }

    lnklst::~lnklst() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPESX
        reinterpret_cast<LnkLst*>(&m_lnk_lst)->~LnkLst();
#endif
    }

    lnklst::lnklst(const LnkLst &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPESX
        new (&m_lnk_lst) LnkLst(v);
#else
        m_lnk_lst = std::make_shared<LnkLst>(v);
#endif
    }

    obj lnklst::create_and_insert_linked_object(size_t idx) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPESX
        return reinterpret_cast<LnkLst*>(&m_lnk_lst)->create_and_insert_linked_object(idx);
#else
        return m_lnk_lst->create_and_insert_linked_object(idx);
#endif
    }

    void lnklst::add(const obj_key &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPESX
        return reinterpret_cast<LnkLst*>(&m_lnk_lst)->add(v);
#else
        return m_lnk_lst->add(v);
#endif
    }

    lnklst::operator LnkLst() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPESX
        return *reinterpret_cast<const LnkLst*>(&m_lnk_lst);
#else
        return *m_lnk_lst;
#endif
    }
}