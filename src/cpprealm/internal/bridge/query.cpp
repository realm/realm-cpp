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

    query& query::equal(col_key column_key, std::nullopt_t) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<Query *>(&m_query) = reinterpret_cast<Query *>(&m_query)->equal(column_key, realm::null{});
#else
        m_query = std::make_shared<Query>(m_query->equal(column_key, realm::null{}));
#endif
        return *this;
    }
    query& query::not_equal(col_key column_key, std::nullopt_t) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<Query *>(&m_query) = reinterpret_cast<Query *>(&m_query)->not_equal(column_key, realm::null{});
#else
        m_query = std::make_shared<Query>(m_query->not_equal(column_key, realm::null{}));
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
