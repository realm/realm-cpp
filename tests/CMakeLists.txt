Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.1 # or a later release
)
FetchContent_MakeAvailable(Catch2)

add_executable(cpprealm_sync_tests
        main.hpp
        main.cpp
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
            sync/test_objects.hpp
            sync/asymmetric_object_tests.cpp
            sync/flexible_sync_tests.cpp)

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
            db/frozen_tests.cpp)
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
            admin_utils.hpp
            admin_utils.cpp
            sync/test_objects.hpp
            sync/asymmetric_object_tests.cpp
            sync/flexible_sync_tests.cpp
            sync/app_tests.cpp
            sync/client_reset_tests.cpp)

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
            db/frozen_tests.cpp)

    target_compile_definitions(cpprealm_sync_tests PUBLIC CPPREALM_ENABLE_SYNC_TESTS)

endif()


target_link_libraries(cpprealm_sync_tests cpprealm Catch2::Catch2)
target_link_libraries(cpprealm_db_tests cpprealm Catch2::Catch2)

add_test(cpprealm_tests cpprealm_sync_tests cpprealm_db_tests)
enable_testing()