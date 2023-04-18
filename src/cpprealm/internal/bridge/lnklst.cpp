#include <cpprealm/internal/bridge/lnklst.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/obj_key.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/list.hpp>

namespace realm::internal::bridge {
    lnklst::lnklst(const LnkLst &v) {
        m_lnk_lst = std::make_shared<LnkLst>(LnkLst(v));
    }

    obj lnklst::create_and_insert_linked_object(size_t idx) {
        return m_lnk_lst->create_and_insert_linked_object(idx);
    }

    void lnklst::add(const obj_key &v) {
        return m_lnk_lst->add(v);
    }

    lnklst::operator LnkLst() const {
        return *m_lnk_lst;
    }
}