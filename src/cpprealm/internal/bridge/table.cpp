#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/query.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>

#include <realm/table.hpp>
#include <realm/mixed.hpp>
//
//#define FE_0(WHAT, cls)
//#define FE_1(WHAT, cls, X) WHAT(cls, X)
//#define FE_2(WHAT, cls, X, ...) WHAT(cls, X)FE_1(WHAT, cls, __VA_ARGS__)
//#define FE_3(WHAT, X, ...) WHAT(X)FE_2(WHAT, __VA_ARGS__)
//#define FE_4(WHAT, X, ...) WHAT(X)FE_3(WHAT, __VA_ARGS__)
//#define FE_5(WHAT, X, ...) WHAT(X)FE_4(WHAT, __VA_ARGS__)
//
//#define GET_MACRO(_0,_1,_2,_3,_4,_5,NAME,...) NAME
//#define FOR_EACH(action, cls, ...) \
//  GET_MACRO(_0,__VA_ARGS__,FE_5,FE_4,FE_3,FE_2,FE_1,FE_0)(action, cls, __VA_ARGS__)
//
//#define __cpp_realm_generate_table_method(res, name, ...) \
//    res table::name(__VA_ARGS__) {                        \
//        return (*reinterpret_cast<const TableRef*>(m_table))->name() \                                                      \
//    }
//#define __cpp_realm_generate_table_method_const(res, name) \
//    res table::name
namespace realm::internal::bridge {
    static_assert(SizeCheck<16, sizeof(TableRef)>{});
    static_assert(SizeCheck<16, sizeof(ConstTableRef)>{});

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