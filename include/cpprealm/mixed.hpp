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

#ifndef CPPREALM_MIXED_HPP
#define CPPREALM_MIXED_HPP

#include <optional>
#include <map>

#include <cpprealm/internal/bridge/col_key.hpp>
#include <cpprealm/internal/bridge/dictionary.hpp>
#include <cpprealm/internal/bridge/list.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/object.hpp>

namespace realm {
    struct db;
    struct uuid;
    struct object_id;
    struct mixed;
    struct decimal128;

    template<typename T, typename>
    struct managed;

    namespace internal {
        internal::bridge::mixed get_mixed_bridge(const mixed&);
    }

    struct context {
        internal::bridge::dictionary create_nested_dictionary(internal::bridge::dictionary& d, const std::string& key);
        internal::bridge::dictionary create_nested_dictionary(internal::bridge::list& d, size_t idx);

        internal::bridge::list create_nested_list(internal::bridge::dictionary& d, const std::string& key);
        internal::bridge::list create_nested_list(internal::bridge::list& d, size_t idx);

        void insert_dictionary_value(internal::bridge::dictionary& d, const std::string& key,  const mixed& values);
        void insert_dictionary_values(internal::bridge::dictionary& d, const std::map<std::string, mixed>& values);

        void set_list_value(internal::bridge::list& d, size_t idx, const mixed& values);
        void add_list_value(internal::bridge::list& d, const mixed& values);
        void insert_list_values(internal::bridge::list& d, const std::vector<mixed>& values);

        void set_managed(internal::bridge::obj& obj,
                         const internal::bridge::col_key& key,
                         const internal::bridge::realm& realm,
                         const realm::mixed& value);
    };

    struct mixed {
        struct vector {
            vector(const std::vector<mixed>& o);
            vector(const internal::bridge::list& o, std::optional<internal::bridge::col_key> ck = std::nullopt);
            vector& operator=(const std::vector<mixed>& o);
            void push_back(const realm::mixed&);
            void pop_back();
            void set(const realm::mixed&, size_t);
            void erase(size_t);
            class iterator {
            public:
                using iterator_category = std::input_iterator_tag;
                bool operator!=(const iterator& other) const;
                bool operator==(const iterator& other) const;
                mixed operator*() noexcept;
                iterator& operator++();
                const iterator& operator++(int i);

                iterator(size_t i, const std::vector<mixed>& o);
                iterator(size_t i, const internal::bridge::list& o, internal::bridge::col_key ck);
                ~iterator();
            private:
                friend struct map;

                union {
                    std::vector<mixed> m_unmanaged_vector;
                    internal::bridge::list m_managed_vector;
                };
                std::optional<internal::bridge::col_key> m_col_key;
                bool m_is_managed = false;

                size_t m_i;
            };

            size_t size() const;
            iterator begin() const;
            iterator end() const;
            mixed operator[] (size_t i);
            operator std::vector<mixed>();
            ~vector();

        private:
            union {
                std::vector<mixed> m_unmanaged_vector;
                internal::bridge::list m_managed_vector;
            };
            std::optional<internal::bridge::col_key> m_col_key;
            bool m_is_managed = false;
        };

        struct map {

            struct box {
                box(std::optional<std::map<std::string, mixed>> map, const std::string& key);
                box(std::optional<internal::bridge::dictionary> map, const std::string& key, internal::bridge::col_key c);

                box& operator=(const mixed&);
                box& operator=(mixed&& m);
                mixed operator *();
            private:
                bool m_is_managed = false;
                std::string m_key;
                std::optional<std::map<std::string, mixed>> m_unmanaged_map;
                std::optional<internal::bridge::dictionary> m_managed_map;
                std::optional<internal::bridge::col_key> m_origin_col_key;
            };

            map(const std::map<std::string, mixed>& o);
            map(const internal::bridge::dictionary& o, std::optional<internal::bridge::col_key> ck = std::nullopt);

            map& operator=(const std::map<std::string, mixed>& o);

            class iterator {
            public:
                using iterator_category = std::input_iterator_tag;

                bool operator!=(const iterator& other) const;

                bool operator==(const iterator& other) const;

                std::pair<std::string, box> operator*() noexcept;

                iterator& operator++();
                const iterator& operator++(int i);
                iterator(size_t i, const std::map<std::string, mixed>& o);
                iterator(size_t i, const internal::bridge::dictionary& o, internal::bridge::col_key ck);

                ~iterator();
            private:
                friend struct map;

