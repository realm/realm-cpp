////////////////////////////////////////////////////////////////////////////
//
// Copyright 2021 Realm Inc.
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

#ifndef realm_object_hpp
#define realm_object_hpp

#include <cpprealm/notifications.hpp>
#include <cpprealm/type_info.hpp>
#include <cpprealm/schema.hpp>

#include <realm/obj.hpp>
#include <realm/table.hpp>
#include <realm/object-store/object.hpp>
#include <realm/object-store/object_store.hpp>
#include <realm/object-store/shared_realm.hpp>

#include <any>

namespace realm {

struct Realm;
struct NotificationToken;
struct object;



// MARK: ObjectChange
/**
 Information about the changes made to an object which is passed to `realm::object`'s
 notification blocks.
 */
template <type_info::ObjectPersistable T>
struct ObjectChange {
    /// The object being observed.
    const T* object;
    /// The object has been deleted from the Realm.
    bool is_deleted;
    /**
     If an error occurs, notification blocks are called one time with an `error`
     result and an `std::exception` containing details about the error. Currently the
     only errors which can occur are when opening the Realm on a background
     worker thread to calculate the change set. The callback will never be
     called again after `error` is delivered.
     */
    std::exception_ptr error;
    /**
     One or more of the properties of the object have been changed.
     */
    PropertyChange property;
};

namespace {
template <type_info::ObjectPersistable T>
struct ObjectChangeCallbackWrapper;
}
// MARK: Object
/**
 `realm::object` is a class used to define Realm model objects.

 In Realm you define your model classes by subclassing `realm::object` and adding properties to be managed.
 You then instantiate and use your custom subclasses instead of using the `realm::object` class directly.

 ```cpp
 class Dog: realm::object {
     realm::persisted<std::string> name;
     realm::persisted<bool> adopted;
     realm::persisted<std::vector<Dog>> siblings;

     using schema = realm::schema<"Dog", realm::property<"name", &Dog::name>,
                                  realm::property<"adopted", &Dog::adopted>,
                                  realm::property<"siblings", &Dog::siblings>>;
 }
 ```
 ### Schema

 All properties which should be stored by Realm must be explicitly marked with
 `realm::persisted` and added to the schema. Any properties not marked with `realm::persisted`
 and not added to the schema will be ignored entirely by Realm, and may be of any type.

 ### Querying

 You can retrieve all objects of a given type from a Realm by calling the `objects(_:)` instance method.

 */
struct object {
    /**
     Performs actions contained within the given block inside a write transaction.

     If the block throws an error, the transaction will be canceled and any
     changes made before the error will be rolled back.

     Only one write transaction can be open at a time for each Realm file. Write
     transactions cannot be nested, and trying to begin a write transaction on a
     Realm which is already in a write transaction will throw an exception.
     Calls to `write` from `Realm` instances for the same Realm file in other
     threads or other processes will block until the current write transaction
     completes or is cancelled.

     Before beginning the write transaction, `write` updates the `Realm`
     instance to the latest Realm version, as if `refresh()` had been called,
     and generates notifications if applicable. This has no effect if the Realm
     was already up to date.

     @param block: The block containing actions to perform.
     @returns: The value returned from the block, if any.

     @throws: An error if the transaction could not be completed successfully.
             If `block` throws, the function throws the propagated `ErrorType` instead.
     */
    void write(std::function<void()> fn);

    /**
     Registers a block to be called each time the object changes.

     The block will be asynchronously called after each write transaction which
     deletes the object or modifies any of the managed properties of the object,
     including self-assignments that set a property to its existing value.

     For write transactions performed on different threads or in different
     processes, the block will be called when the managing Realm is
     (auto)refreshed to a version including the changes, while for local write
     transactions it will be called at some point in the future after the write
     transaction is committed.

     The block will be executed on any insertion,
     modification, or deletion for all object properties and the properties of
     any nested, linked objects.
     ```cpp
     class Dog: realm::object {
         realm::persisted<std::string> name;
         realm::persisted<bool> adopted;
         realm::persisted<std::vector<Dog>> siblings;

         using schema = realm::schema<"Dog", realm::property<"name", &Dog::name>,
                                      realm::property<"adopted", &Dog::adopted>,
                                      realm::property<"siblings", &Dog::siblings>>;
     }

     // ... where `dog` is a managed Dog object.
     dog.observe<Dog>([]{ (auto&& changes)
        // ...
     });
     ```

     Only objects which are managed by a Realm can be observed in this way. You
     must retain the returned token for as long as you want updates to be sent
     to the block. To stop receiving updates, call `invalidate()` on the token.

     It is safe to capture a strong reference to the observed object within the
     callback block. There is no retain cycle due to that the callback is
     retained by the returned token and not by the object itself.

     @warning: This method cannot be called during a write transaction, or when
                the containing Realm is read-only.
     @parameter block: The block to call with information about changes to the object.
     @returns: A token which must be held for as long as you want updates to be delivered.
     */
    template <typename T>
    notification_token observe(std::function<void(ObjectChange<T>)> block);

