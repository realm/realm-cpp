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