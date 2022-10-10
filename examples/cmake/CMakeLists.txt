cmake_minimum_required(VERSION 3.15)

project (hello)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(../../ cmake-build-debug EXCLUDE_FROM_ALL)
add_executable(hello helloworld.cpp)
target_link_libraries(hello cpprealm)
