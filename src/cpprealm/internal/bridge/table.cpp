#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/query.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/keys.hpp>
#include <realm/mixed.hpp>
#include <realm/table.hpp>
#include <realm/table_view.hpp>
#include <realm/query_expression.hpp>

namespace realm::internal::bridge {

    subexpr::subexpr(std::unique_ptr<Subexpr> other) {
        m_subexpr = std::move(other);
    }

    // INT
    query subexpr::equal(const std::optional<int64_t>& rhs) const {
        return *std::dynamic_pointer_cast<::realm::Columns<Int>>(m_subexpr) == rhs;
    }

    query subexpr::not_equal(const std::optional<int64_t>& rhs) const {
        return *std::dynamic_pointer_cast<::realm::Columns<Int>>(m_subexpr) != rhs;
    }

    query subexpr::greater(const std::optional<int64_t>& rhs) const {
        return *std::dynamic_pointer_cast<::realm::Columns<Int>>(m_subexpr) > rhs;
    }

    query subexpr::less(const std::optional<int64_t>& rhs) const {
        return *std::dynamic_pointer_cast<::realm::Columns<Int>>(m_subexpr) < rhs;
    }

    query subexpr::greater_equal(const std::optional<int64_t>& rhs) const {
        return *std::dynamic_pointer_cast<::realm::Columns<Int>>(m_subexpr) >= rhs;
    }

    query subexpr::less_equal(const std::optional<int64_t>& rhs) const {
        return *std::dynamic_pointer_cast<::realm::Columns<Int>>(m_subexpr) <= rhs;
    }

    // BOOL
    query subexpr::equal(const std::optional<bool>& rhs) const {
        return *std::dynamic_pointer_cast<::realm::Columns<Bool>>(m_subexpr) == rhs;
    }

    query subexpr::not_equal(const std::optional<bool>& rhs) const {
        return *std::dynamic_pointer_cast<::realm::Columns<Bool>>(m_subexpr) != rhs;
    }

    // DOUBLE
    query subexpr::equal(const std::optional<double>& rhs) const {
        return *std::dynamic_pointer_cast<::realm::Columns<Double>>(m_subexpr) == rhs;
    }

    query subexpr::not_equal(const std::optional<double>& rhs) const {
        return *std::dynamic_pointer_cast<::realm::Columns<Double>>(m_subexpr) != rhs;
    }

    query subexpr::greater(const std::optional<double>& rhs) const {
        return *std::dynamic_pointer_cast<::realm::Columns<Double>>(m_subexpr) > rhs;
    }

    query subexpr::less(const std::optional<double>& rhs) const {
        return *std::dynamic_pointer_cast<::realm::Columns<Double>>(m_subexpr) < rhs;
    }

    query subexpr::greater_equal(const std::optional<double>& rhs) const {
        return *std::dynamic_pointer_cast<::realm::Columns<Double>>(m_subexpr) >= rhs;
    }

    query subexpr::less_equal(const std::optional<double>& rhs) const {
        return *std::dynamic_pointer_cast<::realm::Columns<Double>>(m_subexpr) <= rhs;
    }

