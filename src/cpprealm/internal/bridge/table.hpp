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

#ifndef CPPREALM_BRIDGE_TABLE_HPP
#define CPPREALM_BRIDGE_TABLE_HPP

#include <optional>
#include <string>
#include <vector>
#include <cpprealm/internal/bridge/obj_key.hpp>

namespace realm {
    class TableRef;
    class ConstTableRef;
    class Mixed;
    class TableView;
    class LinkChain;
    class Subexpr;

    namespace internal::bridge {
        struct obj;
        struct mixed;
        struct col_key;
        struct query;
        struct table;
        struct uuid;
        struct object_id;
        struct decimal128;
        struct binary;
        struct timestamp;

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

            subexpr subquery(query subquery);
            table get_table();

            std::shared_ptr<LinkChain> m_link_chain;
        };

        struct table {
            table();
            table(const table& other) ;
            table& operator=(const table& other) ;
            table(table&& other);
            table& operator=(table&& other);
            ~table();
            table(const TableRef &);
            table(const ConstTableRef &);
            operator TableRef() const;
            operator ConstTableRef() const;

            col_key get_column_key(const std::string_view &name) const;
            uint32_t get_key() const;
            std::string get_name() const;

            obj create_object_with_primary_key(const mixed &key) const;

            obj create_object(const obj_key &obj_key = {}) const;

            table get_link_target(const col_key col_key) const;
            link_chain get_link(const col_key col_key) const;

            [[nodiscard]] bool is_embedded() const;

            struct query query(const std::string &, const std::vector <mixed>&) const;
            struct query where() const;

            void remove_object(const obj_key &) const;
            obj get_object(const obj_key&) const;
            bool is_valid(const obj_key&) const;
            using underlying = TableRef;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::TableRef m_table[1];
#else
        std::shared_ptr<TableRef> m_table;
#endif
        };

        struct table_view {
            table_view();
            table_view(const table_view& other) ;
            table_view& operator=(const table_view& other) ;
            table_view(table_view&& other);
            table_view& operator=(table_view&& other);
            ~table_view();
            table_view(const TableView &);
            operator TableView() const;
            using underlying = TableView;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
            storage::TableView m_table_view[1];
#else
            std::shared_ptr<TableView> m_table_view;
#endif
        };

        bool operator==(const table &, const table &);

        bool operator!=(const table &, const table &);
    }
}

#endif //CPPREALM_BRIDGE_TABLE_HPP
