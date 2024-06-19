NEXT RELEASE Release notes (YYYY-MM-DD)
=============================================================

### Fixed
* Fixed a compilation issue seen in MSVC 19.40.33811 due to usage of `std::apply`.

### Enhancements
<<<<<<< HEAD
* Add `realm::default_scheduler::set_default_factory(std::function<std::shared_ptr<realm::scheduler>()>&& factory_fn)` for generating a default scheduler. 
  Set your scheduler factory before instantiating a `realm::db_config`.
* Add `realm::default_scheduler::make_default()` which generates a platform default scheduler if `realm::default_scheduler::set_default_factory` is not set.
=======
* Add `managed<T>::to_json(std::ostream&)` which allows managed objects to be printed as json.
* Add `rbool::truepredicate()` and `rbool::falsepredicate()` expressions for type safe queries.
>>>>>>> e328fa1 (Add to_json)

### Compatibility
* Fileformat: Generates files with format v24. Reads and automatically upgrade from fileformat v10.

### Internals
* None

----------------------------------------------

2.0.1 Release notes (2024-06-03)
=============================================================

### Enhancements
* Add missing vendor support in Android Blueprint.

### Compatibility
* Fileformat: Generates files with format v24. Reads and automatically upgrade from fileformat v10.

### Internals
* Upgraded to Core v14.9.0

----------------------------------------------

2.0.0 Release notes (2024-05-24)
=============================================================

### Breaking changes
* `sync_manager::path_for_realm(const realm::sync_config&)` is now `App::path_for_realm(const realm::sync_config&)`

### Enhancements
* Added support for updating Atlas Device Sync's base url, in case the need to roam between servers (cloud and/or edge server). Add `-DREALM_ENABLE_EXPERIMENTAL=1` to your CMake command when generating the build to enable this feature.

### Compatibility
* Fileformat: Generates files with format v24. Reads and automatically upgrade from fileformat v10.

### Internals
* Upgraded to Core v14.7.0

----------------------------------------------

1.1.1 Release notes (2024-04-17)
=============================================================

