cmake_minimum_required(VERSION 3.15)

project(realm)

# Project-wide build flags
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")

# Apple deployment targets
set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 14)
set(CMAKE_XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET 10.15)
set(CMAKE_XCODE_ATTRIBUTE_WATCHOS_DEPLOYMENT_TARGET 2)
set(CMAKE_XCODE_ATTRIBUTE_TVOS_DEPLOYMENT_TARGET 9)

if(ENV{REALM_CI} OR REALM_USE_CCACHE)
    include(cmake/ccache.cmake)
endif()

set(REALM_BUILD_LIB_ONLY ON CACHE BOOL "")
add_subdirectory(realm-core EXCLUDE_FROM_ALL)

include_directories(src)

add_subdirectory(src/cpprealm)
add_subdirectory(tests)
