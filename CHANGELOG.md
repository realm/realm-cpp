# NEXT RELEASE

### Enhancements

### Fixed
* Address memory leaks reported by instruments caused by some classes requiring dynamic memory allocation which
  overflowed their buffer set by `std::aligned_storage` ([#67](https://github.com/realm/realm-cpp/pull/67)), since v0.1.0).

### Breaking changes
* None.

### Compatibility
* Fileformat: Generates files with format v22.

-----------

### Internals
* None

----------------------------------------------
