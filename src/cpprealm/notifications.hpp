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

namespace realm {

struct Realm;
struct NotificationToken;
struct object;
template <type_info::ListPersistable T>
struct persisted_container_base;
/**
 A token which is returned from methods which subscribe to changes to a `realm::object`.
 */
struct notification_token {
private:
    template <realm::type_info::ListPersistable T>
    friend struct persisted_container_base;
    List m_list;
    friend struct object;
    realm::Object m_object;
    realm::NotificationToken m_token;
};


// MARK: PropertyChange
/**
 Information about a specific property which changed in an `realm::object` change notification.
 */
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
    std::optional<std::any> old_value;

    /**
     The value of the property after the change occurred. This is not supplied
     for `List` properties and will always be nil.
    */
    std::optional<std::any> new_value;
};

template <type_info::ListPersistable T>
struct CollectionChange {
    /// The list being observed.
    const persisted_container_base<T>* collection;
    std::vector<uint64_t> deletions;
    std::vector<uint64_t> insertions;
    std::vector<uint64_t> modifications;

    // This flag indicates whether the underlying object which is the source of this
    // collection was deleted. This applies to lists, dictionaries and sets.
    // This enables notifiers to report a change on empty collections that have been deleted.
    bool collection_root_was_deleted = false;

    bool empty() const noexcept {
        return deletions.empty() && insertions.empty() && modifications.empty() &&
        !collection_root_was_deleted;
    }
};

} // namespace realm

#endif /* notifications_hpp */
