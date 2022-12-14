#include <cpprealm/internal/bridge/lnklst.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/obj_key.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/list.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<160, sizeof(LnkLst)>{});

    lnklst::lnklst(const LnkLst &v) {
        new (&m_lnk_lst) LnkLst(v);
    }

    obj lnklst::create_and_insert_linked_object(size_t idx) {
        return reinterpret_cast<LnkLst*>(m_lnk_lst)->create_and_insert_linked_object(idx);
    }

    void lnklst::add(const obj_key &v) {
        return reinterpret_cast<LnkLst*>(m_lnk_lst)->add(v);
    }

    lnklst::operator LnkLst() const {
        return *reinterpret_cast<const LnkLst*>(m_lnk_lst);
    }
}