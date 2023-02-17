cmake_minimum_required(VERSION 3.15)

project (hello)

set(CMAKE_CXX_STANDARD 17)
set(REALM_CPP_NO_TESTS 1)

if (USE_REALM_LOCAL)
    add_subdirectory(../../ cmake-build-debug)
else()
    message(STATUS "You did not define USE_REALM_LOCAL so we are assuming you have previously installed realm and
     realmcpp by building it from source and doing a global install as described in the READMEs")
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

if(NOT USE_REALM_LOCAL)
    find_library(REALMCPP_LIB
            NAMES cpprealm
    )
    find_library(REALM_LIB
            NAMES realm
    )
    message(STATUS "REALMCPP_LIB = ${REALMCPP_LIB}")
    message(STATUS "REALM_LIB = ${REALM_LIB}")
endif()


add_executable(hello helloworld.cpp)
target_link_libraries(hello
        cpprealm
        realm
        Threads::Threads
        OpenSSL::SSL
        CURL::libcurl
)

add_executable(example example.cpp)
target_link_libraries(example
        cpprealm
        realm
        Threads::Threads
        OpenSSL::SSL
        CURL::libcurl
)