                union {
                    std::map<std::string, mixed> m_unmanaged_map;
                    internal::bridge::dictionary m_managed_map;
                };
                std::optional<internal::bridge::col_key> m_col_key;
                bool m_is_managed = false;

                iterator(size_t i) : m_i(i)
                {
                }
                size_t m_i;
            };

            size_t size() const;

            iterator begin() const;
            iterator end() const;
            iterator find(const std::string&);

            box operator[] (const std::string& k);

            operator std::map<std::string, mixed>();

            ~map();

        private:
            union {
                std::map<std::string, mixed> m_unmanaged_map;
                internal::bridge::dictionary m_managed_map;
            };
            std::optional<internal::bridge::col_key> m_col_key;
            bool m_is_managed = false;
        };

        mixed();

        mixed(const internal::bridge::mixed& m);

        mixed(internal::bridge::mixed&& m, internal::bridge::object&& o, internal::bridge::col_key c);

        mixed(const mixed& other) {
            switch (other.m_storage_mode) {
                case storage_mode::uninitialized:
                    break;
                case storage_mode::mixed_bridge:
                    new (&m_mixed) internal::bridge::mixed(other.m_mixed);
                    break;
                case storage_mode::managed:
                    new (&m_mixed) internal::bridge::mixed(other.m_mixed);
                    break;
                case storage_mode::unmanaged_dictionary:
                    new (&m_unmanaged_map) std::map<std::string, mixed>(other.m_unmanaged_map);
                    break;
                case storage_mode::vector:
                    new (&m_unmanaged_vector) std::vector<mixed>(other.m_unmanaged_vector);
                    break;
            }
            m_storage_mode = other.m_storage_mode;
        };

        mixed(mixed&& other) {
            switch (other.m_storage_mode) {
                case storage_mode::uninitialized:
                    break;
                case storage_mode::mixed_bridge:
                    new (&m_mixed) internal::bridge::mixed(std::move(other.m_mixed));
                    break;
                case storage_mode::managed:
                    new (&m_mixed) internal::bridge::mixed(std::move(other.m_mixed));
                    break;
                case storage_mode::unmanaged_dictionary:
                    new (&m_unmanaged_map) std::map<std::string, mixed>(std::move(other.m_unmanaged_map));
                    break;
                case storage_mode::vector:
                    new (&m_unmanaged_vector) std::vector<mixed>(std::move(other.m_unmanaged_vector));
                    break;
            }
            m_storage_mode = std::move(other.m_storage_mode);
        };

        mixed& operator=(const mixed& other) {
            switch (other.m_storage_mode) {
                case storage_mode::uninitialized:
                    break;
                case storage_mode::mixed_bridge:
                    new (&m_mixed) internal::bridge::mixed(other.m_mixed);
                    break;
                case storage_mode::managed:
                    new (&m_mixed) internal::bridge::mixed(other.m_mixed);
                    break;
                case storage_mode::unmanaged_dictionary:
                    new (&m_unmanaged_map) std::map<std::string, mixed>(other.m_unmanaged_map);
                    break;
                case storage_mode::vector:
                    new (&m_unmanaged_vector) std::vector<mixed>(other.m_unmanaged_vector);
                    break;
            }
            m_storage_mode = other.m_storage_mode;

            return *this;
        };

        mixed& operator=(mixed&& other) {
            switch (other.m_storage_mode) {
                case storage_mode::uninitialized:
                    break;
                case storage_mode::mixed_bridge:
                    new (&m_mixed) internal::bridge::mixed(std::move(other.m_mixed));
                    break;
                case storage_mode::managed:
                    new (&m_mixed) internal::bridge::mixed(std::move(other.m_mixed));
                    break;
                case storage_mode::unmanaged_dictionary:
                    new (&m_unmanaged_map) std::map<std::string, mixed>(std::move(other.m_unmanaged_map));
                    break;
                case storage_mode::vector:
                    new (&m_unmanaged_vector) std::vector<mixed>(std::move(other.m_unmanaged_vector));
                    break;
            }
            m_storage_mode = std::move(other.m_storage_mode);

            return *this;
        };

        ~mixed() {

        }

        mixed(std::monostate);
        mixed(int64_t);
        mixed(bool);
        mixed(const std::string&);
        mixed(const char*);
        mixed(double);
        mixed(const std::vector<uint8_t>&);
        mixed(std::chrono::time_point<std::chrono::system_clock>);
        mixed(uuid);
        mixed(object_id);
        mixed(decimal128);
        mixed(const std::vector<mixed>& o) {
            new (&m_unmanaged_vector) std::vector<mixed>(o);
            m_storage_mode = storage_mode::vector;
        }

