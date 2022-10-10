cmake_minimum_required(VERSION 3.16)

project(realm)

if("$ENV{REALM_CI}" OR REALM_USE_CCACHE)
    include(cmake/ccache.cmake)
endif()

# Project-wide build flags
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")

set(REALM_BUILD_LIB_ONLY ON CACHE BOOL "")
add_subdirectory(realm-core EXCLUDE_FROM_ALL)

add_subdirectory(src)
add_subdirectory(tests)

target_include_directories(cpprealm PUBLIC src)
