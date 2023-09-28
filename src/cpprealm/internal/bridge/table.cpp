#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/query.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/keys.hpp>
#include <realm/mixed.hpp>
#include <realm/table.hpp>
#include <realm/table_view.hpp>

namespace realm::internal::bridge {
    table::table() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_table) TableRef();
#else
        m_table = std::make_shared<TableRef>();
#endif
    }

    table::table(const table& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_table) TableRef(*reinterpret_cast<const TableRef*>(&other.m_table));
#else
        m_table = other.m_table;
#endif
    }

    table& table::operator=(const table& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<TableRef*>(&m_table) = *reinterpret_cast<const TableRef*>(&other.m_table);
        }
#else
        m_table = other.m_table;
#endif
        return *this;
    }

    table::table(table&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_table) TableRef(std::move(*reinterpret_cast<TableRef*>(&other.m_table)));
#else
        m_table = std::move(other.m_table);
#endif
    }

    table& table::operator=(table&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<TableRef*>(&m_table) = std::move(*reinterpret_cast<TableRef*>(&other.m_table));
        }
#else
        m_table = std::move(other.m_table);
#endif
        return *this;
    }

    table::~table() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<TableRef*>(&m_table)->~TableRef();
#endif
    }
    table::table(const TableRef & ref) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_table) TableRef(ref);
#else
        m_table = std::make_shared<TableRef>(ref);
#endif
    }

    table::table(const ConstTableRef &ref) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_table) ConstTableRef(ref);
#else
        m_table = std::make_shared<TableRef>(ref.cast_away_const());
#endif
    }

    table::operator TableRef() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const TableRef*>(&m_table);
#else
        return *m_table;
#endif
    }

    table::operator ConstTableRef() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const ConstTableRef*>(&m_table);
#else
        return *m_table;
#endif
    }

    bool table::is_embedded() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return (*reinterpret_cast<const TableRef*>(&m_table))->is_embedded();
#else
        return m_table->cast_away_const()->is_embedded();
#endif
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

    col_key table::get_column_key(const std::string_view &name) const {
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

    table_view::table_view() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_table_view) TableView();
#else
        m_table_view = std::make_shared<TableView>();
#endif
    }
    table_view::table_view(const table_view& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_table_view) TableView(*reinterpret_cast<const TableRef*>(&other.m_table_view));
#else
        m_table_view = other.m_table_view;
#endif
    }
    table_view& table_view::operator=(const table_view& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<TableView*>(&m_table_view) = *reinterpret_cast<const TableView*>(&other.m_table_view);
        }
#else
        m_table_view = other.m_table_view;
#endif
        return *this;
    }
    table_view::table_view(table_view&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_table_view) TableView(std::move(*reinterpret_cast<TableView*>(&other.m_table_view)));
#else
        m_table_view = std::move(other.m_table_view);
#endif
    }
    table_view& table_view::operator=(table_view&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<TableView*>(&m_table_view) = std::move(*reinterpret_cast<TableView*>(&other.m_table_view));
        }
#else
        m_table_view = std::move(other.m_table_view);
#endif
        return *this;
    }
    table_view::~table_view() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<TableView*>(&m_table_view)->~TableView();
#endif
    }
    table_view::table_view(const TableView &ref) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_table_view) TableView(ref);
#else
        m_table_view = std::make_shared<TableView>(ref);
#endif
    }
    table_view::operator TableView() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const TableView*>(&m_table_view);
#else
        return *m_table_view;
#endif
    }
}
