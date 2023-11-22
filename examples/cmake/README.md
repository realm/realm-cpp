You will first need to create a [Atlas Device Sync App and turn development mode on for it.](https://www.mongodb.com/docs/atlas/app-services/sync/get-started/)
Make sure to enable anonymous authentication.

set `USE_REALM_LOCAL` to build everything from source otherwise you must have previously installed realm-cpp.

Linux platforms require the following packages:
```
sudo apt-get install -y \
    libcurl4-openssl-dev \
    libssl-dev \
    libuv1-dev \
    ninja-build \
    zlib1g-dev
```

Change to the example project directory.  
```
mkdir build.debug  
cd build.debug  
cmake -GNinja -D CMAKE_BUILD_TYPE=debug ..  
ninja 
./hello
```