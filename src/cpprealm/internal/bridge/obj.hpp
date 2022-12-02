#ifndef REALM_INTERNAL_OBJ_HPP
#define REALM_INTERNAL_OBJ_HPP

#include <any>
#include <functional>
#include <string>
#include <cpprealm/internal/bridge/col_key.hpp>
#include <cpprealm/internal/bridge/object.hpp>
#include <cpprealm/internal/bridge/binary.hpp>
#include <cpprealm/scheduler.hpp>

namespace realm {
    class Group;
    class Realm;
    class Timestamp;
    class Object;
    class ObjectSchema;
    class Obj;
    class TableRef;
    class Query;
    class uuid;
    class ColKey;
}

namespace realm::internal::bridge {
    struct obj;
    struct mixed;
    struct realm;
    struct lnklst;
    struct query;
    struct object_schema;
    struct timestamp;
    struct obj_key;
    struct table;
    struct dictionary;

    struct obj {
        obj(Obj&&);
        table get_table();
        bool is_null(const col_key& col_key);
        obj get_linked_object(const col_key& col_key);
        template <typename T>
        T get(const col_key& col_key) const;

        template <typename T>
        void set(const col_key& col_key, const T& value);
        void set_any(const col_key& col_key, const mixed& value);
        template <typename ValueType>
        void set_list_values(const col_key& col_key, const std::vector<ValueType>& values);

        template <typename T>
        std::vector<T> get_list_values(int64_t col_key);
        dictionary get_dictionary(const col_key& col_key);
        dictionary get_dictionary(const std::string& property_name);
        obj_key get_key() const;
        lnklst get_linklist(const col_key& col_key);
        void set_null(const col_key&);
        obj create_and_set_linked_object(const col_key&);
    private:
        unsigned char m_obj[64]{};
    };

    template <>
    std::string obj::get(const col_key& col_key) const;
    template <>
    uuid obj::get(const col_key& col_key) const;
    template <>
    binary obj::get(const col_key& col_key) const;
    template <>
    timestamp obj::get(const col_key& col_key) const;

    template <>
    void obj::set(const col_key& col_key, const int64_t& value);

    struct lnklst {
        obj create_and_insert_linked_object(size_t idx);
        void add(const obj_key&);
    };

    struct list {
        list() = default;
        list(const realm& realm, const obj& obj, int64_t col_key);
        size_t size() const;
        void remove(size_t idx);
        void remove_all();
        template <typename ValueType>
        void add(const ValueType&);
        template <typename ValueType>
        ValueType get(size_t idx) const;
        size_t find(const obj_key&);
        size_t find(const mixed&);
        realm get_realm() const;
        obj add_embedded();
        template <typename ValueType>
        void set(size_t pos, const ValueType&);
        template <typename ValueType>
        size_t find(const ValueType&);
    private:
        unsigned char m_list[32];
    };

    template<> int64_t list::get(size_t idx) const;


    struct notification_token {
    private:
        unsigned char m_token[32];
    };

    struct group {
        group(realm&);
        table get_table(int64_t table_key);
        table get_table(const std::string& table_key);
    private:
        std::reference_wrapper<realm> m_realm;
    };

    struct thread_safe_reference {
        thread_safe_reference(const object&);
        operator bool() const;
    private:
        unsigned char m_thread_safe_reference[32];
    };

    std::string table_name_for_object_type(const std::string&);
}


#endif //REALM_INTERNAL_OBJ_HPP
