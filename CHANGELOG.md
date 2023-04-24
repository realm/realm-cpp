# NEXT RELEASE

### Enhancements

### Fixed
* Address memory leaks reported by instruments caused by some classes not implementing rule of 5 when using
  `std::aligned_storage` ([#69](https://github.com/realm/realm-cpp/pull/69)), since v0.1.0).

### Breaking changes
* `scheduler::invoke(std::function<void()>)` has been change to `scheduler::invoke(realm::Function<void()>)`.

### Compatibility
* Fileformat: Generates files with format v22.

-----------

### Internals
* Upgraded to Core v13.9.2

----------------------------------------------