### Fixed
* Building from source on Windows would fail due to symbolic links not being automatically 
  enabled on the platform ([#192](https://github.com/realm/realm-cpp/pull/192), since v1.1.0).
* Fixed a memory leak caused by incorrect usage of `util::UniqueFunction` inside of `std::function`.

### Enhancements
* Updated default base URL to be `https://services.cloud.mongodb.com` to support the new domains (was `https://realm.mongodb.com`)

### Compatibility
* Fileformat: Generates files with format v24. Reads and automatically upgrade from fileformat v10.

### Internals
* Upgraded to Core v14.5.1

----------------------------------------------

1.1.0 Release notes (2024-03-26)
=============================================================

### Fixed
* Managed objects would exhibit undefined behaviour when returned from the subscript operator in `std::vector` & `std::map`.
* Type safe queries would not work correctly when link properties were used.

### Enhancements
* Add `realm::holds_alternative` which acts as a substitute to `std::holds_alternative` when using `managed<realm::mixed>`.
* Add `managed<realm::mixed>::get_stored_link` for retrieving a link from a mixed proeprty type.
* Add `managed<realm::mixed>::set_link` for setting a link in a mixed proeprty type.
* Add compile time checking to prevent misuse of managed property types.
* Add `managed<std::vector<>>::as_results()` to allow the ability to derive a `realm::results<>` collection from a managed vector.
* Allow a `realm::uuid` to be constructed with `std::array<uint8_t, 16>`.
* Add support for integrating `cpprealm` with the Conan package manager.
* Add support for integrating `cpprealm` with the vcpkg package manager.
* Add BSON support for `user::call_function` and `user::get_custom_data` API's.
* Add `user::get_custom_data()` and deprecate `user::custom_data()`.

### Breaking Changes
* None

### Compatibility
* Fileformat: Generates files with format v23. Reads and automatically upgrade from fileformat v5.

### Internals
* Upgraded to Core v14.4.1

----------------------------------------------

1.0.0 Release notes (2024-01-08)
=============================================================

### Fixed
* When observing `results<>` the notifications would not fire after the `results<>` instance left the scope.
* Platform-agnostic network transport would pass the incorrect port value when using a proxy configuration.

### Enhancements
* Add the following methods for control over the sync session state:
  - `realm::sync_session::pause()`
  - `realm::sync_session::resume()`
  - `realm::sync_session::reconnect()`
  - `realm::sync_session::state()`
  - `realm::sync_session::connection_state()`
  - `realm::sync_session::observe_connection_change(std::function<void(enum connection_state old_state, 
                                                                       enum connection_state new_state)>&& callback)`
  - `realm::sync_session::unregister_connection_change_observer(uint64_t token)`
* Add support for the following client reset modes:
  - `realm::client_reset::manual()`
  - `realm::client_reset::discard_unsynced_changes(std::function<void(experimental::db local)> before, 
                                                   std::function<void(experimental::db local, experimental::db remote)> after)`
  - `realm::client_reset::recover_unsynced_changes(std::function<void(experimental::db local)> before,
                                                   std::function<void(experimental::db local, experimental::db remote)> after)`
  - `realm::client_reset::recover_or_discard_unsynced_changes(std::function<void(experimental::db local)> before,
                                                              std::function<void(experimental::db local, experimental::db remote)> after)`
* Add `realm::experimental::db::close()` for closing an open Realm.
* Add `realm::experimental::db::is_closed()` for checking if a Realm is closed.
* Add support for manual compaction via `realm::db_config::should_compact_on_launch(std::function<bool(uint64_t total_bytes, uint64_t unused_bytes)>&&)`.

### Breaking Changes
* The `experimental` namespace has been removed and all types under it have been elevated to the `realm` namespace.
* The Realm C++ 'Alpha' SDK has been removed.

### Compatibility
* Fileformat: Generates files with format v23. Reads and automatically upgrade from fileformat v5.

### Internals
* Upgraded to Core v13.25.1

----------------------------------------------

0.6.1 Release notes (2023-12-19)
=============================================================

### Fixed
* Building the Realm C++ SDK as an external library as part of AOSP would fail (since 0.5.0).

### Internals
* Upgraded to Core v13.25.0

----------------------------------------------

0.6.0 Release notes (2023-12-18)
=============================================================

### Fixed
* Platforms using the RealmCore network transport would get 
  a connection failure each time a network request is made. (since 0.5.0)

### Enhancements
* Add ability to thaw a frozen managed object via `managed<T>::thaw()`.
* Add ability to get the Realm associated with a managed object via `managed<T>::get_realm()`.

### Breaking Changes
* `realm::App::credentials::google(auth_code)` has been renamed to `realm::App::credentials::google_auth_code(const std::string&)`
* `realm::App::credentials::google(id_token)` has been renamed to `realm::App::credentials::google_id_token(const std::string&)`
* The following API's that used `BSON` have been replaced with `std::string`
  - `realm::App::credentails::function(const std::string&)`
  - `realm::App::user::call_function(const std::string&)`
  - `realm::App::user::custom_user_data()`

### Compatibility
* Fileformat: Generates files with format v23. Reads and automatically upgrade from fileformat v5.

### Internals
* Realm Core headers are no longer exposed through the Realm C++ SDK
* Upgraded to Core v13.24.1

----------------------------------------------

0.5.0 Release notes (2023-11-27)
=============================================================

### Enhancements
* Add support for Frozen Realm / Objects. An object can be made frozen by calling the `freeze()` method on the instance. 
  Subsequently, if you  can make a frozen Realm / Object live again by calling `thaw()`. 
  It is not recommended to have too many long-lived frozen Realm's / Objects in your application as it may balloon memory consumption.
* Add ability to sort `experimental::results` / `managed<std::vector<T>>`. 
* Add support for HTTP tunneling. Usage:
```cpp
    realm::proxy_config proxy_config;
    proxy_config.port = 8080;
    proxy_config.address = "127.0.0.1";
    proxy_config.username_password = {"username", "password"};

    realm::App::configuration app_config;
    app_config.proxy_configuration = proxy_config;
    auto app = realm::App(app_config);
    
    auto user = app.get_current_user();
    auto sync_config = user->flexible_sync_configuration();
    sync_config.set_proxy_config(proxy_config);
    auto synced_realm = experimental::db(sync_config);
```

### Compatibility
* Fileformat: Generates files with format v23. Reads and automatically upgrade from fileformat v5.

### Internals
* Upgraded to Core v13.23.4

----------------------------------------------

0.4.0 Release notes (2022-10-17)
=============================================================

### Fixed
* Primary keys could be changed after an object was inserted (since 0.1.0)
* Using a property type of vector of enums would cause a compilation error (since 0.1.0).
* Fixed a bug preventing SSL handshake from completing successfully due to failed hostname verification when linking against BoringSSL. (PR #7034)

### Enhancements
* The Sync metadata Realm is now encrypted by default on Apple platforms unless the `REALM_DISABLE_METADATA_ENCRYPTION` environment variable is set.
  To enable encryption on the metadata Realm on other platforms you must set an encryption key on `realm::App::configuration`.
```cpp
std::array<char, 64> example_key = {...};
realm::App::configuration app_config;
app_config.app_id = ...
app_config.metadata_encryption_key = example_key;
auto encrypted_app = realm::App(app_config);
```
* Add ability to encrypt a Realm. Usage: `realm::config::set_encryption_key(const std::array<char, 64>&)`.
* Add support for `std::set` in object models.

### Breaking Changes
* `realm::App(const std::string &app_id, const std::optional<std::string> &base_url,
              const std::optional<std::string> &path, const std::optional<std::map<std::string, std::string>> &custom_http_headers)` has been deprecated.
   use `realm::App(const realm::App::configuration&);` instead.

### Compatibility
* Fileformat: Generates files with format v23. Reads and automatically upgrade from fileformat v5.

### Internals
* Upgraded to Core v13.23.1

----------------------------------------------

0.3.0 Release notes (2023-09-15)
=============================================================

### Fixed
* Windows would not compile under Release build configuration as `aligned_storage` parameters were
  incorrectly set.
* Fix memory leak on internal::bridge::notification_token caused by missing destructor.
* Fix memory leak on internal::bridge::binary caused by wrong destructor being called.
* The default schema mode was incorrectly set to Automatic and not AdditiveDiscovered when using a Synced Realm.
* Fix iterator on `experimental::Results`
* Fix issue where properties on a link column could not be queried.
* `operator bool()` on link properties incorrectly returned true when the link was null.
* The default schema mode was incorrectly set to Automatic and not AdditiveDiscovered when using a Synced Realm.

### Enhancements
* Add support for the Decimal128 data type (`realm::decimal128`).
* Add app::get_current_user()
* Add user::is_logged_in()
* Add ability to set custom http headers. The http headers should be passed when constructing a `realm::App` and when in 
  possession of a config derived from `realm::user::flexible_sync_configuration()` by calling `foo_config.set_custom_http_headers(...);`.
* Add `operator!=()` to collections.
* Add `set_schema_version(uint64_t)`
* Add `managed<std::vector<T*>>::push_back(const managed<T*>&)`
* Add `box<managed<V*>>::box& operator=(const managed<V*>& o)`
* Add `box<managed<V*>>::box& operator=(const managed<V>& o)`
* Add `App::clear_cached_apps()` and `App::get_cached_app(const std::string& app_id, const std::optional<std::string>& base_url);`.

### Breaking Changes
* `managed<>::value()` has been renamed to `managed<>::detach()` to better convey that the returned value will be unmanaged. In the case where the value is a
  pointer type it is up to the consumer of the value to manage the lifetime of the object.
* `sync_session::wait_for_upload_completion` & `sync_session::wait_for_download_completion` now returns a `realm::status` in its callback instead of a `std::error_code`.
* `is_connection_level_protocol_error`, `is_session_level_protocol_error` & `is_client_error` has been removed from `realm::sync_error` and is replaced by
  `realm::sync_error::user_info()`, `realm::sync_error::compensating_writes_info()`, & `realm::sync_error::get_status()`.
* Data ingest Realms must now specify a schema containing the Asymmetric objects and their dependencies e.g `experimental::open<experimental::AllTypesAsymmetricObject, experimental::EmbeddedFoo>(user.flexible_sync_configuration())`.

### Compatibility
* Fileformat: Generates files with format v22.

### Internals
* Upgraded to Core v13.20.1

----------------------------------------------

0.2.0 Preview Release notes (2023-06-21)
=============================================================

### Enhancements
This preview introduces a new way to declare your object model, bringing you closer to feeling like you're interfacing with POCO's.
```
namespace realm::experimental {
    struct Address {
        std::string street;
        std::string city;
        std::string country;
    };
    REALM_EMBEDDED_SCHEMA(Address, street, city, country)

    struct Person {
        primary_key<int64_t> _id;
        std::string name;
        int64_t age;
        Address* address = nullptr;
    };
    REALM_SCHEMA(Person, _id, name, age, address)
}
```
This also allows for automatic schema discovery, so opening a realm is now achieved with `realm::experimental::db(std::move(some_realm_config)`.
These new features are available under the `realm::experimental` namespace.
New API usage is as follows:
```
  #include <cpprealm/experimental/sdk.hpp>
  using namespace realm::experimental;
  ...
  auto realm = db(std::move(config));
  
  auto address = Address();
  address.city = "New York";
  auto person = Person();
  person.name = "John";
  person.address = &address;
  
  auto managed_person = realm.write([&person, &realm] {
      return realm.add(std::move(person));
  });
  
  // Note that `person` has been consumed as an rvalue and `managed_person` 
  // should now be used for any data access or observation.
```

### Other API enhancements:

- Object links are now declared with pointer syntax.
- Added support for linking objects:
  ```
    struct Dog;
    struct Person {
        primary_key<int64_t> _id;
        ...
        Dog* dog;
    };
    REALM_SCHEMA(Person, _id, name, age, dog)
    struct Dog {
        ...
        linking_objects<&Person::dog> owners;
    };
  
    // Sample Usage:
    my_realm.objects<experimental::Dog>()[0].owners.size();
  ```
- Primary keys of `int64_t`, `std::string` and `realm::uuid` and their optional counterparts are now supported.

Usage of Realm under this namespace is also compatible with Windows (MSVC 19.30 and greater).

### Breaking Changes
The following functions now return `std::future` instead of `std::promise`
  - `realm::App::login`
  - `realm::App::register_user`
  - `realm::user::logout`,
  - `realm::sync_subscription_set::update`
  - `realm::sync_session::wait_for_upload_completion`
  - `realm::sync_session::wait_for_download_completion`

### Compatibility
* Fileformat: Generates files with format v22.

### Internals
* Upgraded to Core v13.15.1

----------------------------------------------

0.1.1 Release notes (2023-04-28)
=============================================================

### Fixed
* Address memory leaks reported by instruments caused by some classes not implementing rule of 5 when using
  `std::aligned_storage` ([#69](https://github.com/realm/realm-cpp/pull/69)), since v0.1.0).
* Dereferencing a property on an object link or embedded object would not return any value ([#71](https://github.com/realm/realm-cpp/pull/71)), since v0.1.0).

### Compatibility
* Fileformat: Generates files with format v22.

### Internals
* Upgraded to Core v13.9.2

----------------------------------------------
