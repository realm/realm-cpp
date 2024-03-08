cmake_minimum_required(VERSION 3.16)
project(cpprealm_tests)

Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.1 # or a later release
)
FetchContent_MakeAvailable(Catch2)

#add_executable(cpprealm_sync_tests
#                main.hpp
#                main.cpp
#                admin_utils.hpp
#                admin_utils.cpp
#                sync/test_objects.hpp
#                sync/asymmetric_object_tests.cpp
#                sync/flexible_sync_tests.cpp
#                sync/app_tests.cpp
#                sync/client_reset_tests.cpp)

add_executable(cpprealm_db_tests
                main.hpp
                main.cpp
                db/test_objects.hpp
                db/binary_tests.cpp
#                db/date_tests.cpp
#                db/decimal_tests.cpp
#                db/embedded_object_tests.cpp
#                db/list_tests.cpp
#                db/map_tests.cpp
#                db/mixed_tests.cpp
#                db/object_id_tests.cpp
#                db/object_tests.cpp
#                db/optional_tests.cpp
#                db/query_tests.cpp
#                db/realm_tests.cpp
#                db/results_tests.cpp
#                db/run_loop_tests.cpp
#                db/string_tests.cpp
#                db/performance_tests.cpp
#                db/numeric_tests.cpp
#                db/set_tests.cpp
#                db/frozen_tests.cpp
#                db/uuid_tests.cpp
        )

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:preprocessor /bigobj")
endif()

#target_compile_definitions(cpprealm_sync_tests PUBLIC CPPREALM_ENABLE_SYNC_TESTS)

if (DEFINED USES_CONAN)
    find_package(cpprealm CONFIG REQUIRED)
    find_package(libuv CONFIG REQUIRED)

    target_compile_definitions(cpprealm_db_tests PUBLIC REALM_HAVE_UV=1)
    target_compile_definitions(cpprealm_db_tests PUBLIC REALM_USE_UV=1)

    set(CPPREALM_TARGET cpprealm::cpprealm ${libuv_LIBRARIES})
else()
#     on Apple platforms we use the built-in CFRunLoop
#     everywhere else it's libuv, except UWP where it doesn't build
    if(NOT APPLE AND NOT WINDOWS_STORE AND NOT ANDROID)
        if(VCPKG_TOOLCHAIN)
            find_package(libuv CONFIG REQUIRED)
            if (BUILD_SHARED_LIBS)
                set(libuv_target libuv::uv)
            else()
                set(libuv_target libuv::uv_a)
            endif()
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
        endif()
    endif()

    if (MSVC)
#        set_property(TARGET cpprealm_sync_tests PROPERTY
#            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

        set_property(TARGET cpprealm_db_tests PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

        set_property(TARGET Catch2 PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()

    set(CPPREALM_TARGET cpprealm ${libuv_target})
endif()

# target_link_libraries(cpprealm_sync_tests ${CPPREALM_TARGET} Catch2::Catch2)
target_link_libraries(cpprealm_db_tests ${CPPREALM_TARGET} Catch2::Catch2)

# add_test(cpprealm_tests cpprealm_sync_tests cpprealm_db_tests)
add_test(cpprealm_tests cpprealm_db_tests)

enable_testing()