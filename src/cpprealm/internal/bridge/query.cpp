#include "query.hpp"
#include <cpprealm/internal/bridge/col_key.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <realm/query.hpp>
#include <cpprealm/internal/bridge/binary.hpp>
#include <cpprealm/internal/bridge/timestamp.hpp>
#include <cpprealm/internal/bridge/uuid.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>

#define __generate_query_operator(op, type) \
    query &query::op(col_key column_key, type value) { \
        *reinterpret_cast<Query *>(m_query) = reinterpret_cast<Query *>(m_query)->op(column_key, value); \
        return *this; \
    }

#define __generate_query_operator_case_sensitive(op, type) \
    query &query::op(col_key column_key, type value, bool) { \
        *reinterpret_cast<Query *>(m_query) = reinterpret_cast<Query *>(m_query)->op(column_key, value); \
        return *this; \
    }

#define __generate_query_operator_mixed(op) \
    query &query::op(col_key column_key, mixed value, bool) { \
        *reinterpret_cast<Query *>(m_query) = reinterpret_cast<Query *>(m_query)->op(column_key, static_cast<Mixed>(value)); \
        return *this; \
    }
namespace realm::internal::bridge {
    static_assert(SizeCheck<128, sizeof(Query)>{});

    query::query(const realm::internal::bridge::table &table) {
        new (&m_query) Query(static_cast<ConstTableRef>(table));
    }

    query::query(const realm::Query &v) {
        new (&m_query) Query(v);
    }

    query::operator Query() const {
        return *reinterpret_cast<const Query*>(m_query);
    }
    table query::get_table() {
        return reinterpret_cast<Query*>(m_query)->get_table();
    }
    query query::and_query(const realm::internal::bridge::query &v) {
        return reinterpret_cast<Query*>(m_query)->and_query(v);
    }

    __generate_query_operator(equal, std::nullopt_t)
    __generate_query_operator(not_equal, std::nullopt_t)

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

    __generate_query_operator(equal, bool)
    __generate_query_operator(not_equal, bool)

    __generate_query_operator_case_sensitive(equal, std::string_view)
    __generate_query_operator_case_sensitive(not_equal, std::string_view)

    __generate_query_operator_mixed(equal)
    __generate_query_operator_mixed(not_equal)

    __generate_query_operator_case_sensitive(equal, binary)
    __generate_query_operator_case_sensitive(not_equal, binary)

    query operator||(query const& lhs, query const& rhs) {
        return static_cast<Query>(lhs) || static_cast<Query>(rhs);
    }
}