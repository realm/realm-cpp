#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/query.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>

#include <realm/table.hpp>
#include <realm/table_view.hpp>
#include <realm/keys.hpp>
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
#elif _WIN32
    static_assert(SizeCheck<16, sizeof(TableRef)>{});
    static_assert(SizeCheck<8, alignof(TableRef)>{});
    static_assert(SizeCheck<16, sizeof(ConstTableRef)>{});
    static_assert(SizeCheck<8, alignof(ConstTableRef)>{});
#endif
    table::table() {
        new (&m_table) TableRef();
    }

    table::table(const table& other) {
        new (&m_table) TableRef(*reinterpret_cast<const TableRef*>(&other.m_table));
    }

    table& table::operator=(const table& other) {
        if (this != &other) {
            *reinterpret_cast<TableRef*>(&m_table) = *reinterpret_cast<const TableRef*>(&other.m_table);
        }
        return *this;
    }

    table::table(table&& other) {
        new (&m_table) TableRef(std::move(*reinterpret_cast<TableRef*>(&other.m_table)));
    }

    table& table::operator=(table&& other) {
        if (this != &other) {
            *reinterpret_cast<TableRef*>(&m_table) = std::move(*reinterpret_cast<TableRef*>(&other.m_table));
        }
        return *this;
    }

    table::~table() {
        reinterpret_cast<TableRef*>(&m_table)->~TableRef();
    }
    table::table(const TableRef & ref)
    {
        new (&m_table) TableRef(ref);
    }

    table::table(const ConstTableRef &ref) {
        new (&m_table) ConstTableRef(ref);
    }

    table::operator TableRef() const {
        return *reinterpret_cast<const TableRef*>(&m_table);
    }

    table::operator ConstTableRef() const {
        return *reinterpret_cast<const ConstTableRef*>(&m_table);
    }

    bool table::is_embedded() const {
        return (*reinterpret_cast<const TableRef*>(&m_table))->is_embedded();
    }

    query table::query(const std::string& a,
                       const std::vector<mixed>& b) const {
        std::vector<Mixed> v2;
        for (auto& v : b) {
            v2.push_back(v.operator ::realm::Mixed());
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

    obj table::create_object_with_primary_key(const bridge::mixed& key) const {
        return static_cast<TableRef>(*this)->create_object_with_primary_key(key.operator ::realm::Mixed());
    }
    bool table::is_valid(const obj_key &key) const {
        return static_cast<TableRef>(*this)->is_valid(key);
    }
    obj table::get_object(const obj_key &key) const {
        return static_cast<TableRef>(*this)->get_object(key);
    }

    bool operator ==(table const& lhs, table const& rhs) {
        return static_cast<TableRef>(lhs) == static_cast<TableRef>(rhs);
    }
    bool operator!=(table const &lhs, table const &rhs) {
        return static_cast<TableRef>(lhs) != static_cast<TableRef>(rhs);
    }

#ifdef __i386__
    static_assert(SizeCheck<316, sizeof(TableView)>{});
    static_assert(SizeCheck<4, alignof(TableView)>{});
#elif __x86_64__
#if defined(__clang__)
    static_assert(SizeCheck<568, sizeof(TableView)>{});
    static_assert(SizeCheck<8, alignof(TableView)>{});
#elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<576, sizeof(TableView)>{});
    static_assert(SizeCheck<8, alignof(TableView)>{});
#endif
#elif __arm__
    static_assert(SizeCheck<368, sizeof(TableView)>{});
    static_assert(SizeCheck<8, alignof(TableView)>{});
#elif __aarch64__
#if defined(__clang__)
    static_assert(SizeCheck<568, sizeof(TableView)>{});
    static_assert(SizeCheck<8, alignof(TableView)>{});
#elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<576, sizeof(TableView)>{});
    static_assert(SizeCheck<8, alignof(TableView)>{});
#endif
#elif _WIN32
    #if _DEBUG
    static_assert(SizeCheck<624, sizeof(TableView)>{});
    #else
    static_assert(SizeCheck<576, sizeof(TableView)>{});
    #endif
    static_assert(SizeCheck<8, alignof(TableView)>{});
#endif

    table_view::table_view() {
        new (&m_table_view) TableView();
    }
    table_view::table_view(const table_view& other) {
        new (&m_table_view) TableView(*reinterpret_cast<const TableRef*>(&other.m_table_view));
    }
    table_view& table_view::operator=(const table_view& other) {
        if (this != &other) {
            *reinterpret_cast<TableView*>(&m_table_view) = *reinterpret_cast<const TableView*>(&other.m_table_view);
        }
        return *this;
    }
    table_view::table_view(table_view&& other) {
        new (&m_table_view) TableView(std::move(*reinterpret_cast<TableView*>(&other.m_table_view)));
    }
    table_view& table_view::operator=(table_view&& other) {
        if (this != &other) {
            *reinterpret_cast<TableView*>(&m_table_view) = std::move(*reinterpret_cast<TableView*>(&other.m_table_view));
        }
        return *this;
    }
    table_view::~table_view() {
        reinterpret_cast<TableView*>(&m_table_view)->~TableView();
    }
    table_view::table_view(const TableView &ref) {
        new (&m_table_view) TableView(ref);
    }
    table_view::operator TableView() const {
        return *reinterpret_cast<const TableView*>(&m_table_view);
    }
}
