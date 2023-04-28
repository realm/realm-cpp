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