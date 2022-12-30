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
    class ColKey;
    class LnkLst;
    template <typename T>
    struct object_base;
    class NotificationToken;

    namespace internal::bridge {
        struct obj_key;
    }
    namespace internal::type_info {
        template <typename T, std::enable_if_t<std::is_base_of_v<object_base<T>, T>>>
        bridge::obj_key serialize(const T& o);
        template <typename, typename>
        struct type_info;
    }

    template <typename, typename>
    struct persisted;
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
    struct uuid;
    struct list;
    struct row;

    namespace {
        template <typename T>
        struct is_optional : std::false_type {
            using underlying = T;
        };

        template <typename T>
        struct is_optional<std::optional<T>> : std::true_type {
            using underlying = T;
        };
    }
    struct obj {
        obj(const Obj&); //NOLINT google-explicit-constructor
        operator Obj() const; //NOLINT google-explicit-constructor
        [[nodiscard]] table get_table() const noexcept;
        [[nodiscard]] table get_target_table(col_key) const noexcept;
        [[nodiscard]] bool is_null(const col_key& col_key) const;
        obj get_linked_object(const col_key& col_key);
        template <typename T>
        T get(const col_key& col_key) const {
            if constexpr (is_optional<T>::value) {
                if (is_null(col_key)) {
                    return std::nullopt;
                }
                return
                    persisted<typename T::value_type, void>::deserialize
                            (get<typename type_info::type_info<typename T::value_type, void>::internal_type>(col_key));
            } else {
                return get<T>(col_key);
            }
        }
        template <>
        [[nodiscard]] std::string get(const col_key& col_key) const;
        template <>
        [[nodiscard]] uuid get(const col_key& col_key) const;
        template <>
        [[nodiscard]] binary get(const col_key& col_key) const;
        template <>
        [[nodiscard]] timestamp get(const col_key& col_key) const;
        template <>
        [[nodiscard]] int64_t get(const col_key& col_key) const;
        template <>
        [[nodiscard]] bool get(const col_key& col_key) const;
        template <>
        [[nodiscard]] mixed get(const col_key& col_key) const;

#define __cpp_realm_generate_obj_set(type) \
        void set(const col_key& col_key, const type& value);

        __cpp_realm_generate_obj_set(int64_t)
        __cpp_realm_generate_obj_set(double)
        __cpp_realm_generate_obj_set(std::string)
        __cpp_realm_generate_obj_set(mixed)
        __cpp_realm_generate_obj_set(bool)
        __cpp_realm_generate_obj_set(timestamp)
        __cpp_realm_generate_obj_set(binary)
        __cpp_realm_generate_obj_set(uuid)
        __cpp_realm_generate_obj_set(obj_key)

        template <typename T>
        void set(const col_key& col_key, const T& value) {
            set(col_key, persisted<T, void>::serialize(value));
        }
        void set_list_values(const col_key& col_key, const std::vector<obj_key>& values);
        void set_list_values(const col_key& col_key, const std::vector<std::string>& values);
        void set_list_values(const col_key& col_key, const std::vector<bool>& values);
        void set_list_values(const col_key& col_key, const std::vector<int64_t>& values);
        void set_list_values(const col_key& col_key, const std::vector<internal::bridge::uuid>& values);
        void set_list_values(const col_key& col_key, const std::vector<binary>& values);
        void set_list_values(const col_key& col_key, const std::vector<mixed>& values);
        void set_list_values(const col_key& col_key, const std::vector<timestamp>& values);
        template <typename ValueType>
        void set_list_values(const col_key& col_key, const std::vector<ValueType>& values) {
            std::vector<typename internal::type_info::type_info<ValueType, void>::internal_type> v2;
            for (auto v : values) {
                v2.push_back(persisted<ValueType, void>::serialize(v));
            }
            set_list_values(col_key, v2);
        }

        [[nodiscard]] obj_key get_key() const;
        lnklst get_linklist(const col_key& col_key);
        void set_null(const col_key&);
        obj create_and_set_linked_object(const col_key&);
    private:
        unsigned char m_obj[64]{};
    };

    struct group {
        group(realm&);
        table get_table(int64_t table_key);
        table get_table(const std::string& table_key);
    private:
        std::reference_wrapper<realm> m_realm;
    };

    std::string table_name_for_object_type(const std::string&);
}


#endif //REALM_INTERNAL_OBJ_HPP
