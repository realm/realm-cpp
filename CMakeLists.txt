cmake_minimum_required(VERSION 3.16)

project(cpprealm)

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
endif()


if (VCPKG_TOOLCHAIN)
    find_package(Realm REQUIRED)
else()
    add_subdirectory(realm-core)
endif()

set(REALM_NO_CONFIG)
set(REALM_ENABLE_SYNC)
set(REALM_NO_TESTS)
set(REALM_BUILD_LIB_ONLY)
set(REALM_INSTALL_LIBEXECDIR)
set(REALM_ENABLE_ENCRYPTION 1)
option(REALM_ENABLE_EXPERIMENTAL "Enable experimental Realm API's" OFF)
if(REALM_ENABLE_EXPERIMENTAL)
    add_compile_definitions(REALM_ENABLE_EXPERIMENTAL)
endif()

set(CMAKE_DEBUG_POSTFIX "-dbg")

list(APPEND CMAKE_MODULE_PATH
        "${cpprealm_SOURCE_DIR}/tools/cmake" # for GetVersion
        "${cpprealm_SOURCE_DIR}/realm-core/tools/cmake") # for GetGitRevisionDescription
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

# on Apple platforms we use the built-in CFRunLoop
# everywhere else it's libuv, except UWP where it doesn't build
if(NOT APPLE AND NOT WINDOWS_STORE AND NOT ANDROID)
    set(REALM_HAVE_UV 1)
    add_compile_definitions(REALM_HAVE_UV=1)
endif()

configure_file(src/cpprealm/util/config.h.in include/cpprealm/util/config.h)

add_subdirectory(src)
target_include_directories(${PROJECT_NAME}
        PRIVATE
        # where the library itself will look for its internal headers
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>

        # where external projects will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )

if(REALM_HAVE_UV)
    if(VCPKG_TOOLCHAIN)
        find_package(libuv CONFIG REQUIRED)
        if(BUILD_SHARED_LIBS)
            set(libuv_target libuv::uv)
        else()
            set(libuv_target libuv::uv_a)
        endif()
    elseif(DEFINED USES_CONAN)
        find_package(libuv REQUIRED)
        set(libuv_target ${libuv_LIBRARIES})
    else()
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
            set(libUV_Git_TAG "v1.48.0")
            FetchContent_Declare(
                    libuv
                    GIT_REPOSITORY https://github.com/libuv/libuv.git
                    GIT_TAG ${libUV_Git_TAG}
            )
            FetchContent_Populate(libuv)
            add_subdirectory(${libuv_SOURCE_DIR} ${libuv_BINARY_DIR} EXCLUDE_FROM_ALL)
            set(libuv_target uv_a)
        endif()
    endif()

    target_link_libraries(cpprealm PUBLIC ${libuv_target})
endif()

target_link_libraries(cpprealm PUBLIC Realm::ObjectStore)
set(CMAKE_OSX_SYSROOT ${CMAKE_OSX_SYSROOT_PATH})

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(cpprealm PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(cpprealm PRIVATE stdc++fs)
endif()

if(MSVC AND NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
    if(NOT BUILD_SHARED_LIBS)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        set_property(TARGET cpprealm PROPERTY
                MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        set_property(TARGET uv_a PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

install(EXPORT "${PROJECT_NAME}Targets"
        FILE "${PROJECT_NAME}Targets.cmake"
        DESTINATION cmake
        )

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION "${REALMCXX_VERSION}"
        COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION cmake
        )
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION cmake
        )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/cpprealm/util/config.h
        DESTINATION include/cpprealm/util
        COMPONENT devel)

if (BUILD_SHARED_LIBS)
    set_target_properties(cpprealm
    PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )

    get_target_property(_VisPreset cpprealm CXX_VISIBILITY_PRESET)
    if (NOT MSVC AND _VisPreset STREQUAL "hidden")
        set_target_properties(cpprealm
        PROPERTIES
            CXX_VISIBILITY_PRESET "default"
            VISIBILITY_INLINES_HIDDEN OFF
        )
        message(WARNING "Setting cpprealm's visibility to default."
                        " Hidden visibility is not supported.")
    endif()
endif()

if (NOT REALM_CPP_NO_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()