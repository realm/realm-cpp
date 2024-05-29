This acts as an integration test for the Realm C++ SDK being built as a vendor module in AOSP.

How to run:

- If AOSP has already been checked out and built then skip to step 2, otherwise follow step 1 below:

**Step 1**
- [Clone the AOSP repo](https://source.android.com/docs/setup/download)
- [Build AOSP with the following steps in the root directory](https://source.android.com/docs/setup/build/building)
    - `source build/envsetup.sh`
	- `lunch aosp_cf_x86_64_phone-trunk_staging-eng`
	- `m`

**Step 2**
- `cd` into the root of your AOSP source.
- `cd vendor`
- `git clone https://github.com/realm/realm-cpp.git`
- `cd realm-cpp`
- `git submodule update --init --recursive`
- `mma`
- `cd tests/aosp`
- `m realm_integration_test`
- This will generate realm_integration_test and place it in `out/target/product/vsoc_x86_64/vendor/bin/realm_integration_test`
- Copy the binary and upload it to an emulator using `adb push some_binary_location/realm_integration_test /data/local/tmp`
- Run `adb shell` and then run `su`
- run `cd /data/local/tmp` when inside of the emulators shell
- run `chmod +x realm_integration_test`
- run `./realm_integration_test`
- If the execution is successful you will see the output `"Realm C++ ran successfully."`

