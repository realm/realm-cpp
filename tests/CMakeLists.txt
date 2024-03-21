cmake_minimum_required(VERSION 3.16)
project(cpprealm_tests)
set(CMAKE_CXX_STANDARD 17)

Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.1 # or a later release
)
FetchContent_MakeAvailable(Catch2)

if (DEFINED USES_CONAN OR DEFINED VCPKG_OVERLAY_PORTS)
    set(BUILD_FROM_PACKAGE_MANAGER ON)
else()
    set(BUILD_FROM_PACKAGE_MANAGER OFF)
endif()

if (NOT BUILD_FROM_PACKAGE_MANAGER)
    add_executable(cpprealm_sync_tests
                    main.hpp
                    main.cpp
                    admin_utils.hpp
                    admin_utils.cpp
                    sync/test_objects.hpp
                    sync/asymmetric_object_tests.cpp
                    sync/flexible_sync_tests.cpp
                    sync/app_tests.cpp
                    sync/client_reset_tests.cpp)
endif()

add_executable(cpprealm_db_tests
                main.hpp
                main.cpp
                db/test_objects.hpp
                db/binary_tests.cpp
                db/date_tests.cpp
                db/decimal_tests.cpp
                db/embedded_object_tests.cpp
                db/list_tests.cpp
                db/map_tests.cpp
                db/mixed_tests.cpp
                db/object_id_tests.cpp
                db/object_tests.cpp
                db/optional_tests.cpp
                db/query_tests.cpp
                db/realm_tests.cpp
                db/results_tests.cpp
                db/run_loop_tests.cpp
                db/string_tests.cpp
                db/performance_tests.cpp
                db/numeric_tests.cpp
                db/set_tests.cpp
                db/frozen_tests.cpp
                db/uuid_tests.cpp)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:preprocessor /bigobj")
endif()

if (NOT BUILD_FROM_PACKAGE_MANAGER)
    target_compile_definitions(cpprealm_sync_tests PUBLIC CPPREALM_ENABLE_SYNC_TESTS)
endif()

if (DEFINED USES_CONAN)
    find_package(cpprealm REQUIRED)
    set(CPPREALM_TARGET cpprealm::cpprealm)
elseif(DEFINED VCPKG_OVERLAY_PORTS)
    find_package(cpprealm CONFIG REQUIRED)
    set(CPPREALM_TARGET cpprealm)
else()
    set(CPPREALM_TARGET cpprealm)
endif()

if(NOT APPLE AND NOT WINDOWS_STORE AND NOT ANDROID)
    if (DEFINED USES_CONAN)
        find_package(libuv REQUIRED)
    elseif(DEFINED VCPKG_OVERLAY_PORTS)
        find_package(libuv CONFIG REQUIRED)
        target_link_libraries(cpprealm_db_tests PUBLIC $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>)
    endif()
    target_compile_definitions(cpprealm_db_tests PUBLIC REALM_HAVE_UV)
endif()

if (MSVC AND ENABLE_STATIC)
    set_property(TARGET cpprealm_db_tests PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    if (NOT BUILD_FROM_PACKAGE_MANAGER)
        set_property(TARGET cpprealm_sync_tests PROPERTY
                MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()

    set_property(TARGET Catch2 PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if (NOT BUILD_FROM_PACKAGE_MANAGER)
    target_link_libraries(cpprealm_sync_tests PUBLIC ${CPPREALM_TARGET} Catch2::Catch2)
endif()
target_link_libraries(cpprealm_db_tests PUBLIC ${CPPREALM_TARGET} Catch2::Catch2)

if (BUILD_FROM_PACKAGE_MANAGER)
    add_test(cpprealm_tests cpprealm_db_tests)
else()
    add_test(cpprealm_tests cpprealm_sync_tests cpprealm_db_tests)
endif()

enable_testing()