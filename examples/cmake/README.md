Make sure cpprealm is installed before hand:

git clone https://github.com/realm/realm-cpp.git
cd realm-cpp
git checkout lm/update
git submodule update --init --recursive
cd realm-core
git checkout 55a48c287b5e3a8ca129c257ec7e3b92bcb2a05f
cd ../
mkdir build.debug
cd build.debug
cmake -D CMAKE_BUILD_TYPE=debug ..
sudo cmake --build . --target install

Then build the sample project:

Change to the sample project directory.
mkdir build.debug
cd build.debug
cmake -D CMAKE_BUILD_TYPE=debug ..
cmake --build . 
