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

#include <cpprealm/type_info.hpp>

#include <realm/object-store/list.hpp>
#include <realm/object-store/object.hpp>
#include <realm/object-store/object_store.hpp>
#include <realm/object-store/shared_realm.hpp>

#include <any>
#include "persisted.hpp"

namespace realm {

struct Realm;
struct NotificationToken;
struct notification_token;
struct object_base;
template <typename T>
struct ObjectChange;

template <typename T>
struct persisted_container_base;
/**
 A token which is returned from methods which subscribe to changes to a `realm::object`.
 */
struct notification_token {
    notification_token() = default;
private:
    explicit notification_token(realm::NotificationToken&& token)
    : m_token(std::move(token))
    {
    }

    friend struct object_base;
    template <typename T>
    friend struct persisted_container_base;
    template <typename T>
    friend struct results;
    realm::NotificationToken m_token;
};

template <typename T>
struct collection_change {
    /// The list being observed.
    const persisted_container_base<T>* collection;
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
struct collection_callback_wrapper {
    static_assert(realm::type_info::ListPersistableConcept<T>::value);
    util::UniqueFunction<void(collection_change<T>)> handler;
    persisted<T>& collection;
    bool ignoreChangesInInitialNotification;

    void operator()(realm::CollectionChangeSet const& changes) {
        if (ignoreChangesInInitialNotification) {
            ignoreChangesInInitialNotification = false;
            handler({&collection, {},{},{}});
        }
        else if (changes.empty()) {
            handler({&collection, {},{},{}});

        }
        else if (!changes.collection_root_was_deleted || !changes.deletions.empty()) {
            handler({&collection,
                to_vector(changes.deletions),
                to_vector(changes.insertions),
                to_vector(changes.modifications),
            });
        }
    }

private:
    std::vector<uint64_t> to_vector(const IndexSet& index_set) {
        auto vector = std::vector<uint64_t>();
        for (auto index : index_set.as_indexes()) {
            vector.push_back(index);
        }
        return vector;
    };
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

} // namespace realm

#endif /* notifications_hpp */
