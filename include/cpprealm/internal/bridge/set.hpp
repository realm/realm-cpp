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

#ifndef CPPREALM_BRIDGE_SET_HPP
#define CPPREALM_BRIDGE_SET_HPP

#include <cstdlib>
#include <memory>
#include <set>
#include <string>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    namespace object_store {
        class Set;
    }
}

namespace realm::internal::bridge {
    struct realm;
    struct obj;
    struct obj_key;
    struct mixed;
    struct binary;
    struct uuid;
    struct object_id;
    struct col_key;
    struct timestamp;
    struct table;
    struct notification_token;
    struct collection_change_callback;

    struct set {
        set();
        set(const set& other) ;
        set& operator=(const set& other) ;
        set(set&& other);
        set& operator=(set&& other);
        ~set();
        set(const object_store::Set&); //NOLINT(google-explicit-constructor)
        operator object_store::Set() const; //NOLINT(google-explicit-constructor)
        set(const realm& realm, const obj& obj, const col_key&);

        mixed get_any(const size_t& i) const;
        obj get_obj(const size_t& i) const;

        [[nodiscard]] size_t size() const;
        void remove_all();

        table get_table() const;

        std::pair<size_t, bool> insert(const std::string&);
        std::pair<size_t, bool> insert(const int64_t &);
        std::pair<size_t, bool> insert(const double &);
        std::pair<size_t, bool> insert(const bool &);
        std::pair<size_t, bool> insert(const binary &);
        std::pair<size_t, bool> insert(const uuid &);
        std::pair<size_t, bool> insert(const object_id &);
        std::pair<size_t, bool> insert(const mixed &);
        std::pair<size_t, bool> insert(const obj_key &);
        std::pair<size_t, bool> insert(const timestamp &);

        void remove(const int64_t &);
        void remove(const bool &);
        void remove(const double &);
        void remove(const std::string &);
        void remove(const uuid &);
        void remove(const object_id &);
        void remove(const mixed &);
        void remove(const timestamp &);
        void remove(const binary&);
        void remove(const obj_key&);

        size_t find(const int64_t &);
        size_t find(const bool &);
        size_t find(const double &);
        size_t find(const std::string &);
        size_t find(const uuid &);
        size_t find(const object_id &);
        size_t find(const mixed &);
        size_t find(const timestamp &);
        size_t find(const binary&);
        size_t find(const obj_key&);
        notification_token add_notification_callback(std::shared_ptr<collection_change_callback>);
    private:
        const object_store::Set* get_set() const;
        object_store::Set* get_set();
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::Set m_set[1];
#else
        std::shared_ptr<object_store::Set> m_set;
#endif
    };
}

#endif //CPPREALM_BRIDGE_SET_HPP
