You will first need to create a [Atlas Device Sync App and turn development mode on for it.](https://www.mongodb.com/docs/atlas/app-services/sync/get-started/)
Make sure to enable anonymous authentication.

Change to the example project directory.  
```
mkdir build.debug  
cd build.debug  
cmake -D CMAKE_BUILD_TYPE=debug ..  
cmake --build .  
./hello
```