#ifndef CPP_REALM_BRIDGE_TABLE_HPP
#define CPP_REALM_BRIDGE_TABLE_HPP

#include <string>
#include <vector>
#include <cpprealm/internal/bridge/obj_key.hpp>

namespace realm {
    class TableRef;
    class ConstTableRef;
    class Mixed;
    class TableView;

    namespace internal::bridge {
        struct obj;
        struct mixed;
        struct col_key;
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

            obj create_object_with_primary_key(const mixed &key) const;

            obj create_object(const obj_key &obj_key = {}) const;

            table get_link_target(const col_key col_key) const;

            [[nodiscard]] bool is_embedded() const;

            struct query query(const std::string &, const std::vector <mixed>&) const;

            void remove_object(const obj_key &) const;
            obj get_object(const obj_key&) const;
            bool is_valid(const obj_key&) const;
            using underlying = TableRef;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::TableRef m_table[1];
#elif __i386__
         std::aligned_storage<12, 4>::type m_table[1];
#elif __x86_64__
        std::aligned_storage<16, 8>::type m_table[1];
#elif __arm__
        std::aligned_storage<16, 8>::type m_table[1];
#elif __aarch64__
        std::aligned_storage<16, 8>::type m_table[1];
#elif _WIN32
        std::aligned_storage<16, 8>::type m_table[1];
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
#elif __i386__
            std::aligned_storage<316, 4>::type m_table_view[1];
#elif __x86_64__
#if defined(__clang__)
            std::aligned_storage<568, 8>::type m_table_view[1];
#elif defined(__GNUC__) || defined(__GNUG__)
            std::aligned_storage<576, 8>::type m_table_view[1];
#endif
#elif __arm__
            std::aligned_storage<368, 8>::type m_table_view[1];
#elif __aarch64__
#if defined(__clang__)
            std::aligned_storage<568, 8>::type m_table_view[1];
#elif defined(__GNUC__) || defined(__GNUG__)
            std::aligned_storage<624, 8>::type m_table_view[1];
#endif
#elif _WIN32
            #if _DEBUG
            std::aligned_storage<624, 8>::type m_table_view[1];
            #else
            std::aligned_storage<576, 8>::type m_table_view[1];
            #endif
#endif
        };

        bool operator==(const table &, const table &);

        bool operator!=(const table &, const table &);
    }
}

#endif //CPP_REALM_BRIDGE_TABLE_HPP
