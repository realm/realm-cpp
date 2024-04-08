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

#include <cpprealm/thread_safe_reference.hpp>
#include <cpprealm/internal/bridge/dictionary.hpp>
#include <cpprealm/internal/bridge/list.hpp>
#include <cpprealm/internal/bridge/set.hpp>

#include <any>
#include <future>
#include <utility>

namespace realm {
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
        m_set = std::move(other.m_set);
        m_results = std::move(other.m_results);
        m_realm = std::move(other.m_realm);
    };
    notification_token &operator=(notification_token &&other) {
        m_token = std::move(other.m_token);
        m_dictionary = std::move(other.m_dictionary);
        m_list = std::move(other.m_list);
        m_set = std::move(other.m_set);
        m_results = std::move(other.m_results);
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
    std::shared_ptr<internal::bridge::results> m_results;
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

} // namespace realm

#endif /* notifications_hpp */
