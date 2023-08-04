#include <cpprealm/internal/bridge/query.hpp>
#include <cpprealm/internal/bridge/col_key.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <realm/query.hpp>
#include <cpprealm/internal/bridge/binary.hpp>
#include <cpprealm/internal/bridge/timestamp.hpp>
#include <cpprealm/internal/bridge/uuid.hpp>
#include <cpprealm/internal/bridge/object_id.hpp>
#include <cpprealm/internal/bridge/decimal128.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>

#define __generate_query_operator(op, type) \
    query &query::op(col_key column_key, type value) { \
        *reinterpret_cast<Query *>(&m_query) = reinterpret_cast<Query *>(&m_query)->op(column_key, value); \
        return *this; \
    }

#define __generate_query_operator_case_sensitive(op, type) \
    query &query::op(col_key column_key, type value, bool) { \
        *reinterpret_cast<Query *>(&m_query) = reinterpret_cast<Query *>(&m_query)->op(column_key, value); \
        return *this; \
    }

#define __generate_query_operator_mixed(op) \
    query &query::op(col_key column_key, mixed value, bool) { \
        *reinterpret_cast<Query *>(&m_query) = reinterpret_cast<Query *>(&m_query)->op(column_key, value.operator ::realm::Mixed()); \
        return *this; \
    }
namespace realm::internal::bridge {
#ifdef __i386__
    static_assert(SizeCheck<68, sizeof(Query)>{});
    static_assert(SizeCheck<4, alignof(Query)>{});
#elif __x86_64__
    #if defined(__clang__)
    static_assert(SizeCheck<128, sizeof(Query)>{});
    #elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<136, sizeof(Query)>{});
    #endif
    static_assert(SizeCheck<8, alignof(Query)>{});
#elif __arm__
    static_assert(SizeCheck<80, sizeof(Query)>{});
    static_assert(SizeCheck<8, alignof(Query)>{});
#elif __aarch64__
#if defined(__clang__)
    static_assert(SizeCheck<128, sizeof(Query)>{});
#elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<136, sizeof(Query)>{});
#endif
    static_assert(SizeCheck<8, alignof(Query)>{});
#elif _WIN32
    static_assert(SizeCheck<8, alignof(Query)>{});
    #if _DEBUG
    static_assert(SizeCheck<160, sizeof(Query)>{});
    #else
    static_assert(SizeCheck<136, sizeof(Query)>{});
    #endif
#endif

    query::query() {
        new (&m_query) Query();
    }

    query::query(const query& other) {
        new (&m_query) Query(*reinterpret_cast<const Query*>(&other.m_query));
    }

    query& query::operator=(const query& other) {
        if (this != &other) {
            *reinterpret_cast<Query*>(&m_query) = *reinterpret_cast<const Query*>(&other.m_query);
        }
        return *this;
    }

    query::query(query&& other) {
        new (&m_query) Query(std::move(*reinterpret_cast<Query*>(&other.m_query)));
    }

    query& query::operator=(query&& other) {
        if (this != &other) {
            *reinterpret_cast<Query*>(&m_query) = std::move(*reinterpret_cast<Query*>(&other.m_query));
        }
        return *this;
    }

    query::~query() {
        reinterpret_cast<Query*>(&m_query)->~Query();
    }

    query::query(const table &table) {
        new (&m_query) Query(static_cast<ConstTableRef>(table));
    }

    query::query(const Query &v) {
        new (&m_query) Query(v);
    }

    query::operator Query() const {
        return *reinterpret_cast<const Query*>(&m_query);
    }
    table query::get_table() {
        return reinterpret_cast<Query*>(&m_query)->get_table();
    }
    query query::and_query(const query &v) {
        return reinterpret_cast<Query*>(&m_query)->and_query(v);
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

    query& query::equal(col_key column_key, std::nullopt_t) {
        *reinterpret_cast<Query *>(&m_query) = reinterpret_cast<Query *>(&m_query)->equal(column_key, realm::null{});
        return *this;
    }
    query& query::not_equal(col_key column_key, std::nullopt_t) {
        *reinterpret_cast<Query *>(&m_query) = reinterpret_cast<Query *>(&m_query)->not_equal(column_key, realm::null{});
        return *this;
    }

    query& query::negate() {
        *reinterpret_cast<Query *>(&m_query) = reinterpret_cast<Query *>(&m_query)->Not();
        return *this;
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