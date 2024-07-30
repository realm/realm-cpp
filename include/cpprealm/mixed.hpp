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

    struct mixed {

        struct map {

            struct box {
                box(const std::shared_ptr<mixed>& m) {
                    m_mixed = m;
                }

                box& operator=(const mixed&) {
                    std::terminate();
                }

                box& operator=(mixed&&) {
                    // set new value on managed object
                    std::terminate();
                }

                mixed operator *() {
                    return *m_mixed;
                }
            private:
                std::shared_ptr<mixed> m_mixed;
            };

            map(const std::map<std::string, mixed>& o) {
                new (&m_unmanaged_map) std::map<std::string, mixed>(o);
                m_is_managed = false;
            }
            map(const internal::bridge::dictionary& o) {
                new (&m_managed_map) internal::bridge::dictionary(o);
                m_is_managed = true;
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
                        return { pair.first, std::make_shared<mixed>(mixed(pair.second)) };
                    } else {
                        auto it = m_unmanaged_map.begin();
                        std::advance(it, m_i);
                        return std::pair { it->first, std::make_shared<mixed>(it->second) };
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
                iterator(size_t i, const internal::bridge::dictionary& o) {
                    m_i = i;
                    new (&m_managed_map) internal::bridge::dictionary(o);
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
                bool m_is_managed = false;

                iterator(size_t i/*, const managed<std::map<std::string, T>>* parent*/)
                        : m_i(i)/*, m_parent(parent)*/
                {
                }
                size_t m_i;
//                const managed<std::map<std::string, T>>* m_parent;
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
                    return iterator(0, m_managed_map);
                } else {
                    return iterator(0, m_unmanaged_map);
                }
            }

            iterator end() const
            {
                if (m_is_managed) {
                    return iterator(size(), m_managed_map);
                } else {
                    return iterator(size(), m_unmanaged_map);
                }
            }

            box operator[] (const std::string& k) {
                if (m_is_managed) {
                    auto key_index = m_managed_map.get_key_index(k); // TODO: dont use index
                    return std::make_shared<mixed>(m_managed_map.get_pair(key_index).second);
                } else {
                    return std::make_shared<mixed>(m_unmanaged_map[k]);
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
                case storage_mode::dictionary:
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
                case storage_mode::dictionary:
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
                case storage_mode::dictionary:
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
                case storage_mode::dictionary:
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
            m_storage_mode = storage_mode::dictionary;
        }

        // unmanaged
        template <typename T, std::enable_if_t<managed<T, void>::is_object, bool> = 0>
        mixed(const T& o) {
            m_storage_mode = storage_mode::unmanaged_object;
            new (&m_unmanaged_object) std::shared_ptr<void>(std::make_shared<T>(o));
        }

        // managed
        template <typename T>
        mixed(const managed<T, void>&) {
        }

        template <typename T>
        mixed(const managed<T*, void>&) {
        }

//        template <typename T>
//        mixed(const T*) {
//        }
        map get_dictionary() const;

        enum class storage_mode {
            uninitialized,
            mixed_bridge,
            unmanaged_object,
            dictionary,
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
            case mixed::storage_mode::dictionary:
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
