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
template <typename T>
struct ObjectChange {
    static_assert(type_info::ObjectBasePersistableConcept<T>::value);
    /// The object being observed.
    const T* object;
    /// The object has been deleted from the Realm.
    bool is_deleted = false;
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
    std::vector<PropertyChange<T>> property_changes;
};

namespace {
template <typename T>
struct ObjectChangeCallbackWrapper;
template <typename T>
using ObjectNotificationCallback = std::function<void(const T*,
                                                      std::vector<std::string> property_names,
                                                      std::vector<typename decltype(T::schema)::variant_t> old_values,
                                                      std::vector<typename decltype(T::schema)::variant_t> new_values,
                                                      const std::exception_ptr error)>;
} // anonymous namespace

struct object_base {
    bool is_managed() const noexcept {
        return m_object && m_object->is_valid();
    }
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
    notification_token observe(std::function<void(ObjectChange<T>)> block)
    {

        struct ObjectChangeCallbackWrapper {
            ObjectNotificationCallback<T> block{};
            const T& object;
            const Object& m_object;

            std::optional<std::vector<std::string>> property_names = std::nullopt;
            std::optional<std::vector<typename decltype(T::schema)::variant_t>> old_values = std::nullopt;
            bool deleted = false;

            void populateProperties(realm::CollectionChangeSet const& c)
            {
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
                const TableRef table = m_object.obj().get_table();

                for (size_t i = 0; i < std::tuple_size<decltype(T::schema.properties)>{}; i++) {
                    if (c.columns.count(table->get_column_key(T::schema.names[i]).value)) {
                        properties.push_back(T::schema.names[i]);
                    }
                }

                if (!properties.empty()) {
                    property_names = properties;
                }
            }

            std::optional<std::vector<typename decltype(T::schema)::variant_t>> readValues(realm::CollectionChangeSet const& c) {
                if (c.empty()) {
                    return std::nullopt;
                }
                populateProperties(c);
                if (!property_names) {
                    return std::nullopt;
                }

                std::vector<typename decltype(T::schema)::variant_t> values;
                for (auto& name : *property_names) {
                    auto value = T::schema.property_value_for_name(name, object);
                    values.push_back(value);
                }
                return values;
            }

            void before(realm::CollectionChangeSet const& c)
            {
                old_values = readValues(c);
            }

            void after(realm::CollectionChangeSet const& c)
            {
                auto new_values = readValues(c);
                if (deleted) {
                    block(nullptr, {}, {}, {}, nullptr);
                } else if (new_values) {
                    block(&object,
                          *property_names,
                          old_values ? *old_values : std::vector<typename decltype(T::schema)::variant_t>{},
                          *new_values,
                          nullptr);
                }
                property_names = std::nullopt;
                old_values = std::nullopt;
            }

            void error(std::exception_ptr err) {
                block(nullptr, {}, {}, {}, err);
            }
        };
        if (!is_managed()) {
            throw std::runtime_error("Only objects which are managed by a Realm support change notifications");
        }
        return notification_token(m_object->add_notification_callback(ObjectChangeCallbackWrapper {
                [block](const T* ptr,
                        std::vector<std::string> property_names,
                        std::vector<typename decltype(T::schema)::variant_t> old_values,
                        std::vector<typename decltype(T::schema)::variant_t> new_values,
                        const std::exception_ptr& error) {
                    if (!ptr) {
                        if (error) {
                            block(ObjectChange<T> { .error = error });
                        } else {
                            block(ObjectChange<T> { .is_deleted = true });
                        }
                    } else {
                        std::vector<PropertyChange<T>> property_changes;
                        for (size_t i = 0; i < property_names.size(); i++) {
                            PropertyChange<T> property;
                            property.name = property_names[i];
                            if (!old_values.empty()) {
                                property.old_value = old_values[i];
                            }
                            if (!new_values.empty()) {
                                property.new_value = new_values[i];
                            }
                            property_changes.push_back(property);
                        }
                        block(ObjectChange<T> { .object = ptr, .property_changes = property_changes });
                    }
                }, *static_cast<T*>(this), *m_object}));
    }
protected:
    std::optional<Object> m_object;
    template <typename T, typename>
    friend struct persisted_base;
    template <typename T>
    friend class persisted_object_container_base;
    template <typename T, typename>
    friend struct type_info::persisted_type;
    template <typename Class, typename ...Properties>
    friend struct internal::schema;
    template <auto Ptr, bool IsPrimaryKey>
    friend struct property;
    template <typename T, typename>
    friend struct thread_safe_reference;
    template <typename ...Ts>
    friend struct db;

    template <typename T>
    friend typename std::enable_if<std::is_base_of<realm::object_base, T>::value, std::ostream>::type&
    operator<< (std::ostream& stream, const T& object);
    template <typename T>
    friend inline auto operator==(
            const T& lhs,
            const T& rhs) -> std::enable_if_t<type_info::ObjectBasePersistableConcept<T>::value, bool>;
};
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
struct object : public object_base {
};

/**
 `embedded_object` is a base class used to define embedded Realm model objects.

 Embedded objects work similarly to normal objects, but are owned by a single
 parent Object (which itself may be embedded). Unlike normal top-level objects,
 embedded objects cannot be directly created in or added to a Realm. Instead,
 they can only be created as part of a parent object, or by assigning an
 unmanaged object to a parent object's property. Embedded objects are
 automatically deleted when the parent object is deleted or when the parent is
 modified to no longer point at the embedded object, either by reassigning an
 Object property or by removing the embedded object from the List containing it.

 Embedded objects can only ever have a single parent object which links to
 them, and attempting to link to an existing managed embedded object will throw
 an exception.

 The property types supported on `embedded_object` are the same as for `object`,
 except for that embedded objects cannot link to top-level objects, so `object`
 and `std::vector<object>` properties are not supported (`embedded_object` and
 `std::vector<embedded_object>` *are*).

 Embedded objects cannot have primary keys or indexed properties.

 ```cpp
 class Owner: object {
     realm::persisted<std::string> name;
     realm::persisted<std::vector<dog>> dogs;
 };
 class Dog: embedded_object {
     realm::persisted<std::string> name;
     realm::persisted<bool> adopted;
 };
 ```
 */
struct embedded_object : public object_base {
};

template <typename T>
inline auto operator==(const T& lhs,
                       const T& rhs) -> std::enable_if_t<type_info::ObjectBasePersistableConcept<T>::value, bool> {
    if (lhs.is_managed() && rhs.is_managed()) {
        const Object& a = *lhs.m_object;
        const Object& b = *rhs.m_object;

        if (a.get_realm() != b.get_realm()) {
            return false;
        }

        const Obj obj1 = a.obj();
        const Obj obj2 = b.obj();
        // if table and index are the same
        return obj1.get_table() == obj2.get_table()
            && obj1.get_key() == obj2.get_key();
    }

    return false;
}

} // namespace realm

#endif /* realm_object_hpp */
