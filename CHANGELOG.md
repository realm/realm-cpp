# NEXT RELEASE

### Enhancements
* Add support for Embedded Objects. Users can implement their own embedded objects by inheriting from `realm::embedded_object`. 
* Add support for notifications on `results` collections by calling `results::observe`.
* Add support for calling a Device Sync function by calling `user::call_function(realm::bson::Bson)`.
* Add support for accessing custom user data via `user::custom_user_data()` and `user::refresh_custom_user_data()`.

### Fixed
* None

### Breaking changes
* None.

### Compatibility
* Fileformat: Generates files with format v22. Reads and automatically upgrade from fileformat v5.

-----------

### Internals
* None

----------------------------------------------
