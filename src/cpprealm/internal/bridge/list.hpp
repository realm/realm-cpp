#ifndef CPP_REALM_BRIDGE_LIST_HPP
#define CPP_REALM_BRIDGE_LIST_HPP

#include <cstdlib>
#include <string>

namespace realm {
    class List;
    template <typename>
    struct object;
    template <typename>
    struct embedded_object;
    template <typename, typename>
    struct persisted;
}

namespace realm::internal::bridge {
    struct realm;
    struct obj;
    struct obj_key;
    struct mixed;
    struct binary;
    struct uuid;
    struct col_key;
    struct timestamp;
    struct table;
    struct notification_token;
    struct collection_change_callback;

    struct list {
        list();
        list(const List&); //NOLINT(google-explicit-constructor)
        operator List() const; //NOLINT(google-explicit-constructor)
        list(const realm& realm, const obj& obj, const col_key&);

        [[nodiscard]] size_t size() const;
        void remove(size_t idx);
        void remove_all();

        table get_table() const;

        void add(const std::string&);
        void add(const int64_t &);
        void add(const bool &);
        void add(const binary &);
        void add(const uuid &);
        void add(const mixed &);
        void add(const obj_key &);
        void add(const timestamp &);
        template <typename ValueType>
        void add(const ValueType& v) {
            add(persisted<ValueType, void>::serialize(v));
        }
        obj add_embedded();

        template <typename ValueType>
        ValueType get(size_t idx) const;
        template <>
        [[nodiscard]] std::string get(size_t idx) const;
        template <>
        [[nodiscard]] int64_t get(size_t idx) const;
        template <>
        [[nodiscard]] binary get(size_t idx) const;
        template <>
        [[nodiscard]] uuid get(size_t idx) const;
        template <>
        [[nodiscard]] mixed get(size_t idx) const;
        template <>
        [[nodiscard]] obj get(size_t idx) const;

        [[nodiscard]] realm get_realm() const;


        void set(size_t pos, const int64_t &);
        void set(size_t pos, const bool &);
        void set(size_t pos, const std::string &);
        void set(size_t pos, const double &);
        void set(size_t pos, const uuid &);
        void set(size_t pos, const mixed &);
        void set(size_t pos, const timestamp &);
        void set(size_t pos, const binary &);
        template <typename ValueType>
        void set(size_t pos, const ValueType& v) {
            set(pos, persisted<ValueType, void>::serialize(v));
        }

        size_t find(const int64_t &);
        size_t find(const bool &);
        size_t find(const double &);
        size_t find(const std::string &);
        size_t find(const uuid &);
        size_t find(const mixed &);
        size_t find(const timestamp &);
        size_t find(const binary&);
        size_t find(const obj_key&);
        template <typename ValueType>
        size_t find(const ValueType&v) {
            return find(persisted<ValueType, void>::serialize(v));
        }
        notification_token add_notification_callback(std::shared_ptr<collection_change_callback>);
    private:
        unsigned char m_list[80]{};
    };
}

#endif //CPP_REALM_BRIDGE_LIST_HPP
