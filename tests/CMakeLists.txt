Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.1 # or a later release
)
FetchContent_MakeAvailable(Catch2)

if(MSVC)
    add_executable(cpprealm_sync_tests
            main.hpp
            main.cpp
            sync_test_utils.hpp
            admin_utils.hpp
            admin_utils.cpp
            experimental/sync/test_objects.hpp
            experimental/sync/asymmetric_object_tests.cpp
            experimental/sync/flexible_sync_tests.cpp)

    add_executable(cpprealm_db_tests
            main.hpp
            main.cpp
            experimental/db/test_objects.hpp
            experimental/db/binary_tests.cpp
            experimental/db/date_tests.cpp
            experimental/db/decimal_tests.cpp
            experimental/db/embedded_object_tests.cpp
            experimental/db/list_tests.cpp
            experimental/db/map_tests.cpp
            experimental/db/mixed_tests.cpp
            experimental/db/object_id_tests.cpp
            experimental/db/object_tests.cpp
            experimental/db/optional_tests.cpp
            experimental/db/query_tests.cpp
            experimental/db/realm_tests.cpp
            experimental/db/results_tests.cpp
            experimental/db/run_loop_tests.cpp
            experimental/db/string_tests.cpp
            experimental/db/frozen_tests.cpp)
    target_compile_definitions(cpprealm_sync_tests PUBLIC CPPREALM_ENABLE_SYNC_TESTS)

    set_property(TARGET cpprealm_sync_tests PROPERTY
      MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    set_property(TARGET cpprealm_db_tests PROPERTY
      MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    set_property(TARGET Catch2 PROPERTY
      MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()

    add_executable(cpprealm_sync_tests
            main.hpp
            main.cpp
            sync_test_utils.hpp
            admin_utils.hpp
            admin_utils.cpp
            experimental/sync/test_objects.hpp
            experimental/sync/asymmetric_object_tests.cpp
            experimental/sync/flexible_sync_tests.cpp
            experimental/sync/app_tests.cpp)

    add_executable(cpprealm_db_tests
            main.hpp
            main.cpp
            experimental/db/test_objects.hpp
            experimental/db/binary_tests.cpp
            experimental/db/date_tests.cpp
            experimental/db/decimal_tests.cpp
            experimental/db/embedded_object_tests.cpp
            experimental/db/list_tests.cpp
            experimental/db/map_tests.cpp
            experimental/db/mixed_tests.cpp
            experimental/db/object_id_tests.cpp
            experimental/db/object_tests.cpp
            experimental/db/optional_tests.cpp
            experimental/db/query_tests.cpp
            experimental/db/realm_tests.cpp
            experimental/db/results_tests.cpp
            experimental/db/run_loop_tests.cpp
            experimental/db/string_tests.cpp
            experimental/db/performance_tests.cpp
            experimental/db/numeric_tests.cpp
            experimental/db/set_tests.cpp
            experimental/db/frozen_tests.cpp)

    target_compile_definitions(cpprealm_sync_tests PUBLIC CPPREALM_ENABLE_SYNC_TESTS)

    if(ENABLE_ALPHA_SDK)
        add_executable(cpprealm_alpha_tests
                main.hpp
                main.cpp
                admin_utils.hpp
                admin_utils.cpp
                sync_test_utils.hpp
                alpha/str_tests.cpp
                alpha/list_tests.cpp
                alpha/query_tests.cpp
                alpha/test_objects.hpp
                alpha/date_tests.cpp
                alpha/mixed_tests.cpp
                alpha/results_tests.cpp
                alpha/binary_tests.cpp
                alpha/flx_sync_tests.cpp
                alpha/app_tests.cpp
                alpha/embedded_object_tests.cpp
                alpha/opt_tests.cpp
                alpha/run_loop_tests.cpp
                alpha/map_tests.cpp
                alpha/perf_tests.cpp
                alpha/object_tests.cpp
                alpha/object_id_tests.cpp
                alpha/asymmetric_object_tests.cpp
                alpha/realm_tests.cpp)

        target_link_libraries(cpprealm_alpha_tests cpprealm Catch2::Catch2)
        target_compile_definitions(cpprealm_alpha_tests PUBLIC CPPREALM_ENABLE_SYNC_TESTS)
    endif()

endif()

target_link_libraries(cpprealm_sync_tests cpprealm Catch2::Catch2)
target_link_libraries(cpprealm_db_tests cpprealm Catch2::Catch2)

file(COPY ../realm-core/evergreen DESTINATION ./${CMAKE_BUILD_TYPE})
file(MAKE_DIRECTORY baas)

add_test(cpprealm_tests cpprealm_sync_tests cpprealm_db_tests)
enable_testing()