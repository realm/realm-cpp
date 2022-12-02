#ifndef CPP_REALM_BRIDGE_QUERY_HPP
#define CPP_REALM_BRIDGE_QUERY_HPP

#include <string_view>

namespace realm::internal::bridge {
    struct table;
    struct col_key;
    struct timestamp;

    struct query {
        query(const table& table);
        table get_table();
        query and_query(const query&);
        template <typename T>
        query equal(const col_key& col_key, const T& value);
        template <typename T>
        query not_equal(const col_key& col_key, const T& value);
        template <typename T>
        query greater(const col_key& col_key, const T& value);
        template <typename T>
        query greater_equal(const col_key& col_key, const T& value);
        template <typename T>
        query less(const col_key& col_key, const T& value);
        template <typename T>
        query less_equal(const col_key& col_key, const T& value);
        template <typename T>
        query contains(const col_key& col_key, const T& value);
    private:
        void* m_query;
    };

    query operator || (const query& lhs, const query& rhs);

    template <>
    query query::equal(const col_key& col_key, const std::string_view &value);
    template <>
    query query::equal(const col_key& col_key, const std::string &value);
    template <>
    query query::equal(const col_key& col_key, const timestamp &value);
}

#endif //CPP_REALM_BRIDGE_QUERY_HPP