    bool is_managed() const noexcept {
        return m_obj.has_value();
    }

private:
    template <type_info::Persistable T>
    friend struct persisted_base;
    template <type_info::OptionalObjectPersistable T>
    friend constexpr typename type_info::persisted_type<T>::type type_info::convert_if_required(const T& a);
    template <type_info::ListPersistable T>
    friend constexpr typename type_info::persisted_type<T>::type type_info::convert_if_required(const T& a);
    template <StringLiteral, auto Ptr, bool IsPrimaryKey>
    friend struct property;
    template <StringLiteral, type_info::Propertyable ...Properties>
    friend struct schema;
    template <type_info::ObjectPersistable T>
    friend struct ObjectChangeCallbackWrapper;
    template <type_info::ObjectPersistable ...Ts>
    friend struct db;
    template <typename T>
    friend struct thread_safe_reference;
    template <realm::type_info::ListPersistable T>
    friend struct persisted_container_base;

    template <type_info::ObjectPersistable T>
    friend inline bool operator==(const T& lhs, const T& rhs);

    std::shared_ptr<Realm> m_realm = nullptr;
    template <realm::type_info::ObjectPersistable T>
    friend std::ostream& operator<< (std::ostream& stream, const T& object);
    std::optional<Obj> m_obj;
};

template <type_info::ObjectPersistable T>
inline bool operator==(const T& lhs, const T& rhs) {
    if (lhs.is_managed() && rhs.is_managed()) {
        return *lhs.m_obj == *rhs.m_obj && rhs.m_realm == lhs.m_realm;
    } else if (!lhs.is_managed() && !rhs.is_managed()) {
        return false;
    }
    return false;
};

// MARK: - Implementations

namespace {
template <type_info::ObjectPersistable T>
using ObjectNotificationCallback = std::function<void(const T*,
                                                      std::vector<std::string> property_names,
                                                      std::vector<std::any> old_values,
                                                      std::vector<std::any> new_values,
                                                      std::exception_ptr error)>;


}

template <typename T>
notification_token object::observe(std::function<void(ObjectChange<T>)> block) {
  struct ObjectChangeCallbackWrapper {
    ObjectNotificationCallback<T> block;
    const T& object;

    std::optional<std::vector<std::string>> property_names = std::nullopt;
    std::optional<std::vector<std::any>> old_values = std::nullopt;
    bool deleted = false;

    void populateProperties(realm::CollectionChangeSet const& c) {
        if (property_names) {
            return;
        }
        if (!c.deletions.empty()) {
            deleted = true;
            return;
        }
        if (c.columns.empty()) {
            return;
        }

        // FIXME: It's possible for the column key of a persisted property
        // to equal the column key of a computed property.
        auto properties = std::vector<std::string>();
        TableRef table = object.m_realm->read_group().get_table(ObjectStore::table_name_for_object_type(T::schema::name));

        std::apply([&c, &table, &properties](auto&&... props) {
            (((c.columns.count(table->get_column_key(props.name).value)) ?
              properties.push_back(props.name) : void()), ...);
        }, T::schema::properties);
        if (!properties.empty()) {
            property_names = properties;
        }
    }

    std::optional<std::vector<std::any>> readValues(realm::CollectionChangeSet const& c) {
        if (c.empty()) {
            return std::nullopt;
        }
        populateProperties(c);
        if (!property_names) {
            return std::nullopt;
        }

        std::vector<std::any> values;
        for (auto& name : *property_names) {
            std::apply([&name, &values, this](auto&&... props) {
                ((name == props.name ? values.push_back(*(object.*props.ptr)) : void()), ...);
            }, T::schema::properties);
        }
        return values;
    }

    void before(realm::CollectionChangeSet const& c) {
        old_values = readValues(c);
    }

    void after(realm::CollectionChangeSet const& c) {
        auto new_values = readValues(c);
        if (deleted) {
            block(nullptr, {}, {}, {}, nullptr);
        } else if (new_values) {
            block(&object, *property_names, old_values ? *old_values : std::vector<std::any>{}, *new_values, nullptr);
        }
        property_names = std::nullopt;
        old_values = std::nullopt;
    }

    void error(std::exception_ptr err) {
        block(nullptr, {}, {}, {}, err);
    }
  };
    if (!m_realm) {
        throw std::runtime_error("Only objects which are managed by a Realm support change notifications");
    }
    notification_token token;
    token.m_object = realm::Object(m_realm, T::schema::to_core_schema(), *(m_obj));
    token.m_token = token.m_object.add_notification_callback(ObjectChangeCallbackWrapper{
        [block](const T* ptr,
                std::vector<std::string> property_names,
                std::vector<std::any> old_values,
                std::vector<std::any> new_values,
                std::exception_ptr error) {
            if (!ptr) {
                if (error) {
                    block(ObjectChange<T> { .error = error });
                } else {
                    block(ObjectChange<T> { .is_deleted = true });
                }
            } else {
                for (size_t i = 0; i < property_names.size(); i++) {
                    PropertyChange property;
                    property.name = property_names[i];
                    if (old_values.size()) {
                        property.old_value = old_values[i];
                    }
                    if (new_values.size()) {
                        property.new_value = new_values[i];
                    }
                    block(ObjectChange<T> { .object = ptr, .property = property });
                }
            }
        }, *static_cast<T*>(this)});
    return token;
};

} // namespace realm

#endif /* realm_object_hpp */
