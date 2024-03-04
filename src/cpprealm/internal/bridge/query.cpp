#include <cpprealm/internal/bridge/query.hpp>

#include <cpprealm/internal/bridge/binary.hpp>
#include <cpprealm/internal/bridge/col_key.hpp>
#include <cpprealm/internal/bridge/decimal128.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/object_id.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/timestamp.hpp>
#include <cpprealm/internal/bridge/uuid.hpp>

#include <realm/query.hpp>
#include <realm/query_expression.hpp>

#define __generate_query_operator(op, type) \
    query &query::op(col_key column_key, type value) { \
        this->operator=(get_query()->op(column_key, value)); \
        return *this; \
    }

#define __generate_query_operator_case_sensitive(op, type) \
    query &query::op(col_key column_key, type value, bool) { \
        this->operator=(get_query()->op(column_key, value)); \
        return *this; \
    }

#define __generate_query_operator_mixed(op) \
    query &query::op(col_key column_key, mixed value, bool) { \
        this->operator=(get_query()->op(column_key, value.operator ::realm::Mixed())); \
        return *this; \
    }
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

    // LINKS
    query subexpr::equal(const std::optional<obj>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<Link>>(m_subexpr) == *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<Link>>(m_subexpr) != ::realm::null();
        }
    }

    query subexpr::not_equal(const std::optional<obj>& rhs) const {
        if (rhs) {
            return *std::dynamic_pointer_cast<::realm::Columns<Link>>(m_subexpr) != *rhs;
        } else {
            return *std::dynamic_pointer_cast<::realm::Columns<Link>>(m_subexpr) != ::realm::null();
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

    template<>
    subexpr link_chain::column<obj>(col_key col_name) {
        return m_link_chain->column<Link>(col_name).clone();
    }

    subexpr link_chain::column_mixed(col_key col_name) {
        return m_link_chain->column<Mixed>(col_name).clone();
    }

    subexpr link_chain::subquery(query subquery) {
        return reinterpret_cast<LinkChain*>(&m_link_chain)->subquery(subquery);
    }

    table link_chain::get_table() {
        return reinterpret_cast< LinkChain*>(&m_link_chain)->get_current_table();
    }


    query::query() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_query) Query();
#else
        m_query = std::make_shared<Query>();
#endif
    }

    query::query(const query& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_query) Query(*reinterpret_cast<const Query*>(&other.m_query));
#else
        m_query = other.m_query;
#endif
    }

    query& query::operator=(const query& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<Query*>(&m_query) = *reinterpret_cast<const Query*>(&other.m_query);
        }
#else
        m_query = other.m_query;
#endif
        return *this;
    }

    query::query(query&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_query) Query(std::move(*reinterpret_cast<Query*>(&other.m_query)));
#else
        m_query = std::move(other.m_query);
#endif
    }

    query& query::operator=(query&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<Query*>(&m_query) = std::move(*reinterpret_cast<Query*>(&other.m_query));
        }
#else
        m_query = std::move(other.m_query);
#endif
        return *this;
    }

    query::~query() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<Query*>(&m_query)->~Query();
#endif
    }

    inline Query* query::get_query() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<Query*>(&m_query);
#else
        return m_query.get();
#endif
    }

    query::query(const table &table) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_query) Query(table.operator ConstTableRef());
#else
        m_query = std::make_shared<Query>(table.operator ConstTableRef());
#endif
    }

    query::query(const Query &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_query) Query(v);
#else
        m_query = std::make_shared<Query>(v);
#endif
    }

    query::operator Query() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const Query*>(&m_query);
#else
        return *m_query;
#endif
    }
    table query::get_table() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<Query*>(&m_query)->get_table();
#else
        return m_query->get_table();
