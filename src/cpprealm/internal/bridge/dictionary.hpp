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

#ifndef CPPREALM_BRIDGE_DICTIONARY_HPP
#define CPPREALM_BRIDGE_DICTIONARY_HPP

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm::object_store {
    class Dictionary;
}

namespace realm {
    class Dictionary;
    using CoreDictionary = Dictionary;
    struct dictionary_change_set {
        dictionary_change_set(const dictionary_change_set&);
        dictionary_change_set(dictionary_change_set&&) = default;
        dictionary_change_set& operator=(const dictionary_change_set&);
        dictionary_change_set& operator=(dictionary_change_set&&) = default;

        // Keys which were removed from the _old_ dictionary
        std::vector<std::string> deletions;

        // Keys in the _new_ dictionary which are new insertions
        std::vector<std::string> insertions;

        // Keys of objects/values which were modified
        std::vector<std::string> modifications;

        bool collection_root_was_deleted = false;
    };

    struct dictionary_collection_change {
        dictionary_collection_change() = default;


        dictionary_collection_change(std::vector<std::string>&& i,
                                     std::vector<std::string>&& m,
                                     std::vector<std::string>&& d,
                                     bool c)
            : insertions(std::move(i)),
              modifications(std::move(m)),
              deletions(std::move(d)),
              collection_root_was_deleted(c) {}

        std::vector<std::string> insertions;
        std::vector<std::string> modifications;
        std::vector<std::string> deletions;
        // This flag indicates whether the underlying object which is the source of this
        // collection was deleted. This applies to lists, dictionaries and sets.
        // This enables notifiers to report a change on empty collections that have been deleted.
        bool collection_root_was_deleted = false;

        [[nodiscard]] bool empty() const noexcept {
            return deletions.empty() && insertions.empty() && modifications.empty() &&
                   !collection_root_was_deleted;
        }
    };

    struct dictionary_callback_wrapper {
        std::function<void(dictionary_collection_change)> handler;
        bool ignore_changes_in_initial_notification;

        dictionary_callback_wrapper(std::function<void(dictionary_collection_change)> handler,
                                    bool ignore_changes_in_initial_notification)
            : handler(handler)
              , ignore_changes_in_initial_notification(ignore_changes_in_initial_notification)
        {}
    };
}

namespace realm::internal::bridge {
    using Dictionary = object_store::Dictionary;
    using CoreDictionary = CoreDictionary;

    struct mixed;
    struct binary;
    struct timestamp;
    struct obj_key;
    struct uuid;
    struct object_id;
    struct notification_token;
    struct collection_change_callback;
    struct obj;

    struct core_dictionary {
        core_dictionary();
        core_dictionary(const core_dictionary &other);
        core_dictionary &operator=(const core_dictionary &other);
        core_dictionary(core_dictionary &&other);
        core_dictionary &operator=(core_dictionary &&other);
        ~core_dictionary();

        core_dictionary(const CoreDictionary& v); //NOLINT(google-explicit-constructor)
        operator CoreDictionary () const; //NOLINT(google-explicit-constructor)
        void insert(const std::string& key, const mixed& value);
        void insert(const std::string& key, const std::string& value);
        obj create_and_insert_linked_object(const std::string& key);
        obj create_and_insert_linked_object(const std::string& key, const internal::bridge::mixed& pk);
        mixed get(const std::string& key) const;
        void erase(const std::string& key);
        obj get_object(const std::string& key);
        std::pair<mixed, mixed> get_pair(size_t ndx) const;
        size_t find_any_key(const std::string& value) const noexcept;

        size_t size() const;
    private:
        const CoreDictionary* get_dictionary() const;
        CoreDictionary* get_dictionary();
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::CoreDictionary m_dictionary[1];
#else
        std::shared_ptr<CoreDictionary> m_dictionary;
#endif
    };


    struct dictionary {
        dictionary();
        dictionary(const dictionary& other) ;
        dictionary& operator=(const dictionary& other) ;
        dictionary(dictionary&& other);
        dictionary& operator=(dictionary&& other);
        ~dictionary();
        dictionary(const Dictionary& v); //NOLINT(google-explicit-constructor)
        operator Dictionary() const; //NOLINT(google-explicit-constructor)
        void insert(const std::string& key, const mixed& value);
        void insert(const std::string &key, const std::string &value);
        [[nodiscard]] size_t size() const;
        void remove_all();
        void remove(const std::string&);
        std::pair<std::string, mixed> get_pair(size_t);
        [[nodiscard]] size_t get_key_index(const std::string&);
        void clear();
        [[nodiscard]] size_t find(const std::string&);
        obj insert_embedded(const std::string&);
        notification_token add_notification_callback(std::shared_ptr<collection_change_callback>&& cb);
        notification_token add_key_based_notification_callback(std::shared_ptr<dictionary_callback_wrapper>&& cb);
    private:
        const Dictionary* get_dictionary() const;
        Dictionary* get_dictionary();
        friend inline Dictionary * get_dictionary(dictionary& lst);
        friend inline const Dictionary* get_dictionary(const dictionary& lst);
        template <typename T>
        friend T get(dictionary&, const std::string&);
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::Dictionary m_dictionary[1];
#else
        std::shared_ptr<Dictionary> m_dictionary;
#endif
    };

    template <typename T>
    [[nodiscard]] T get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] std::string get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] uuid get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] object_id get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] timestamp get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] binary get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] obj get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] obj_key get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] int64_t get(dictionary&, const std::string&);
    template <>
    [[nodiscard]] double get(dictionary&, const std::string&);
}

#endif //CPPREALM_BRIDGE_DICTIONARY_HPP
