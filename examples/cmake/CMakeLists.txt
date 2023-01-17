cmake_minimum_required(VERSION 3.15)

project(hello LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CPPREALM_NO_TESTS 1)
add_subdirectory(../../ cpprealm-build)

add_executable(hello helloworld.cpp)
target_link_libraries(hello cpprealm)