    // BINARY
    query subexpr::equal(const std::optional<binary>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<BinaryData>>(m_subexpr) == *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<BinaryData>>(m_subexpr) == ::realm::null();
        }
    }

    query subexpr::not_equal(const std::optional<binary>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<BinaryData>>(m_subexpr) != *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<BinaryData>>(m_subexpr) != ::realm::null();
        }
    }

    // TIMESTAMP
    query subexpr::equal(const std::optional<timestamp>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<Timestamp>>(m_subexpr) == *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<Timestamp>>(m_subexpr) == ::realm::null();
        }
    }

    query subexpr::not_equal(const std::optional<timestamp>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<Timestamp>>(m_subexpr) != *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<Timestamp>>(m_subexpr) != ::realm::null();
        }
    }

    query subexpr::greater(const std::optional<timestamp>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<Timestamp>>(m_subexpr) > *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<Timestamp>>(m_subexpr) > ::realm::null();
        }
    }

    query subexpr::less(const std::optional<timestamp>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<Timestamp>>(m_subexpr) < *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<Timestamp>>(m_subexpr) < ::realm::null();
        }
    }

    query subexpr::greater_equal(const std::optional<timestamp>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<Timestamp>>(m_subexpr) >= *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<Timestamp>>(m_subexpr) >= ::realm::null();
        }
    }

    query subexpr::less_equal(const std::optional<timestamp>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<Timestamp>>(m_subexpr) <= *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<Timestamp>>(m_subexpr) <= ::realm::null();
        }
    }

    // UUID
    query subexpr::equal(const std::optional<uuid>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<UUID>>(m_subexpr) == *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<UUID>>(m_subexpr) == ::realm::null();
        }
    }

    query subexpr::not_equal(const std::optional<uuid>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<UUID>>(m_subexpr) != *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<UUID>>(m_subexpr) != ::realm::null();
        }
    }

    // OBJECT_ID
    query subexpr::equal(const std::optional<object_id>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<ObjectId>>(m_subexpr) == *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<ObjectId>>(m_subexpr) == ::realm::null();
        }
    }

    query subexpr::not_equal(const std::optional<object_id>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<ObjectId>>(m_subexpr) != *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<ObjectId>>(m_subexpr) != ::realm::null();
        }
    }

    // DECIMAL128
    query subexpr::equal(const std::optional<decimal128>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<Decimal128>>(m_subexpr) == *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<Decimal128>>(m_subexpr) == ::realm::null();
        }
    }

    query subexpr::not_equal(const std::optional<decimal128>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<Decimal128>>(m_subexpr) != *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<Decimal128>>(m_subexpr) != ::realm::null();
        }
    }

    query subexpr::greater(const std::optional<decimal128>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<Decimal128>>(m_subexpr) > *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<Decimal128>>(m_subexpr) > ::realm::null();
        }
    }

    query subexpr::less(const std::optional<decimal128>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<Decimal128>>(m_subexpr) < *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<Decimal128>>(m_subexpr) < ::realm::null();
        }
    }

    query subexpr::greater_equal(const std::optional<decimal128>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<Decimal128>>(m_subexpr) >= *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<Decimal128>>(m_subexpr) >= ::realm::null();
        }
    }

    query subexpr::less_equal(const std::optional<decimal128>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<Decimal128>>(m_subexpr) <= *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<Decimal128>>(m_subexpr) <= ::realm::null();
        }
    }

    // STRING
    query subexpr::equal(const std::optional<std::string>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<StringData>>(m_subexpr) == *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<StringData>>(m_subexpr) >= ::realm::null();
        }
    }

    query subexpr::not_equal(const std::optional<std::string>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<StringData>>(m_subexpr) != *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<StringData>>(m_subexpr) != ::realm::null();
        }
    }

    query subexpr::contains(const std::optional<std::string>& rhs, bool case_sensitive) const {
        return std::dynamic_pointer_cast<::realm::Columns<StringData>>(m_subexpr)->contains(rhs, case_sensitive);
    }

    // MIXED
    query subexpr::mixed_equal(const internal::bridge::mixed& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<Mixed>>(m_subexpr) == rhs.operator Mixed();
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<Mixed>>(m_subexpr) == ::realm::null();
        }
    }

    query subexpr::mixed_not_equal(const internal::bridge::mixed& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<Mixed>>(m_subexpr) != rhs.operator Mixed();
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<Mixed>>(m_subexpr) != ::realm::null();
        }
    }

    link_chain::link_chain() {
        m_link_chain = std::make_shared<LinkChain>();
    }

    link_chain::link_chain(const link_chain& other) {
        m_link_chain = other.m_link_chain;
    }

    link_chain& link_chain::operator=(const link_chain& other) {
        m_link_chain = other.m_link_chain;
        return *this;
    }

    link_chain::link_chain(link_chain&& other) {
        m_link_chain = std::move(other.m_link_chain);
    }

    link_chain& link_chain::operator=(link_chain&& other) {
        m_link_chain = std::move(other.m_link_chain);
        return *this;
    }

    link_chain::link_chain(const LinkChain& other) {
        m_link_chain = std::make_shared<LinkChain>(other);
    }

    link_chain& link_chain::link(col_key ck) {
        m_link_chain->link(ck);
        return *this;
    }

    link_chain& link_chain::link(std::string col_name) {
        m_link_chain->link(col_name);
        return *this;
    }

    link_chain& link_chain::backlink(const table& origin, col_key origin_col_key) {
        reinterpret_cast<LinkChain*>(&m_link_chain)->backlink(origin.operator ConstTableRef().operator*(), origin_col_key);
        return *this;
    }

    template<>
    subexpr link_chain::column<int64_t>(col_key col_name) {
        return m_link_chain->column<Int>(col_name).clone();
    }

    template<>
    subexpr link_chain::column<bool>(col_key col_name) {
        return m_link_chain->column<Bool>(col_name).clone();
    }

    template<>
    subexpr link_chain::column<std::string>(col_key col_name) {
        return m_link_chain->column<StringData>(col_name).clone();
    }

    template<>
    subexpr link_chain::column<std::vector<uint8_t>>(col_key col_name) {
        return m_link_chain->column<BinaryData>(col_name).clone();
    }

    template<>
    subexpr link_chain::column<std::chrono::time_point<std::chrono::system_clock>>(col_key col_name) {
        return m_link_chain->column<Timestamp>(col_name).clone();
    }

    template<>
    subexpr link_chain::column<double>(col_key col_name) {
        return m_link_chain->column<Double>(col_name).clone();
    }

    template<>
    subexpr link_chain::column<::realm::decimal128>(col_key col_name) {
        return m_link_chain->column<Decimal128>(col_name).clone();
    }

    template<>
    subexpr link_chain::column<::realm::object_id>(col_key col_name) {
        return m_link_chain->column<ObjectId>(col_name).clone();
    }

    template<>
    subexpr link_chain::column<::realm::uuid>(col_key col_name) {
        return m_link_chain->column<UUID>(col_name).clone();
    }

    subexpr link_chain::subquery(query subquery) {
        return reinterpret_cast<LinkChain*>(&m_link_chain)->subquery(subquery);
    }

    table link_chain::get_table() {
        return reinterpret_cast< LinkChain*>(&m_link_chain)->get_current_table();
    }

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

    uint32_t table::get_key() const {
        return static_cast<TableRef>(*this)->get_key().value;
    }

    std::string table::get_name() const {
        return static_cast<TableRef>(*this)->get_name();
    }

    table table::get_link_target(const col_key col_key) const {
        return static_cast<TableRef>(*this)->get_link_target(col_key);
    }

    link_chain table::get_link(const col_key col_key) const {
        return static_cast<const TableRef>(*this)->link(col_key);
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
