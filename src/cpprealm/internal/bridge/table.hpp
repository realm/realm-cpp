#ifndef CPP_REALM_BRIDGE_TABLE_HPP
#define CPP_REALM_BRIDGE_TABLE_HPP

#include <string>
#include <cpprealm/internal/bridge/obj_key.hpp>

namespace realm {
    class TableRef;
    class ConstTableRef;

    namespace internal::bridge {
        struct obj;
        struct mixed;
        struct col_key;
        struct query;

        struct table {
            table(const TableRef &);
            table(const ConstTableRef &);
            col_key get_column_key(const std::string &name);

            obj create_object_with_primary_key(const mixed &key);

            obj create_object(const obj_key &obj_key = {});

            table get_link_target(const col_key col_key);

            [[nodiscard]] bool is_embedded() const;

            query query(const std::string &, std::vector <mixed>);

            void remove_object(const obj_key &);

        private:
            unsigned char m_table[16]{};
        };

        bool operator==(const table &, const table &);

        bool operator!=(const table &, const table &);
    }
}

#endif //CPP_REALM_BRIDGE_TABLE_HPP
