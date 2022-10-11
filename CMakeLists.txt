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

if(MSVC)
    # Core always defaults to statically-linking the C++ runtime library on Windows
    # but we shouldn't do this in the C++ SDK
    set_target_properties(Bid sha_win32 Storage Sync ObjectStore QueryParser PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )
endif()

add_subdirectory(src)
add_subdirectory(tests)

target_include_directories(cpprealm PUBLIC src)
