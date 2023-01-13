#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/query.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>

#include <realm/table.hpp>
#include <realm/mixed.hpp>

namespace realm::internal::bridge {
#ifdef __i386__
    static_assert(SizeCheck<12, sizeof(TableRef)>{});
    static_assert(SizeCheck<4, alignof(TableRef)>{});
    static_assert(SizeCheck<12, sizeof(ConstTableRef)>{});
    static_assert(SizeCheck<4, alignof(ConstTableRef)>{});
#elif __x86_64__
    static_assert(SizeCheck<16, sizeof(TableRef)>{});
    static_assert(SizeCheck<8, alignof(TableRef)>{});
    static_assert(SizeCheck<16, sizeof(ConstTableRef)>{});
    static_assert(SizeCheck<8, alignof(ConstTableRef)>{});
#elif __arm__
    static_assert(SizeCheck<16, sizeof(TableRef)>{});
    static_assert(SizeCheck<8, alignof(TableRef)>{});
    static_assert(SizeCheck<16, sizeof(ConstTableRef)>{});
    static_assert(SizeCheck<8, alignof(ConstTableRef)>{});
#elif __aarch64__
    static_assert(SizeCheck<16, sizeof(TableRef)>{});
    static_assert(SizeCheck<8, alignof(TableRef)>{});
    static_assert(SizeCheck<16, sizeof(ConstTableRef)>{});
    static_assert(SizeCheck<8, alignof(ConstTableRef)>{});
#endif
    table::table(const TableRef & ref)
    {
        new (&m_table) TableRef(ref);
    }

    table::table(const ConstTableRef &ref) {
        new (&m_table) ConstTableRef(ref);
    }

    table::operator TableRef() const {
        return *reinterpret_cast<const TableRef*>(m_table);
    }

    table::operator ConstTableRef() const {
        return *reinterpret_cast<const ConstTableRef*>(m_table);
    }

    bool table::is_embedded() const {
        return (*reinterpret_cast<const TableRef*>(m_table))->is_embedded();
    }

    query table::query(const std::string& a,
                       const std::vector<mixed>& b) const {
        std::vector<Mixed> v2;
        for (auto& v : b) {
            v2.push_back(static_cast<Mixed>(v));
        }
        return static_cast<TableRef>(*this)->query(a, v2);
    }

    obj table::create_object(const obj_key &obj_key) const {
        return static_cast<TableRef>(*this)->create_object(obj_key);
    }

    void table::remove_object(const obj_key & v) const {
        return static_cast<TableRef>(*this)->remove_object(v);
    }

    col_key table::get_column_key(const std::string &name) const {
        return static_cast<TableRef>(*this)->get_column_key(name);
    }

    table table::get_link_target(const col_key col_key) const {
        return static_cast<TableRef>(*this)->get_link_target(col_key);
    }

    obj table::create_object_with_primary_key(const mixed &key) const {
        return static_cast<TableRef>(*this)->create_object_with_primary_key(static_cast<const Mixed>(key));
    }

    bool operator ==(table const& lhs, table const& rhs) {
        return static_cast<TableRef>(lhs) == static_cast<TableRef>(rhs);
    }
//    query table::query(decltype(std::get<0>(PtrExtractor<&table::query>::SpreadArgs)) a,
//                       decltype(std::get<1>(PtrExtractor<&table::query>::SpreadArgs)) b) const {
//        return (*reinterpret_cast<const TableRef*>(m_table))->query(a,
//                                                                    *reinterpret_cast<const std::vector<Mixed>*>(&b));
//    }
}