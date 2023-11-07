cmake_minimum_required(VERSION 3.16)

project(RealmCxx)

if("$ENV{REALM_CI}" OR REALM_USE_CCACHE)
    include(cmake/ccache.cmake)
endif()

# Project-wide build flags
set(CMAKE_CXX_STANDARD 17)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:preprocessor /bigobj")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

set(REALM_BUILD_LIB_ONLY ON)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(REALM_USE_SYSTEM_OPENSSL ON)
    set(OPENSSL_USE_STATIC_LIBS OFF)
endif()

add_subdirectory(realm-core)

set(REALM_NO_CONFIG)
set(REALM_ENABLE_SYNC)
set(REALM_NO_TESTS)
set(REALM_BUILD_LIB_ONLY)
set(REALM_INSTALL_LIBEXECDIR)
set(REALM_ENABLE_ENCRYPTION 1)

list(APPEND CMAKE_MODULE_PATH
        "${RealmCxx_SOURCE_DIR}/tools/cmake" # for GetVersion
        "${RealmCxx_SOURCE_DIR}/realm-core/tools/cmake") # for GetGitRevisionDescription
if(NOT DEFINED REALMCXX_VERSION)
    # Get accurate git-describe version
    include(GetGitRevisionDescription)
    git_describe(REALMCXX_VERSION)
endif()

add_compile_definitions(REALM_NO_CONFIG)
add_compile_definitions(REALM_ENABLE_SYNC)
add_compile_definitions(REALM_ENABLE_ENCRYPTION)
add_compile_definitions(REALM_INSTALL_LIBEXECDIR)
add_compile_definitions(REALM_BUILD_LIB_ONLY)

# Check if the compiler is Clang or GCC >= 11
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "11.0"))
    set(ENABLE_ALPHA_SDK TRUE)
else()
    set(ENABLE_ALPHA_SDK FALSE)
endif()

add_subdirectory(src)
target_include_directories(cpprealm PUBLIC src)

# on Apple platforms we use the built-in CFRunLoop
# everywhere else it's libuv, except UWP where it doesn't build
if(NOT APPLE AND NOT WINDOWS_STORE AND NOT ANDROID)
    set(REALM_HAVE_UV 1)

    option(REALM_FETCH_MISSING_DEPENDENCIES "Download missing dependencies with CMake's FetchContent where possible" ON)
    if(REALM_FETCH_MISSING_DEPENDENCIES)
        find_package(LibUV)
    else()
        find_package(LibUV REQUIRED)
    endif()
    if(LibUV_FOUND)
        set(libuv_target LibUV::LibUV)
    elseif(REALM_FETCH_MISSING_DEPENDENCIES)
        message(STATUS "LibUV not found, building from source with FetchContent")
        include(FetchContent)
        set(libUV_Git_TAG "v1.35.0")
        if(MSVC)
            set(liUV_Git_TAG "v1.43.0")
        endif()
        FetchContent_Declare(
            libuv
            GIT_REPOSITORY https://github.com/libuv/libuv.git
            GIT_TAG ${libUV_Git_TAG}
        )
        FetchContent_Populate(libuv)
        add_subdirectory(${libuv_SOURCE_DIR} ${libuv_BINARY_DIR} EXCLUDE_FROM_ALL)
        set(libuv_target uv_a)
    endif()
    
    target_link_libraries(cpprealm PUBLIC ${libuv_target})

    target_compile_definitions(cpprealm PUBLIC REALM_DISABLE_METADATA_ENCRYPTION=1 REALM_ENABLE_SYNC=1)
    target_compile_definitions(ObjectStore PUBLIC REALM_HAVE_UV=1)
    target_link_libraries(ObjectStore PUBLIC ${libuv_target})

else()
    target_compile_definitions(cpprealm PUBLIC REALM_ENABLE_SYNC=1)
endif()

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 10 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11)
target_compile_options(cpprealm PUBLIC -fcoroutines)
endif()

target_link_libraries(cpprealm PUBLIC Realm::ObjectStore)
set(CMAKE_OSX_SYSROOT ${CMAKE_OSX_SYSROOT_PATH})

if (APPLE)
#    target_link_libraries(cpprealm PUBLIC "-framework Foundation")
#    target_link_libraries(cpprealm PUBLIC "-framework CoreFoundation")
#    target_link_libraries(cpprealm PUBLIC "-ObjC")
#    target_link_libraries(cpprealm PUBLIC objc)
#    add_framework(cpprealm )
#    enable_language(OBJCXX)
#    add_framework(Foundation cpprealm)
#    add_definitions("-ObjC++")
    target_compile_options(cpprealm PUBLIC -fno-objc-msgsend-selector-stubs)
    target_sources(cpprealm PRIVATE src/cpprealm/internal/apple/network_transport.mm)
elseif(ANDROID)
    set(REALM_ANDROID)
    target_sources(cpprealm PRIVATE src/cpprealm/internal/network/network_transport.cpp)
elseif(MSVC)
    target_sources(cpprealm PRIVATE src/cpprealm/internal/network/network_transport.cpp)
else()
    find_package(CURL)
    if(NOT CURL_FOUND)
        message(WARNING "CURL not found. Realm C++ will use internal networking components instead.")
        target_sources(cpprealm PRIVATE src/cpprealm/internal/network/network_transport.cpp)
    else()
        target_link_libraries(cpprealm PUBLIC CURL::libcurl)
        target_sources(cpprealm PRIVATE src/cpprealm/internal/curl/network_transport.cpp)
    endif()
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        target_link_libraries(cpprealm PRIVATE stdc++fs)
    endif()
endif()

if(MSVC)
    set_property(TARGET cpprealm PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET uv_a PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if (NOT REALM_CPP_NO_TESTS)
    add_subdirectory(tests)
endif()