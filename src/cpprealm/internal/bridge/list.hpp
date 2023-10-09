#ifndef CPP_REALM_BRIDGE_LIST_HPP
#define CPP_REALM_BRIDGE_LIST_HPP

#include <cstdlib>
#include <string>
#include <memory>
#include <optional>

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
    struct object_id;
    struct decimal128;
    struct col_key;
    struct timestamp;
    struct table;
    struct notification_token;
    struct collection_change_callback;

    struct list {
        list();
        list(const list& other) ;
        list& operator=(const list& other) ;
        list(list&& other);
        list& operator=(list&& other);
        ~list();
        list(const List&); //NOLINT(google-explicit-constructor)
        operator List() const; //NOLINT(google-explicit-constructor)
        list(const realm& realm, const obj& obj, const col_key&);

        [[nodiscard]] size_t size() const;
        void remove(size_t idx);
        void remove_all();

        table get_table() const;

        void add(const std::string&);
        void add(const int64_t &);
        void add(const double &);
        void add(const bool &);
        void add(const binary &);
        void add(const uuid &);
        void add(const object_id &);
        void add(const decimal128 &);
        void add(const mixed &);
        void add(const obj_key &);
        void add(const timestamp &);
        obj add_embedded();

        void set(size_t pos, const int64_t &);
        void set(size_t pos, const double &);
        void set(size_t pos, const bool &);
        void set(size_t pos, const std::string &);
        void set(size_t pos, const uuid &);
        void set(size_t pos, const object_id &);
        void set(size_t pos, const decimal128 &);
        void set(size_t pos, const mixed &);
        void set(size_t pos, const timestamp &);
        void set(size_t pos, const binary &);

        size_t find(const int64_t &);
        size_t find(const bool &);
        size_t find(const double &);
        size_t find(const std::string &);
        size_t find(const uuid &);
        size_t find(const object_id &);
        size_t find(const decimal128 &);
        size_t find(const mixed &);
        size_t find(const timestamp &);
        size_t find(const binary&);
        size_t find(const obj_key&);
        notification_token add_notification_callback(std::shared_ptr<collection_change_callback>);
    private:
        template <typename ValueType>
        friend ValueType get(const list&, size_t idx);
        friend inline List* get_list(const list& lst);
        friend inline const List* get_list_const(const list& lst);
        inline ::realm::List* get_list();
        inline const ::realm::List* get_list_const() const;
//#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
//        storage::List m_list[1];
//#else
        std::shared_ptr<::realm::List> m_list;
//#endif
    };

    template <typename ValueType>
    [[nodiscard]] ValueType get(const list&, size_t idx);
    template <>
    [[nodiscard]] std::string get(const list&, size_t idx);
    template <>
    [[nodiscard]] int64_t get(const list&, size_t idx);
    template <>
    [[nodiscard]] double get(const list&, size_t idx);
    template <>
    [[nodiscard]] binary get(const list&, size_t idx);
    template <>
    [[nodiscard]] uuid get(const list&, size_t idx);
    template <>
    [[nodiscard]] object_id get(const list&, size_t idx);
    template <>
    [[nodiscard]] decimal128 get(const list&, size_t idx);
    template <>
    [[nodiscard]] mixed get(const list&, size_t idx);
    template <>
    [[nodiscard]] obj get(const list&, size_t idx);

    template <>
    [[nodiscard]] std::optional<int64_t> get(const list& lst, size_t idx);
    template <>
    [[nodiscard]] std::optional<double> get(const list& lst, size_t idx);
    template <>
    [[nodiscard]] std::optional<bool> get(const list& lst, size_t idx);
    template <>
    [[nodiscard]] std::optional<uuid> get(const list& lst, size_t idx);
    template <>
    [[nodiscard]] std::optional<object_id> get(const list& lst, size_t idx);
    template <>
    [[nodiscard]] std::optional<std::string> get(const list& lst, size_t idx);
    template <>
    [[nodiscard]] std::optional<binary> get(const list& lst, size_t idx);
    template <>
    [[nodiscard]] std::optional<timestamp> get(const list& lst, size_t idx);
}

#endif //CPP_REALM_BRIDGE_LIST_HPP
