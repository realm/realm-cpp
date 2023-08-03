#ifndef CPPREALM_OBSERVATION_HPP
#define CPPREALM_OBSERVATION_HPP

#include <cpprealm/experimental/macros.hpp>
#include <cpprealm/internal/bridge/object.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/bridge/list.hpp>

#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/thread_safe_reference.hpp>

#include <realm/object-store/util/scheduler.hpp>
#include <iostream>
#include <variant>

namespace realm::experimental {
    template<typename T>
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
    struct ObjectChangeCallbackWrapper : internal::bridge::collection_change_callback {
        ObjectChangeCallbackWrapper(std::function<void(object_change < T > )> &&b,
                                    const T *obj,
                                    std::shared_ptr<internal::bridge::object> internal_object)
                : block(std::move(b)), object(*obj), m_object(internal_object) {
            static_cast<void>(obj);
        }
        std::function<void(object_change < T > )> block;
        const T object;
        std::shared_ptr<internal::bridge::object> m_object;

        std::optional<std::vector<std::string>> property_names = std::nullopt;
        std::optional<std::vector<typename decltype(T::schema)::variant_t>> old_values = std::nullopt;
        bool deleted = false;

        void populateProperties(internal::bridge::collection_change_set const &c) {
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

            // FIXME: It's possible for the column key of a persisted property
            // FIXME: to equal the column key of a computed property.
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
        readValues(internal::bridge::collection_change_set const &c) {
            if (c.empty()) {
                return std::nullopt;
            }
            populateProperties(c);
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
            old_values = readValues(c);
        }

        void after(internal::bridge::collection_change_set const &c) override {
            auto new_values = readValues(c);
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
                    block(std::forward<realm::experimental::object_change<T>>(std::move(oc)));
                } else {
                    auto oc = object_change<T>();
                    oc.is_deleted = true;
                    block(std::forward<realm::experimental::object_change<T>>(std::move(oc)));
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
                block(std::forward<realm::experimental::object_change<T>>(std::move(oc)));
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


        void before(const realm::internal::bridge::collection_change_set &c) final {}
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

#if __cpp_coroutines
        template<typename T>
        struct ObjectChangeCallbackWrapper2;

        template <typename T>
        struct generator {
            struct promise_type;
            using handle_type = coro::coroutine_handle<promise_type>;

            struct promise_type // required
            {
                T value_;
                std::exception_ptr exception_;

                generator get_return_object() {
                    return generator(handle_type::from_promise(*this));
                }

                coro::suspend_always initial_suspend() { return {}; }

                coro::suspend_always final_suspend() noexcept {
                    std::cout << "generator::promise_type::final_suspend\n";
                    return {};
                }

                void unhandled_exception() { exception_ = std::current_exception(); } // saving
                // exception

                template <std::convertible_to<T> From>
                // C++20 concept
                coro::suspend_always yield_value(From &&from) {
                    value_ = std::forward<From>(from); // caching the result in promise
                    return {};
                }

                std::shared_ptr<ObjectChangeCallbackWrapper2<typename T::underlying>> v;

                void return_value(std::shared_ptr<ObjectChangeCallbackWrapper2<typename T::underlying>> val) {
                    v = val;
                }
            };

            handle_type h_;

            generator(handle_type h)
                    : h_(h) {
            }

            explicit operator bool() {
                fill();
                return !h_.done();
            }

            T operator()() {
                fill();
                full_ = false;
                return std::move(h_.promise().value_);
            }

            bool await_ready() { return false; }

            coro::coroutine_handle<> out_h;
        private:
            bool full_ = false;

            void fill() {
                if (!full_) {
                    h_();
                    if (h_.promise().exception_)
                        std::rethrow_exception(h_.promise().exception_);

                    full_ = true;
                }
            }
        };

        template<typename T>
        struct ObjectChangeCallbackWrapper2 : internal::bridge::collection_change_callback,
                                              std::enable_shared_from_this<ObjectChangeCallbackWrapper2<T>> {
            using std::enable_shared_from_this<ObjectChangeCallbackWrapper2<T>>::shared_from_this;

            ObjectChangeCallbackWrapper2(const T *obj,
                                         const internal::bridge::object &internal_object)
                    : object(obj),
                      m_object(internal_object.freeze(internal_object.get_realm().freeze())) {}

            internal::bridge::notification_token token;
            const T *object;
            internal::bridge::object m_object;

            std::optional<std::vector<std::string>> property_names = std::nullopt;
            std::optional<std::vector<typename decltype(T::schema)::variant_t>> old_values = std::nullopt;
            bool deleted = false;

            void populateProperties(internal::bridge::collection_change_set const &c) {
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
                auto table = m_object.obj().get_table();

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
            readValues(internal::bridge::collection_change_set const &c, bool is_old = false) {
                if (c.empty()) {
                    return std::nullopt;
                }
                populateProperties(c);
                if (!property_names) {
                    return std::nullopt;
                }

                std::vector<typename decltype(T::schema)::variant_t> values;
                for (auto &name: *property_names) {
                    if (is_old) {
                        auto value = T(std::move(m_object).obj(), object->m_realm).property_value_for_name(name);
                        values.push_back(value);
                    } else {
                        auto value = object->property_value_for_name(name);
                        values.push_back(value);
                    }
                }
                if (is_old) {
                    auto config = object->m_realm.get_config();
                    config.set_scheduler(realm::util::Scheduler::make_dispatch(dispatch_get_current_queue()));
                    auto realm = internal::bridge::realm(config);
                    realm.refresh();
                    m_object = internal::bridge::object(object->m_realm, object->m_obj).freeze(realm);
                }
                return values;
            }

            void before(internal::bridge::collection_change_set const &c) override {
                old_values = readValues(c, true);
            }

            std::mutex m;
            std::condition_variable cv;
            bool ready = false;
            bool processed = false;

            std::optional<std::vector<typename decltype(T::schema)::variant_t>> new_values;

            void after(internal::bridge::collection_change_set const &c) override {
                new_values = readValues(c);
                if (!deleted && !new_values) {
                    return;
                }
                processed = true;
                std::cout << "Worker thread signals data processing completed\n";
                cv.notify_one();
            }

            std::exception_ptr ptr;

            void error(const std::exception_ptr &err) {
                ptr = err;
            }

            generator<object_change<T>> forward_change() {
                while (true) {
                    std::cout << "FORWARD_CHANGE" << std::endl;
                    {
                        std::lock_guard lk(m);
                        ready = true;
                        std::cout << "main() signals data ready for processing\n";
                    }
                    cv.notify_one();
                    {
                        std::unique_lock lk(m); \
                    cv.wait(lk, [&] { return processed; }); \

                    }
                    std::cout << "Back in main()\n";


                    if (deleted) {
                        if (ptr) {
                            std::cout << "yield err\n";
                            co_yield realm::experimental::object_change<T>({.error = ptr});
                        } else {
                            std::cout << "yield deleted\n";
                            co_yield realm::experimental::object_change<T>({.is_deleted = true});
                        }
                    } else if (property_names) {
                        std::cout << "SIZE" << property_names->size() << std::endl;
                        for (size_t i = 0; i < property_names->size(); i++) {
                            realm::experimental::PropertyChange<T> property;
                            property.name = (*property_names)[i];
                            if (!old_values->empty()) {
                                property.old_value = (*old_values)[i];
                            }
                            if (!new_values->empty()) {
                                property.new_value = (*new_values)[i];
                            }
                            std::cout << "yield change\n";
                            co_yield realm::experimental::object_change<T>(
                                    {.object = object, .property = property});
                        }
                    } else {
                        std::cout << "yield nothing\n";
                    }
                    property_names = std::nullopt;
                    old_values = std::nullopt;
                    ready = false;
                    processed = false;
                }
                co_return shared_from_this();
            }
        };

        auto switch_to_new_thread(std::thread &out) {
            struct awaitable {
                std::thread *p_out;

                bool await_ready() { return false; }

                void await_suspend(coro::coroutine_handle<> h) {
                    std::thread &out = *p_out;
                    if (out.joinable())
                        throw std::runtime_error("Output jthread parameter not empty");
                    out = std::thread([h]() mutable { h.resume(); });
                    // Potential undefined behavior: accessing potentially destroyed *this
                    // std::cout << "New thread ID: " << p_out->get_id() << '\n';
                    std::cout << "New thread ID: " << out.get_id() << '\n'; // this is OK
                }

                void await_resume() {}
            };
            return awaitable{&out};
        }

        template<typename T>
        auto switch_to_new_queue(dispatch_queue_t &out) {
            struct awaitable {
                dispatch_queue_t *p_out;

                bool await_ready() { return false; }

                void await_suspend(coro::coroutine_handle<> h) {
                    dispatch_queue_t &out = *p_out;
                    __block auto _h = h;
                    __block auto loop = CFRunLoopGetCurrent();
                    dispatch_async(out, ^{
                        _h.resume();
                    });
                }

                void await_resume() {
                }
            };
            return awaitable{&out};
        }

    template<typename T>
    struct observe_task {
        observe_task(T *cls)
                : cls(cls)
                {}

        T *cls;
        dispatch_queue_t out = dispatch_queue_create("", nullptr);
        bool await_ready() { return false; }

        internal::bridge::notification_token token;
        std::shared_ptr<realm::experimental::ObjectChangeCallbackWrapper2<T>> wrapper;

        void await_suspend(coro::coroutine_handle<> h) {
            __block auto config = cls->m_realm.get_config();
            __block auto tbl_key = cls->m_obj.get_table().get_key();
            __block auto obj_key = cls->m_obj.get_key();
            __block auto _h = h;

            dispatch_async_and_wait(out, ^{
                config.set_scheduler(realm::util::Scheduler::make_dispatch(out));
                auto realm = internal::bridge::realm(config);
                auto table = realm.table_for_key(tbl_key);
                internal::bridge::object object{std::move(realm), table.get_object(obj_key)};
                wrapper = std::make_shared<realm::experimental::ObjectChangeCallbackWrapper2<T>>(
                    cls, object);
                wrapper->token = object.add_notification_callback(wrapper);
            });
            _h();
        }

        realm::experimental::generator<object_change<T>> await_resume() {
            std::cout << "resuming!\n";
            return wrapper->forward_change();
        }
    };
#endif

//    template<typename T>
//    struct observe_task {
//
//    };
//    namespace detail {
//        template <typename T>
//        observe_task<T> observe(T* cls) {
//
//        }
//    }
}


template <typename T>
inline std::ostream& operator<< (std::ostream& stream, const realm::experimental::object_change<T>& object)
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
