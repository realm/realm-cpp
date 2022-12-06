## Building an Android App with Realm C++ SDK 
- Add `<uses-permission android:name="android.permission.INTERNET" />` to your AndroidManifest.xml 
- Add the subdirectory of the Realm C++ SDK to your native library's CMakeLists.txt e.g: 
```
add_subdirectory("realm-cpp")
...
target_link_libraries( # Specifies the target library.
        myapplication
        # make sure to link the Realm C++ SDK.
        cpprealm)
``` 
- Ensure that the git submodules are initialized inside of the realm-cpp folder before building. 
- When instantiating the Realm or the Realm App you must pass the `filesDir.path` to the `path` parameter in the respective constructors. 
- See the MainActivity.kt in the Android example app for reference.