#endif
    }
    query query::and_query(const query &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<Query*>(&m_query)->and_query(v);
#else
        return m_query->and_query(v);
#endif
    }

    __generate_query_operator(greater, int64_t)
    __generate_query_operator(less, int64_t)
    __generate_query_operator(greater_equal, int64_t)
    __generate_query_operator(less_equal, int64_t)
    __generate_query_operator(equal, int64_t)
    __generate_query_operator(not_equal, int64_t)

    __generate_query_operator(greater, double)
    __generate_query_operator(less, double)
    __generate_query_operator(greater_equal, double)
    __generate_query_operator(less_equal, double)
    __generate_query_operator(equal, double)
    __generate_query_operator(not_equal, double)

    __generate_query_operator(greater, timestamp)
    __generate_query_operator(less, timestamp)
    __generate_query_operator(greater_equal, timestamp)
    __generate_query_operator(less_equal, timestamp)
    __generate_query_operator(equal, timestamp)
    __generate_query_operator(not_equal, timestamp)

    __generate_query_operator(greater, internal::bridge::uuid)
    __generate_query_operator(less, internal::bridge::uuid)
    __generate_query_operator(greater_equal, internal::bridge::uuid)
    __generate_query_operator(less_equal, internal::bridge::uuid)
    __generate_query_operator(equal, internal::bridge::uuid)
    __generate_query_operator(not_equal, internal::bridge::uuid)

    __generate_query_operator(greater, internal::bridge::object_id)
    __generate_query_operator(less, internal::bridge::object_id)
    __generate_query_operator(greater_equal, internal::bridge::object_id)
    __generate_query_operator(less_equal, internal::bridge::object_id)
    __generate_query_operator(equal, internal::bridge::object_id)
    __generate_query_operator(not_equal, internal::bridge::object_id)

    __generate_query_operator(greater, internal::bridge::decimal128)
    __generate_query_operator(less, internal::bridge::decimal128)
    __generate_query_operator(greater_equal, internal::bridge::decimal128)
    __generate_query_operator(less_equal, internal::bridge::decimal128)
    __generate_query_operator(equal, internal::bridge::decimal128)
    __generate_query_operator(not_equal, internal::bridge::decimal128)

    __generate_query_operator(equal, bool)
    __generate_query_operator(not_equal, bool)
                                                                                                                                                                                                                                                                                                                    query& links_to(col_key column_key, bool value);
    query& query::links_to(col_key column_key, const internal::bridge::obj& o) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<Query *>(&m_query) = reinterpret_cast<Query *>(&m_query)->links_to(column_key, o.get_key());
#else
        m_query = std::make_shared<Query>(m_query->links_to(column_key, o.get_key()));
#endif
        return *this;
    }

    query& query::not_links_to(col_key column_key, const internal::bridge::obj& o) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<Query *>(&m_query) = reinterpret_cast<Query *>(&m_query)->not_links_to(column_key, {o.get_key()});
#else
        m_query = std::make_shared<Query>(m_query->not_links_to(column_key, {o.get_key()}));
#endif
        return *this;
    }

    query& query::equal(col_key column_key, std::nullopt_t) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<Query *>(&m_query) = reinterpret_cast<Query *>(&m_query)->equal(column_key, ::realm::null{});
#else
        m_query = std::make_shared<Query>(m_query->equal(column_key, ::realm::null{}));
#endif
        return *this;
    }
    query& query::not_equal(col_key column_key, std::nullopt_t) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<Query *>(&m_query) = reinterpret_cast<Query *>(&m_query)->not_equal(column_key, ::realm::null{});
#else
        m_query = std::make_shared<Query>(m_query->not_equal(column_key, ::realm::null{}));
#endif
        return *this;
    }

    query& query::negate() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<Query *>(&m_query) = reinterpret_cast<Query *>(&m_query)->operator!();
#else
        m_query = std::make_shared<Query>(m_query->operator!());
#endif
        return *this;
    }

    std::string query::description() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Query *>(&m_query)->get_description();
#else
        return m_query->get_description();
#endif
    }

    __generate_query_operator_case_sensitive(equal, std::string_view)
    __generate_query_operator_case_sensitive(not_equal, std::string_view)
    __generate_query_operator_case_sensitive(contains, std::string_view)

    __generate_query_operator_mixed(equal)
    __generate_query_operator_mixed(not_equal)

    __generate_query_operator_case_sensitive(equal, binary)
    __generate_query_operator_case_sensitive(not_equal, binary)

    query operator||(query const& lhs, query const& rhs) {
        return static_cast<Query>(lhs) || static_cast<Query>(rhs);
    }
}
