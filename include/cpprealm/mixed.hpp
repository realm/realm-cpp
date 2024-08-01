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

        void insert_dictionary_value(internal::bridge::dictionary& d, const std::string& key,  const mixed& values);
        void insert_dictionary_values(internal::bridge::dictionary& d, const std::map<std::string, mixed>& values);

        void set_managed(internal::bridge::obj& obj,
                         const internal::bridge::col_key& key,
                         const internal::bridge::realm& realm,
                         const realm::mixed& value);
    };


    struct mixed {

        struct map {

            struct box {
                box(std::optional<std::map<std::string, mixed>> map, const std::string& key) {
                    m_unmanaged_map = map;
                    m_key = key;
                    m_is_managed = false;
                }

                box(std::optional<internal::bridge::dictionary> map, const std::string& key, internal::bridge::col_key c) {
                    m_managed_map = map;
                    m_key = key;
                    m_origin_col_key = c;
                    m_is_managed = true;
                }

                box& operator=(const mixed&) {
                    std::terminate();
                }

                box& operator=(mixed&& m) {

                    if (m_is_managed) {
                        context ctx;
                        ctx.insert_dictionary_value(*m_managed_map, m_key, std::move(m));
                    } else {
                        std::terminate();
                    }


                    return *this;
                }

                mixed operator *() {
                    if (m_is_managed) {
                        if (m_managed_map->get(m_key).is_collection_type(internal::bridge::mixed::collection_type::dictionary)) {
                            auto m = mixed();
                            m.m_nested_dictionary_key = m_key;
                            m.m_nested_managed_map = m_managed_map->get_dictionary(m_key);
                            m.m_storage_mode = storage_mode::managed;
                            return m;
                        } else if (m_managed_map->get(m_key).is_collection_type(internal::bridge::mixed::collection_type::list)) {
                            std::terminate();
                        } else {
                            return m_managed_map->get(m_key);
                        }
                    } else {
                        return m_unmanaged_map->at(m_key);
                    }
                }
            private:
//                std::shared_ptr<mixed> m_mixed;

                bool m_is_managed = false;
                std::string m_key;
                std::optional<std::map<std::string, mixed>> m_unmanaged_map;
                std::optional<internal::bridge::dictionary> m_managed_map;
                std::optional<internal::bridge::col_key> m_origin_col_key;

            };

            map(const std::map<std::string, mixed>& o) {
                new (&m_unmanaged_map) std::map<std::string, mixed>(o);
                m_is_managed = false;
            }
            map(const internal::bridge::dictionary& o, std::optional<internal::bridge::col_key> ck = std::nullopt) {
                new (&m_managed_map) internal::bridge::dictionary(o);
                m_col_key = ck;
                m_is_managed = true;
            }

            map& operator=(const std::map<std::string, mixed>& o) {
                m_managed_map.clear();
                context ctx;
                ctx.insert_dictionary_values(m_managed_map, o);

                return *this;
            }

            class iterator {
            public:
                using iterator_category = std::input_iterator_tag;

                bool operator!=(const iterator& other) const
                {
                    return !(*this == other);
                }

                bool operator==(const iterator& other) const
                {
                    return (m_i == other.m_i);
                }

                std::pair<std::string, box> operator*() noexcept
                {
                    if (m_is_managed) {
                        auto pair = m_managed_map.get_pair(m_i);
                        return { pair.first, box(m_managed_map, pair.first, *m_col_key) };
                    } else {
                        auto it = m_unmanaged_map.begin();
                        std::advance(it, m_i);
                        return std::pair { it->first, box(m_unmanaged_map, it->first) };
                    }
                }

                iterator& operator++()
                {
                    this->m_i++;
                    return *this;
                }

                const iterator& operator++(int i)
                {
                    this->m_i += i;
                    return *this;
                }

                iterator(size_t i, const std::map<std::string, mixed>& o) {
                    m_i = i;
                    new (&m_unmanaged_map) std::map<std::string, mixed>(o);
                    m_is_managed = false;
                }
                iterator(size_t i, const internal::bridge::dictionary& o, internal::bridge::col_key ck) {
                    m_i = i;
                    new (&m_managed_map) internal::bridge::dictionary(o);
                    m_col_key = ck;
                    m_is_managed = true;
                }

                ~iterator() {

                }
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

            size_t size() const
            {
                if (m_is_managed) {
                    return m_managed_map.size();
                } else {
                    return m_unmanaged_map.size();
                }
            }

            iterator begin() const
            {
                if (m_is_managed) {
                    return iterator(0, m_managed_map, *m_col_key);
                } else {
                    return iterator(0, m_unmanaged_map);
                }
            }

            iterator end() const
            {
                if (m_is_managed) {
                    return iterator(size(), m_managed_map, *m_col_key);
                } else {
                    return iterator(size(), m_unmanaged_map);
                }
            }

            box operator[] (const std::string& k) {
                if (m_is_managed) {
                    return box(m_managed_map, k, *m_col_key);
                } else {
                    return box(m_unmanaged_map, k);
                }
            }

            operator std::map<std::string, mixed>() {
                if (m_is_managed) {
                    std::map<std::string, mixed> m;
                    for (size_t i = 0; i < m_managed_map.size(); i++) {
                        auto pair = m_managed_map.get_pair(i);
                        m[pair.first] = pair.second;
                    }
                    return m;
                } else {
                    return m_unmanaged_map;
                }
            }

            ~map() {

            }

        private:
            union {
                std::map<std::string, mixed> m_unmanaged_map;
                internal::bridge::dictionary m_managed_map;
            };
            std::optional<internal::bridge::col_key> m_col_key;
            bool m_is_managed = false;
        };

        mixed() { };

        mixed(const internal::bridge::mixed& m) {
            m_storage_mode = storage_mode::managed;
            m_mixed = m;
//            m_obj = std::move(o);
//            m_col_key = std::move(c);
        };

        mixed(internal::bridge::mixed&& m, internal::bridge::object&& o, internal::bridge::col_key c) {
            m_storage_mode = storage_mode::managed;
            m_mixed = std::move(m);
            m_obj = std::move(o);
            m_col_key = std::move(c);
        };

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
                case storage_mode::unmanaged_object:
                    new (&m_unmanaged_object) std::shared_ptr<void>(other.m_unmanaged_object);
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
                case storage_mode::unmanaged_object:
                    new (&m_unmanaged_object) std::shared_ptr<void>(std::move(other.m_unmanaged_object));
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
                case storage_mode::unmanaged_object:
                    new (&m_unmanaged_object) std::shared_ptr<void>(other.m_unmanaged_object);
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
                case storage_mode::unmanaged_object:
                    new (&m_unmanaged_object) std::shared_ptr<void>(std::move(other.m_unmanaged_object));
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
        mixed(std::vector<uint8_t>);
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
        mixed(const T& o) {
            m_storage_mode = storage_mode::unmanaged_object;
            new (&m_unmanaged_object) std::shared_ptr<void>(std::make_shared<T>(o));
        }

        // managed
        template <typename T>
        mixed(const managed<T, void>& link) {
            m_mixed = internal::bridge::mixed(internal::bridge::obj_link(link.m_obj.get_table().get_key(), link.m_obj.get_key()));
            m_storage_mode = storage_mode::mixed_bridge;
        }

        template <typename T>
        mixed(const managed<T*, void>& link) {
            m_mixed = internal::bridge::mixed(internal::bridge::obj_link(link.m_obj.get_table().get_key(), link.m_obj.get_key()));
            m_storage_mode = storage_mode::mixed_bridge;
        }

        map get_dictionary() const; // TODO: Make private

        enum class storage_mode {
            uninitialized,
            mixed_bridge,
            unmanaged_object,
            unmanaged_dictionary,
            vector,
            managed
        };
        storage_mode m_storage_mode = storage_mode::uninitialized; // TODO: Make private

        union { // TODO: Make private
            internal::bridge::mixed m_mixed;
            std::shared_ptr<void> m_unmanaged_object;
            std::map<std::string, mixed> m_unmanaged_map;
            std::vector<mixed> m_unmanaged_vector;
        };


        std::optional<std::string> m_nested_dictionary_key;
        std::optional<internal::bridge::dictionary> m_nested_managed_map;

    private:
        friend bool operator==(const realm::mixed&, const realm::mixed&);
        friend bool operator!=(const realm::mixed&, const realm::mixed&);
        friend bool operator>(const realm::mixed&, const realm::mixed&);
        friend bool operator>=(const realm::mixed&, const realm::mixed&);
        friend bool operator<(const realm::mixed&, const realm::mixed&);
        friend bool operator<=(const realm::mixed&, const realm::mixed&);
        friend internal::bridge::mixed internal::get_mixed_bridge(const mixed&);



        std::optional<internal::bridge::object> m_obj;
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
    inline std::string mixed_cast(const mixed& o) {
        switch (o.m_storage_mode) {
            case mixed::storage_mode::uninitialized:
                break;
            case mixed::storage_mode::mixed_bridge:
            case mixed::storage_mode::managed:
                return o.m_mixed.operator std::string();
            case mixed::storage_mode::unmanaged_object:
                break;
            case mixed::storage_mode::unmanaged_dictionary:
                break;
            case mixed::storage_mode::vector:
                break;
        }
        std::terminate();
    }

    // Collections

    template <>
    inline mixed::map mixed_cast(const mixed& o) {
        return o.get_dictionary();
    }

    template <>
    inline std::map<std::string, mixed> mixed_cast(const mixed& o) {
        auto map = o.get_dictionary();
        return map.operator std::map<std::string, mixed>();
    }

    // Objects

    template <typename T>
    inline
    std::enable_if_t<T::is_object, T>
    mixed_cast(const mixed&) {
        std::terminate();
    }
}

#endif //CPPREALM_MIXED_HPP
