cmake_minimum_required(VERSION 3.16)

project(realm)

if("$ENV{REALM_CI}" OR REALM_USE_CCACHE)
    include(cmake/ccache.cmake)
endif()

# Project-wide build flags
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")

set(REALM_BUILD_LIB_ONLY ON)
add_subdirectory(realm-core)

set(REALM_INCLUDE_CERTS 0)
set(REALM_NO_CONFIG)
set(REALM_ENABLE_SYNC)
set(REALM_NO_TESTS)
set(REALM_BUILD_LIB_ONLY)
set(REALM_INSTALL_LIBEXECDIR)
set(REALM_ENABLE_ENCRYPTION 1)

add_compile_definitions(REALM_NO_CONFIG)
add_compile_definitions(REALM_ENABLE_SYNC)
add_compile_definitions(REALM_ENABLE_ENCRYPTION)
add_compile_definitions(REALM_INSTALL_LIBEXECDIR)
add_compile_definitions(REALM_BUILD_LIB_ONLY)
add_compile_definitions(REALM_VERSION_MAJOR=0)
add_compile_definitions(REALM_VERSION_MINOR=0)
add_compile_definitions(REALM_VERSION_PATCH=1)

add_subdirectory(src)
target_include_directories(cpprealm PUBLIC src)

# on Apple platforms we use the built-in CFRunLoop
# everywhere else it's libuv, except UWP where it doesn't build
if(NOT APPLE AND NOT WINDOWS_STORE AND NOT ANDROID)
    set(REALM_HAVE_UV 1)

    option(REALM_FETCH_MISSING_DEPENDENCIES "Download missing dependencies with CMake's FetchContent where possible" ON)
    list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/realm-core/tools/cmake")
    if(REALM_FETCH_MISSING_DEPENDENCIES)
        find_package(LibUV)
    else()
        find_package(LibUV REQUIRED)
    endif()
    list(POP_BACK CMAKE_MODULE_PATH)
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
    target_sources(cpprealm PRIVATE
        src/cpprealm/internal/apple/network_transport.mm
    )
elseif(ANDROID)
    set(REALM_ANDROID)
    target_sources(cpprealm PUBLIC
        src/cpprealm/internal/android/network_transport.cpp)
else()
    find_package(CURL REQUIRED)
    target_link_libraries(cpprealm PUBLIC
        CURL::libcurl
    )
    target_sources(cpprealm PRIVATE
        src/cpprealm/internal/curl/network_transport.cpp
    )
endif()

if (NOT ANDROID)
Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.1 # or a later release
)
FetchContent_MakeAvailable(Catch2)

add_executable(cpprealm_exe_tests tests/str_tests.cpp tests/list_tests.cpp tests/query_tests.cpp tests/main.hpp tests/sync_test_utils.hpp tests/test_objects.hpp tests/main.cpp tests/admin_utils.hpp tests/admin_utils.cpp tests/date_tests.cpp tests/mixed_tests.cpp tests/results_tests.cpp tests/binary_tests.cpp tests/flx_sync_tests.cpp tests/app_tests.cpp tests/embedded_object_tests.cpp tests/opt_tests.cpp)
target_link_libraries(cpprealm_exe_tests cpprealm Catch2::Catch2)
file(COPY tests/setup_baas.rb DESTINATION ${CMAKE_BINARY_DIR})
file(COPY tests/dependencies.list DESTINATION ${CMAKE_BINARY_DIR})
file(COPY tests/config_overrides.json DESTINATION ${CMAKE_BINARY_DIR})

add_test(cpprealm_tests cpprealm_exe_tests)
enable_testing()
endif()