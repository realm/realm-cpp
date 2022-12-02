#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/table.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<16, sizeof(TableRef)>{});
    static_assert(SizeCheck<16, sizeof(ConstTableRef)>{});

    table::table(const TableRef & ref)
    {
        new (&m_table) TableRef(ref);
    }

    table::table(const realm::ConstTableRef &ref) {
        new (&m_table) ConstTableRef(ref);
    }
    bool table::is_embedded() const {
        auto ref = reinterpret_cast<const TableRef*>(m_table);
        return ref->operator->()->is_embedded();
    }
}