        mixed(const std::map<std::string, mixed>& o) {
            new (&m_unmanaged_map) std::map<std::string, mixed>(o);
            m_storage_mode = storage_mode::unmanaged_dictionary;
        }

        // unmanaged
        template <typename T, std::enable_if_t<managed<T, void>::is_object, bool> = 0>
        mixed(const T&) {
            static_assert(!std::is_same<T, T>::value, "This function is deleted: Unmanaged object must be added to the Realm manually first.");
        }

        // managed
        template <typename T, std::enable_if_t<T::is_object, bool> = 0>
        mixed(const T& link) {
            m_mixed = internal::bridge::mixed(internal::bridge::obj_link(link.m_obj.get_table().get_key(), link.m_obj.get_key()));
            m_storage_mode = storage_mode::mixed_bridge;
        }

        template <typename T>
        mixed(const managed<T*, void>& link) {
            m_mixed = internal::bridge::mixed(internal::bridge::obj_link(link.m_obj->get_table().get_key(), link.m_obj->get_key()));
            m_storage_mode = storage_mode::mixed_bridge;
        }

        map get_dictionary() const; // TODO: Make private
        vector get_list() const; // TODO: Make private

        enum class storage_mode {
            uninitialized,
            mixed_bridge,
            unmanaged_dictionary,
            vector,
            managed
        };
        storage_mode m_storage_mode = storage_mode::uninitialized; // TODO: Make private

        union { // TODO: Make private
            internal::bridge::mixed m_mixed;
            std::map<std::string, mixed> m_unmanaged_map;
            std::vector<mixed> m_unmanaged_vector;
        };


        std::optional<std::string> m_nested_dictionary_key;
        std::optional<internal::bridge::dictionary> m_nested_managed_map;
        std::optional<internal::bridge::list> m_nested_managed_list;
        std::optional<internal::bridge::object> m_obj;

    private:
        friend bool operator==(const realm::mixed&, const realm::mixed&);
        friend bool operator!=(const realm::mixed&, const realm::mixed&);
        friend bool operator>(const realm::mixed&, const realm::mixed&);
        friend bool operator>=(const realm::mixed&, const realm::mixed&);
        friend bool operator<(const realm::mixed&, const realm::mixed&);
        friend bool operator<=(const realm::mixed&, const realm::mixed&);
        friend internal::bridge::mixed internal::get_mixed_bridge(const mixed&);

        std::optional<internal::bridge::col_key> m_col_key;
    };

    template<typename T>
    void get(const realm::mixed&, const realm::db&);

    bool operator==(const realm::mixed&, const realm::mixed&);
    bool operator!=(const realm::mixed&, const realm::mixed&);
    bool operator>(const realm::mixed&, const realm::mixed&);
    bool operator>=(const realm::mixed&, const realm::mixed&);
    bool operator<(const realm::mixed&, const realm::mixed&);
    bool operator<=(const realm::mixed&, const realm::mixed&);

    template <typename T>
    inline T mixed_cast(const mixed&);

    template <>
    std::string mixed_cast(const mixed& o);
    template <>
    std::monostate mixed_cast(const mixed& o);
    template <>
    int64_t mixed_cast(const mixed& o);
    template <>
    bool mixed_cast(const mixed& o);
    template <>
    double mixed_cast(const mixed& o);
    template <>
    std::vector<uint8_t> mixed_cast(const mixed& o);
    template <>
    std::chrono::time_point<std::chrono::system_clock> mixed_cast(const mixed& o);
    template <>
    uuid mixed_cast(const mixed& o);
    template <>
    object_id mixed_cast(const mixed& o);
    template <>
    decimal128 mixed_cast(const mixed& o);

    // Collections
    template <>
    mixed::map mixed_cast(const mixed& o);
    template <>
    mixed::vector mixed_cast(const mixed& o);

//    template <>
//    inline std::map<std::string, mixed> mixed_cast(const mixed& o) {
//        auto map = o.get_dictionary();
//        return map.operator std::map<std::string, mixed>();
//    }

    internal::bridge::object get_stored_object(const mixed& m, const char* class_name, db& realm);

    internal::bridge::obj get_obj(const internal::bridge::object&);
    internal::bridge::realm get_realm(const internal::bridge::object&);

        // Objects
    template <typename T>
    T mixed_cast(const mixed& m, db& realm) {
        auto o = get_stored_object(m, T::schema.name, realm);
        return T(get_obj(o), get_realm(o));
    }
}

#endif //CPPREALM_MIXED_HPP
