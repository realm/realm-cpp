////////////////////////////////////////////////////////////////////////////
//
// Copyright 2022 Realm Inc.
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

#ifndef notifications_hpp
#define notifications_hpp

#include <cpprealm/alpha_support.hpp>
#ifdef CPP_REALM_ENABLE_ALPHA_SDK
#include <cpprealm/persisted.hpp>
#endif

#include <cpprealm/thread_safe_reference.hpp>
#include <cpprealm/internal/bridge/dictionary.hpp>
#include <cpprealm/internal/bridge/list.hpp>
#include <cpprealm/internal/bridge/set.hpp>

#include <any>
#include <future>
#include <utility>
#include <optional>

namespace realm {
template <typename T>
struct object_base;
template <typename T>
struct ObjectChange;

/**
 A token which is returned from methods which subscribe to changes to a `realm::object`.
 */
struct notification_token {
    notification_token(const notification_token &nt) noexcept = delete;
    notification_token &operator=(const notification_token &) = delete;
    notification_token(notification_token&& other) noexcept {
        m_token = std::move(other.m_token);
        m_dictionary = std::move(other.m_dictionary);
        m_list = std::move(other.m_list);
        m_realm = std::move(other.m_realm);
    };
    notification_token &operator=(notification_token &&other) {
        m_token = std::move(other.m_token);
        m_dictionary = std::move(other.m_dictionary);
        m_list = std::move(other.m_list);
        m_realm = std::move(other.m_realm);
        return *this;
    };
    notification_token() = default;
    ~notification_token() = default;

    notification_token(internal::bridge::notification_token&& token)
            : m_token(std::move(token)) {}
    void unregister() {
        m_token.unregister();
    }

    internal::bridge::notification_token m_token;
    std::shared_ptr<internal::bridge::dictionary> m_dictionary;
    std::shared_ptr<internal::bridge::list> m_list;
    std::shared_ptr<internal::bridge::set> m_set;
    internal::bridge::realm m_realm;
};

// MARK: PropertyChange
/**
 Information about a specific property which changed in an `realm::object` change notification.
 */
template <typename T>
struct PropertyChange {
    /**
     The name of the property which changed.
    */
    std::string name;

    /**
     Value of the property before the change occurred. This is not supplied if
     the change happened on the same thread as the notification and for `List`
     properties.

     For object properties this will give the object which was previously
     linked to, but that object will have its new values and not the values it
     had before the changes. This means that `previousValue` may be a deleted
     object, and you will need to check `isInvalidated` before accessing any
     of its properties.
    */
    std::optional<typename decltype(T::schema)::variant_t> old_value;

    /**
     The value of the property after the change occurred. This is not supplied
     for `List` properties and will always be nil.
    */
    std::optional<typename decltype(T::schema)::variant_t> new_value;
};

#ifdef CPP_REALM_ENABLE_ALPHA_SDK
template <typename T>
        struct collection_change {
    /// The list being observed.
    const persisted<T>* collection;
    std::vector<uint64_t> deletions;
    std::vector<uint64_t> insertions;
    std::vector<uint64_t> modifications;

    // This flag indicates whether the underlying object which is the source of this
    // collection was deleted. This applies to lists, dictionaries and sets.
    // This enables notifiers to report a change on empty collections that have been deleted.
    bool collection_root_was_deleted = false;

    [[nodiscard]] bool empty() const noexcept {
        return deletions.empty() && insertions.empty() && modifications.empty() &&
               !collection_root_was_deleted;
    }
};


template <typename T>
struct collection_callback_wrapper : internal::bridge::collection_change_callback {
    std::function<void(collection_change<T>)> handler;
    persisted<T>& collection;
    bool ignoreChangesInInitialNotification;

    collection_callback_wrapper(std::function<void(collection_change<T>)> handler,
                                persisted<T>& collection,
                                bool ignoreChangesInInitialNotification)
        : handler(handler)
          , collection(collection)
          , ignoreChangesInInitialNotification(ignoreChangesInInitialNotification)
    {}

    void before(const realm::internal::bridge::collection_change_set &c) final {}
    void after(internal::bridge::collection_change_set const& changes) final {
        if (ignoreChangesInInitialNotification) {
            ignoreChangesInInitialNotification = false;
            handler({&collection, {},{},{}});
        }
        else if (changes.empty()) {
            handler({&collection, {},{},{}});

        }
        else if (!changes.collection_root_was_deleted() || !changes.deletions().empty()) {
            handler({&collection,
                    to_vector(changes.deletions()),
                    to_vector(changes.insertions()),
                    to_vector(changes.modifications()),
            });
        }
    }

private:
    std::vector<uint64_t> to_vector(const internal::bridge::index_set& index_set) {
        auto vector = std::vector<uint64_t>();
        for (auto index : index_set.as_indexes()) {
            vector.push_back(index);
        }
        return vector;
    };
};
#endif

} // namespace realm

#endif /* notifications_hpp */
