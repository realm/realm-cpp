cmake_minimum_required(VERSION 3.15)

project (realm_helloworld)

set(CMAKE_CXX_STANDARD 17)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:preprocessor /bigobj")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()
add_executable(realm_helloworld helloworld.cpp)

if (DEFINED VCPKG_OVERLAY_PORTS)
    find_package(cpprealm REQUIRED)
else()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../realm-cpp ${CMAKE_CURRENT_BINARY_DIR}/realm-cpp)
endif()

target_link_libraries(realm_helloworld cpprealm)

if(MSVC AND DEFINED ENABLE_STATIC)
    set_property(TARGET realm_helloworld PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()