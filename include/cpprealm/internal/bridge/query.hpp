////////////////////////////////////////////////////////////////////////////
//
// Copyright 2024 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef CPPREALM_BRIDGE_QUERY_HPP
#define CPPREALM_BRIDGE_QUERY_HPP

#include <cpprealm/internal/bridge/col_key.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <optional>
#include <string_view>

namespace realm {
    struct object_id;
    struct decimal128;
    struct uuid;
    class LinkChain;
    class Query;
    class Subexpr;
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

    struct subexpr {
        subexpr(subexpr&& other) = default;
        subexpr& operator=(subexpr&& other) = default;
        ~subexpr() = default;
        subexpr(std::unique_ptr<Subexpr> other);

        query equal(const std::optional<int64_t>& rhs) const;
        query not_equal(const std::optional<int64_t>& rhs) const;
        query greater(const std::optional<int64_t>& rhs) const;
        query less(const std::optional<int64_t>& rhs) const;
        query greater_equal(const std::optional<int64_t>& rhs) const;
        query less_equal(const std::optional<int64_t>& rhs) const;

        query equal(const std::optional<bool>& rhs) const;
        query not_equal(const std::optional<bool>& rhs) const;

        query equal(const std::optional<double>& rhs) const;
        query not_equal(const std::optional<double>& rhs) const;
        query greater(const std::optional<double>& rhs) const;
        query less(const std::optional<double>& rhs) const;
        query greater_equal(const std::optional<double>& rhs) const;
        query less_equal(const std::optional<double>& rhs) const;

        query equal(const std::optional<binary>& rhs) const;
        query not_equal(const std::optional<binary>& rhs) const;

        query equal(const std::optional<timestamp>& rhs) const;
        query not_equal(const std::optional<timestamp>& rhs) const;
        query greater(const std::optional<timestamp>& rhs) const;
        query less(const std::optional<timestamp>& rhs) const;
        query greater_equal(const std::optional<timestamp>& rhs) const;
        query less_equal(const std::optional<timestamp>& rhs) const;

        query equal(const std::optional<std::string>& rhs) const;
        query not_equal(const std::optional<std::string>& rhs) const;
        query contains(const std::optional<std::string>& rhs, bool case_sensitive = true) const;

        query equal(const std::optional<internal::bridge::uuid>& rhs) const;
        query not_equal(const std::optional<internal::bridge::uuid>& rhs) const;

        query equal(const std::optional<internal::bridge::object_id>& rhs) const;
        query not_equal(const std::optional<internal::bridge::object_id>& rhs) const;

        query equal(const std::optional<internal::bridge::decimal128>& rhs) const;
        query not_equal(const std::optional<internal::bridge::decimal128>& rhs) const;
        query greater(const std::optional<internal::bridge::decimal128>& rhs) const;
        query less(const std::optional<internal::bridge::decimal128>& rhs) const;
        query greater_equal(const std::optional<internal::bridge::decimal128>& rhs) const;
        query less_equal(const std::optional<internal::bridge::decimal128>& rhs) const;

        query mixed_equal(const internal::bridge::mixed& rhs) const;
        query mixed_not_equal(const internal::bridge::mixed& rhs) const;

        query equal(const std::optional<obj>&) const;
        query not_equal(const std::optional<obj>&) const;

    private:
        std::shared_ptr<Subexpr> m_subexpr;
    };

    struct link_chain {
        link_chain();
        link_chain(const link_chain& other) ;
        link_chain& operator=(const link_chain& other) ;
        link_chain(link_chain&& other);
        link_chain& operator=(link_chain&& other);
        ~link_chain() = default;
        link_chain(const LinkChain& other);

        link_chain& link(col_key);
        link_chain& link(std::string col_name);
        link_chain& backlink(const table& origin, col_key origin_col_key);

        template<typename>
        subexpr column(col_key);
        subexpr column_mixed(col_key);

        subexpr subquery(query subquery);
        table get_table();

    private:
        std::shared_ptr<LinkChain> m_link_chain;
    };

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

        // Conditions: links
        query& links_to(col_key column_key, const internal::bridge::obj& o);
        query& not_links_to(col_key column_key, const internal::bridge::obj& o);

        query& dictionary_has_value_for_key_equals(col_key column_key, const std::string& key, const mixed& value);
        query& dictionary_has_value_for_key_not_equals(col_key column_key, const std::string& key, const mixed& value);
        query& dictionary_has_value_for_key_greater_than(col_key column_key, const std::string& key, const mixed& value);
        query& dictionary_has_value_for_key_less_than(col_key column_key, const std::string& key, const mixed& value);
        query& dictionary_has_value_for_key_greater_than_equals(col_key column_key, const std::string& key, const mixed& value);
        query& dictionary_has_value_for_key_less_than_equals(col_key column_key, const std::string& key, const mixed& value);
        query& dictionary_contains_string_for_key(col_key column_key, const std::string& key, const std::string& value);
        query& dictionary_contains_key(col_key column_key, const std::string& key);
        subexpr dictionary_link_subexpr(col_key column_key, col_key link_column_key, const std::string& key);

        // Expressions
        static query falsepredicate();

        std::string description() const;
    private:
        inline Query* get_query();
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::Query m_query[1];
#else
        std::shared_ptr<Query> m_query;
#endif

    };

    query operator || (const query& lhs, const query& rhs);
}

#endif //CPPREALM_BRIDGE_QUERY_HPP
