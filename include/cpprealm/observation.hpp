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

#ifndef CPPREALM_OBSERVATION_HPP
#define CPPREALM_OBSERVATION_HPP

#include <cpprealm/internal/bridge/list.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/object.hpp>
#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/macros.hpp>

#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/thread_safe_reference.hpp>

#include <iostream>
#include <variant>

namespace realm {

    template<typename T>
    struct object_change {
        using underlying = T;
        /// The object being observed.
        const T *object;
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

    template<typename T>
    struct object_change_callback_wrapper : internal::bridge::collection_change_callback {
        object_change_callback_wrapper(std::function<void(object_change < T > )> &&b,
                                    const T *obj,
                                    std::shared_ptr<internal::bridge::object> internal_object)
                : block(std::move(b)), object(*obj), m_object(internal_object) {
            static_cast<void>(obj);
        }
        std::function<void(object_change<T>)> block;
        const T object;
        std::shared_ptr<internal::bridge::object> m_object;

        std::optional<std::vector<std::string>> property_names = std::nullopt;
        std::optional<std::vector<typename decltype(T::schema)::variant_t>> old_values = std::nullopt;
        bool deleted = false;

        void populate_properties(internal::bridge::collection_change_set const &c) {
            if (property_names) {
                return;
            }
            if (!c.deletions().empty()) {
                deleted = true;
                return;
            }
            if (c.columns().empty()) {
                return;
            }

            auto properties = std::vector<std::string>();
            auto table = m_object->get_obj().get_table();

            for (auto i = 0; i < std::tuple_size<decltype(T::schema.properties)>{}; i++) {
                if (c.columns().count(table.get_column_key(T::schema.names[i]).value())) {
                    properties.push_back(T::schema.names[i]);
                }
            }

            if (!properties.empty()) {
                property_names = properties;
            }
        }

        std::optional<std::vector<typename decltype(T::schema)::variant_t>>
        read_values(internal::bridge::collection_change_set const &c) {
            if (c.empty()) {
                return std::nullopt;
            }
            populate_properties(c);
            if (!property_names) {
                return std::nullopt;
            }

            std::vector<typename decltype(T::schema)::variant_t> values;
            for (auto &name: *property_names) {
                auto value = T::schema.property_value_for_name(name, object, true);
                values.push_back(value);
            }
            return values;
        }

        void before(internal::bridge::collection_change_set const &c) override {
            old_values = read_values(c);
        }

        void after(internal::bridge::collection_change_set const &c) override {
            auto new_values = read_values(c);
            if (deleted) {
                forward_change(nullptr, {}, {}, {}, nullptr);
            } else if (new_values) {
                forward_change(&object,
                               *property_names,
                               old_values ? *old_values : std::vector<typename decltype(T::schema)::variant_t>{},
                               *new_values,
                               nullptr);
            }
            property_names = std::nullopt;
            old_values = std::nullopt;
        }

        void error(std::exception_ptr err) {
            forward_change(nullptr, {}, {}, {}, err);
        }

        void forward_change(const T *ptr,
                std::vector<std::string> property_names,
                std::vector<typename decltype(T::schema)::variant_t> old_values,
                std::vector<typename decltype(T::schema)::variant_t> new_values,
                const std::exception_ptr &error) {
            if (!ptr) {
                if (error) {
                    auto oc = object_change<T>();
                    oc.error = error;
                    block(std::forward<realm::object_change<T>>(std::move(oc)));
                } else {
                    auto oc = object_change<T>();
                    oc.is_deleted = true;
                    block(std::forward<realm::object_change<T>>(std::move(oc)));
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
                    property_changes.push_back(std::move(property));
                }
                auto oc = object_change<T>();
                oc.object = ptr;
                oc.property_changes = property_changes;
                block(std::forward<realm::object_change<T>>(std::move(oc)));
            }
        }
    };

    struct collection_change {
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

    struct collection_callback_wrapper : internal::bridge::collection_change_callback {
        std::function<void(collection_change)> handler;
        bool ignoreChangesInInitialNotification;

        collection_callback_wrapper(std::function<void(collection_change)> handler,
                                    bool ignoreChangesInInitialNotification)
            : handler(handler)
              , ignoreChangesInInitialNotification(ignoreChangesInInitialNotification)
        {}


        void before(const realm::internal::bridge::collection_change_set &) final {}
        void after(internal::bridge::collection_change_set const& changes) final {
            if (ignoreChangesInInitialNotification) {
                ignoreChangesInInitialNotification = false;
                handler({{},{},{}});
            }
            else if (changes.empty()) {
                handler({{},{},{}});

            }
            else if (!changes.collection_root_was_deleted() || !changes.deletions().empty()) {
                handler({to_vector(changes.deletions()),
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
}


template <typename T>
inline std::ostream& operator<< (std::ostream& stream, const realm::object_change<T>& object)
{
    stream << "{\n";
    stream << "\tis_deleted: " << object.is_deleted << "\n";
    stream << "\tchange_to: " << object.property.name << "\n";
    if (object.property.old_value) {
        stream << "\told_value: ";
        std::visit([&stream](auto&& arg) {
            using M = std::decay_t<decltype(arg)>;
            stream << static_cast<M>(arg);
        }, *object.property.old_value);
    }
    if (object.property.new_value) {
        stream << "\tnew_value: ";
        std::visit([&stream](auto&& arg) {
            using M = std::decay_t<decltype(arg)>;
            stream << static_cast<M>(arg);
        }, *object.property.new_value);
    }
    stream << "\n}";
    return stream;
}

#endif //CPPREALM_OBSERVATION_HPP
