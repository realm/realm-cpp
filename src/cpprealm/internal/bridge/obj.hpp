#ifndef REALM_INTERNAL_OBJ_HPP
#define REALM_INTERNAL_OBJ_HPP

#include <any>
#include <functional>
#include <string>
#include <cpprealm/internal/bridge/col_key.hpp>
#include <cpprealm/internal/bridge/object.hpp>
#include <cpprealm/internal/bridge/binary.hpp>
#include <cpprealm/scheduler.hpp>
#include <cpprealm/internal/type_info.hpp>

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
    class Dictionary;
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

    struct dict {
        class iterator {
        public:
            typedef std::forward_iterator_tag iterator_category;
            typedef std::pair<const Mixed, Mixed> value_type;
            typedef ptrdiff_t difference_type;
            typedef const value_type* pointer;
            typedef const value_type& reference;

            value_type operator*() const;
            iterator& operator++();
            iterator& operator+=(ptrdiff_t adj);
            iterator operator+(ptrdiff_t n) const;

            size_t get_position();

//        private:
            unsigned char m_iterator[304]{};
        };

        dict(const ::realm::Dictionary&);
        operator ::realm::Dictionary() const;

        std::pair<mixed, mixed> get_pair(size_t ndx) const;
        mixed get_key(size_t ndx) const;

        size_t size() const;
        bool is_null(size_t ndx) const;
        mixed get_any(size_t ndx) const;
        size_t find_any(mixed value) const;
        size_t find_any_key(mixed value) const noexcept;
        std::pair<iterator, bool> insert(const mixed& key, const mixed& value);
        template <typename T>
        std::pair<iterator, bool> insert(const mixed& key, const T& value) {
            return insert(key, mixed(static_cast<typename type_info::type_info<T>::internal_type>(value)));
        }
        std::pair<iterator, bool> insert(mixed key, const obj& obj);

        obj create_and_insert_linked_object(mixed key);

        // throws std::out_of_range if key is not found
        mixed get(mixed key) const;
        // Noexcept version
        std::optional<mixed> try_get(mixed key) const noexcept;
        // adds entry if key is not found
        const mixed operator[](mixed key);

        obj get_object(StringData key);
    private:
        unsigned char m_dictionary[128]{};
    };

    struct obj {
        obj();
        obj(const Obj&); //NOLINT google-explicit-constructor
        operator Obj() const; //NOLINT google-explicit-constructor
        [[nodiscard]] table get_table() const noexcept;
        [[nodiscard]] table get_target_table(col_key) const noexcept;
        [[nodiscard]] bool is_null(const col_key& col_key) const;
        obj get_linked_object(const col_key& col_key);
        dict get_dictionary(const col_key& col_key);
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
        [[nodiscard]] int get(const col_key& col_key) const {
            return get<int64_t>(col_key);
        }
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
            if constexpr (type_info::MixedPersistableConcept<T>::value) {
                std::visit([&col_key, this](auto&& arg) {
                    using M = std::decay_t<decltype(arg)>;
                    set(col_key, mixed(typename type_info::type_info<M>::internal_type(arg)));
                }, value);
            } else {
                set(col_key, typename type_info::type_info<T>::internal_type(value));
            }
        }
        template <typename T>
        void set(const col_key& col_key, const std::optional<T>& value) {
            if (!value) {
                set_null(col_key);
            } else {
                if constexpr (type_info::MixedPersistableConcept<T>::value) {
                    std::visit([this, &col_key](auto &&arg) {
                        set(col_key, arg);
                    }, *value);
                } else {
                    set(col_key, typename type_info::type_info<T, void>::internal_type(*value));
                }
            }
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

        template <typename T>
        std::vector<T> get_list_values(int64_t col_key);
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
