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

#include <string>
#include <vector>
#include <cpprealm/internal/bridge/obj_key.hpp>

namespace realm {
    class TableRef;
    class ConstTableRef;
    class Mixed;
    class TableView;


    namespace internal::bridge {
        struct col_key;
        struct link_chain;
        struct mixed;
        struct obj;
        struct query;

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
