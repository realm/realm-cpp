# Realm C++ SDK Release Process:

### What to check on GitHub Actions:
There are four main workflows:
- CMake build
- SPM
- Vcpkg
- Conan

#### CMake build:
This is where the majority of testing is done, tests run on macos, Windows and Ubuntu. On Ubuntu a variety of compilers are tested. Debug and Release configurations are tested on each platform.
DB tests, Sync tests and Client reset tests run separately as part of the workflow.
#### SPM:
Only DB tests are performed.
#### Vcpkg:
The Vcpkg tests are to ensure that nothing in our CMakeLists has changed to break the Vcpkg portfile. It will install the cpprealm Vcpkg and link it against the DB tests to ensure everything integrates fine.
#### Conan:
Similar to Vcpkg, the purpose of these tests is to ensure the Conanfile is setup correctly and it links correctly against the DB tests.

### AOSP:
Before each release check that everything builds fine on AOSP. It’s recommended to install AOSP on a Ubuntu workstation VM.
Clone the repo (can take a few hours)
https://source.android.com/docs/setup/download
Build android (also takes hours)
https://source.android.com/docs/setup/build/building
- `source build/envsetup.sh`
- `lunch aosp_cf_x86_64_phone-trunk_staging-eng`
- `m`
Once that is done, create a ‘vendor’ folder
All 3rd party libs should be cloned inside of `vendor`
Cd into your project folder and run `mma`

### Vcpkg:
Whenever Core has a new release we should take it upon ourselves to update the portfile on the vcpkg repo. The main steps are:
Fork Vcpkg
Create a branch for the Core update
Update the versions in the portfile, example [here](https://github.com/microsoft/vcpkg/pull/39888).

Once the Core update has performed, the same process should be taken for the C++ SDK. On Vcpkg the SDK is called cpprealm

### Conanfile:
We haven’t submitted to the Conan index yet, but users can consume the conanfile locally.

# Releasing from main:
- Ensure changelog is correct
- Ensure Core submodule is updated in both [Package.swift](https://github.com/realm/realm-cpp/blob/5ec1bda338dfd0c91ce1eea2ccb2c0adf7d86690/Package.swift#L5) and git submodules
- Ensure Conanfile.py is [pointing to the latest upstream](https://github.com/realm/realm-cpp/blob/main/conanfile.py#L39) and the [version is correct](https://github.com/realm/realm-cpp/blob/main/conanfile.py#L8).
- Ensure [version in local vcpkg portfile](https://github.com/realm/realm-cpp/blob/5ec1bda338dfd0c91ce1eea2ccb2c0adf7d86690/ports/vcpkg.json#L3) is correct
- Ensure the version in [dependencies.list](https://github.com/realm/realm-cpp/blob/5ec1bda338dfd0c91ce1eea2ccb2c0adf7d86690/dependencies.list#L1) is correct.
- Create a tag prefixed with v and publish the release.
- Copy the link for the release to appx-releases.
