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

if (NOT DEFINED USES_CONAN)
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

if (NOT DEFINED USES_CONAN)
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

if(DEFINED CPPREALM_USE_UV)
    if (DEFINED USES_CONAN)
        find_package(libuv REQUIRED)
    elseif(DEFINED VCPKG_OVERLAY_PORTS)
        find_package(libuv CONFIG REQUIRED)
        target_link_libraries(cpprealm_db_tests PUBLIC $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>)
    else()
        include(FetchContent)
        set(liUV_Git_TAG "v1.48.0")
        FetchContent_Declare(
                libuv
                GIT_REPOSITORY https://github.com/libuv/libuv.git
                GIT_TAG ${libUV_Git_TAG}
        )
        FetchContent_Populate(libuv)
        add_subdirectory(${libuv_SOURCE_DIR} ${libuv_BINARY_DIR} EXCLUDE_FROM_ALL)
        set(libuv_LIBRARIES uv_a)
        target_link_libraries(${CPPREALM_TARGET} PUBLIC ${libuv_LIBRARIES})
    endif()
endif()

if (MSVC AND NOT DEFINED BUILD_SHARED_LIBS AND NOT DEFINED USES_CONAN)
    set_property(TARGET cpprealm_sync_tests PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    set_property(TARGET cpprealm_db_tests PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    set_property(TARGET Catch2 PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if (NOT DEFINED USES_CONAN)
    target_link_libraries(cpprealm_sync_tests PUBLIC ${CPPREALM_TARGET} Catch2::Catch2)
endif()
target_link_libraries(cpprealm_db_tests PUBLIC ${CPPREALM_TARGET} Catch2::Catch2)

if (DEFINED USES_CONAN)
    if (MSVC)
        target_link_libraries(cpprealm_db_tests PUBLIC Version)
    endif()
    add_test(cpprealm_tests cpprealm_db_tests)
else()
    add_test(cpprealm_tests cpprealm_sync_tests cpprealm_db_tests)
endif()

enable_testing()