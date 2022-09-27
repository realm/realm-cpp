cmake_minimum_required(VERSION 3.16)

project(realm)

if(ENV{REALM_CI} OR REALM_USE_CCACHE)
    include(cmake/ccache.cmake)
endif()

# Project-wide build flags
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")

# Apple deployment targets
set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 14)
set(CMAKE_XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET 10.15)
set(CMAKE_XCODE_ATTRIBUTE_WATCHOS_DEPLOYMENT_TARGET 2)
set(CMAKE_XCODE_ATTRIBUTE_TVOS_DEPLOYMENT_TARGET 9)

set(REALM_BUILD_LIB_ONLY ON)
add_subdirectory(realm-core EXCLUDE_FROM_ALL)

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

set(SOURCES
    src/cpprealm/sdk.cpp
) # REALM_SOURCES

set(HEADERS
    src/cpprealm/sdk.hpp
    src/cpprealm/app.hpp
    src/cpprealm/db.hpp
    src/cpprealm/notifications.hpp
    src/cpprealm/object.hpp
    src/cpprealm/persisted.hpp
    src/cpprealm/results.hpp
    src/cpprealm/schema.hpp
    src/cpprealm/task.hpp
    src/cpprealm/thread_safe_reference.hpp
    src/cpprealm/type_info.hpp
    src/cpprealm/flex_sync.hpp
) # REALM_INSTALL_HEADERS

add_library(cpprealm SHARED ${SOURCES} ${HEADERS})
add_executable(cpprealm_exe_tests tests/tests.cpp tests/str_tests.cpp tests/list_tests.cpp tests/query_tests.cpp tests/test_utils.hpp tests/sync_test_utils.hpp tests/test_objects.hpp tests/test_utils.cpp tests/admin_utils.hpp tests/admin_utils.cpp)
#add_test(cpprealm_tests)

target_include_directories(cpprealm PRIVATE realm-core/src)
target_include_directories(cpprealm PUBLIC /usr/local/include)
target_include_directories(cpprealm PUBLIC /usr/include/c++/10)
target_include_directories(cpprealm PUBLIC src)
target_include_directories(cpprealm_exe_tests PUBLIC src)
target_include_directories(cpprealm_exe_tests PUBLIC realm-core/src)

target_sources(cpprealm PRIVATE ${SOURCES})
if (APPLE)
    target_sources(cpprealm PRIVATE src/cpprealm/network_transport.mm)
endif()
set_property(TARGET cpprealm PROPERTY CXX_STANDARD 20)
set_property(TARGET Storage PROPERTY REALM_BUILD_LIB_ONLY 1)
target_compile_options(cpprealm PUBLIC -std=c++2a)

# on Apple platforms we use the built-in CFRunLoop
# everywhere else it's libuv, except UWP where it doesn't build
if(NOT APPLE AND NOT WINDOWS_STORE)
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

set_target_properties(cpprealm PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS cpprealm
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cpprealm)
set(CMAKE_OSX_SYSROOT ${CMAKE_OSX_SYSROOT_PATH})

if (APPLE)
    target_link_options(cpprealm PUBLIC 
        "SHELL:-framework CFNetwork"
    )
else()
    find_package(CURL REQUIRED)
    target_link_libraries(cpprealm PUBLIC
        CURL::libcurl
    )
endif()

target_link_libraries(cpprealm_exe_tests cpprealm)

file(COPY tests/setup_baas.rb DESTINATION ${CMAKE_BINARY_DIR})
file(COPY tests/dependencies.list DESTINATION ${CMAKE_BINARY_DIR})
file(COPY tests/config_overrides.json DESTINATION ${CMAKE_BINARY_DIR})

add_test(cpprealm_tests cpprealm_exe_tests)
enable_testing()
