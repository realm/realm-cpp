cmake_minimum_required(VERSION 3.15)

project (realm_helloworld)

set(CMAKE_CXX_STANDARD 17)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:preprocessor /bigobj")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()
add_executable(realm_helloworld helloworld.cpp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../realm-cpp ${CMAKE_CURRENT_BINARY_DIR}/realm-cpp)

target_link_libraries(realm_helloworld cpprealm)

if(MSVC)
    set_property(TARGET realm_helloworld PROPERTY
                 MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()