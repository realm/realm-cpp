#ifndef CPP_REALM_BRIDGE_QUERY_HPP
#define CPP_REALM_BRIDGE_QUERY_HPP

#include <cpprealm/internal/bridge/col_key.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <optional>
#include <string_view>

namespace realm {
    struct object_id;
    struct decimal128;
    struct uuid;
    class Query;
}
namespace realm::internal::bridge {
    struct table;
    struct col_key;
    struct timestamp;
    struct binary;
    struct object_id;
    struct decimal128;
    struct uuid;
    struct mixed;


    struct query {
        query();
        query(const query& other) ;
        query& operator=(const query& other) ;
        query(query&& other);
        query& operator=(query&& other);
        ~query();
        query(const table& table); //NOLINT(google-explicit-constructor)
        table get_table();
        query and_query(const query&);
        query& negate();

        query(const Query&); //NOLINT(google-explicit-constructor)
        operator Query() const; //NOLINT(google-explicit-constructor)

        // Conditions: null
        query& equal(col_key column_key, std::nullopt_t);
        query& not_equal(col_key column_key, std::nullopt_t);

        query& equal(col_key column_key, int64_t value);
        query& not_equal(col_key column_key, int64_t value);
        query& greater(col_key column_key, int64_t value);
        query& greater_equal(col_key column_key, int64_t value);
        query& less(col_key column_key, int64_t value);
        query& less_equal(col_key column_key, int64_t value);
        query& between(col_key column_key, int64_t from, int64_t to);

        // Conditions: double
        query& equal(col_key column_key, double value);
        query& not_equal(col_key column_key, double value);
        query& greater(col_key column_key, double value);
        query& greater_equal(col_key column_key, double value);
        query& less(col_key column_key, double value);
        query& less_equal(col_key column_key, double value);
        query& between(col_key column_key, double from, double to);

        // Conditions: timestamp
        query& equal(col_key column_key, timestamp value);
        query& not_equal(col_key column_key, timestamp value);
        query& greater(col_key column_key, timestamp value);
        query& greater_equal(col_key column_key, timestamp value);
        query& less_equal(col_key column_key, timestamp value);
        query& less(col_key column_key, timestamp value);

        // Conditions: UUID
        query& equal(col_key column_key, uuid value);
        query& not_equal(col_key column_key, uuid value);
        query& greater(col_key column_key, uuid value);
        query& greater_equal(col_key column_key, uuid value);
        query& less_equal(col_key column_key, uuid value);
        query& less(col_key column_key, uuid value);

        // Conditions: ObjectId
        query& equal(col_key column_key, object_id value);
        query& not_equal(col_key column_key, object_id value);
        query& greater(col_key column_key, object_id value);
        query& greater_equal(col_key column_key, object_id value);
        query& less_equal(col_key column_key, object_id value);
        query& less(col_key column_key, object_id value);

        // Conditions: Decimal128
        query& equal(col_key column_key, decimal128 value);
        query& not_equal(col_key column_key, decimal128 value);
        query& greater(col_key column_key, decimal128 value);
        query& greater_equal(col_key column_key, decimal128 value);
        query& less_equal(col_key column_key, decimal128 value);
        query& less(col_key column_key, decimal128 value);

        // Conditions: string
        query& equal(col_key column_key, std::string_view value, bool case_sensitive = true);
        query& not_equal(col_key column_key, std::string_view value, bool case_sensitive = true);
        query& begins_with(col_key column_key, const std::string& value, bool case_sensitive = true);
        query& ends_with(col_key column_key, const std::string& value, bool case_sensitive = true);
        query& contains(col_key column_key, std::string_view value, bool case_sensitive = true);
        query& like(col_key column_key, const std::string& value, bool case_sensitive = true);

        // Conditions: binary
        query& equal(col_key column_key, binary value, bool case_sensitive = true);
        query& not_equal(col_key column_key, binary value, bool case_sensitive = true);
        query& begins_with(col_key column_key, binary value, bool case_sensitive = true);
        query& ends_with(col_key column_key, binary value, bool case_sensitive = true);
        query& contains(col_key column_key, binary value, bool case_sensitive = true);
        query& like(col_key column_key, binary b, bool case_sensitive = true);

        // Conditions: Mixed
        query& equal(col_key column_key, mixed value, bool case_sensitive = true);
        query& not_equal(col_key column_key, mixed value, bool case_sensitive = true);
        query& greater(col_key column_key, mixed value);
        query& greater_equal(col_key column_key, mixed value);
        query& less(col_key column_key, mixed value);
        query& less_equal(col_key column_key, mixed value);
        query& begins_with(col_key column_key, mixed value, bool case_sensitive = true);
        query& ends_with(col_key column_key, mixed value, bool case_sensitive = true);
        query& contains(col_key column_key, mixed value, bool case_sensitive = true);
        query& like(col_key column_key, mixed value, bool case_sensitive = true);

        // Conditions: bool
        query& equal(col_key column_key, bool value);
        query& not_equal(col_key column_key, bool value);
        using underlying = Query;

        std::string description() const;
    private:
        inline Query* get_query();
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::Query m_query[1];
#else
        std::shared_ptr<Query> m_query;
#endif

    };

    template <typename T>
    using QFn = query& (query::*)(col_key, T);
    template <typename T>
    using QFnCS = query& (query::*)(col_key, T, bool);

    query operator || (const query& lhs, const query& rhs);
}

#endif //CPP_REALM_BRIDGE_QUERY_HPP
