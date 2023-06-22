#ifndef CPP_REALM_BRIDGE_QUERY_HPP
#define CPP_REALM_BRIDGE_QUERY_HPP

#include <cpprealm/internal/bridge/col_key.hpp>

#include <string_view>
#include <optional>

namespace realm {
    struct object_id;
    struct uuid;
    class Query;
}
namespace realm::internal::bridge {
    struct table;
    struct col_key;
    struct timestamp;
    struct binary;
    struct object_id;
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
    private:
#ifdef __i386__
        std::aligned_storage<68, 4>::type m_query[1];
#elif __x86_64__
    #if defined(__clang__)
        std::aligned_storage<128, 8>::type m_query[1];
    #elif defined(__GNUC__) || defined(__GNUG__)
        std::aligned_storage<136, 8>::type m_query[1];
    #endif
#elif __arm__
        std::aligned_storage<80, 8>::type m_query[1];
#elif __aarch64__
#if defined(__clang__)
        std::aligned_storage<128, 8>::type m_query[1];
#elif defined(__GNUC__) || defined(__GNUG__)
        std::aligned_storage<136, 8>::type m_query[1];
#endif
#elif _WIN32
        std::aligned_storage<160, 8>::type m_query[1];
#endif

    };

    template <typename T>
    using QFn = query& (query::*)(col_key, T);
    template <typename T>
    using QFnCS = query& (query::*)(col_key, T, bool);

    query operator || (const query& lhs, const query& rhs);
}

#endif //CPP_REALM_BRIDGE_QUERY_HPP
