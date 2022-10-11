set(CATCH_CONFIG_NO_POSIX_SIGNALS ON)
add_compile_definitions(CATCH_CONFIG_NO_POSIX_SIGNALS=1)

add_subdirectory(../Catch2 ${CMAKE_CURRENT_BINARY_DIR}/Catch2 EXCLUDE_FROM_ALL)

add_executable(cpprealm-tests
    admin_utils.cpp
    admin_utils.hpp
    app_tests.cpp
    binary_tests.cpp
    date_tests.cpp
    embedded_object_tests.cpp
    flx_sync_tests.cpp
    list_tests.cpp
    main.cpp
    main.hpp
    mixed_tests.cpp
    opt_tests.cpp
    query_tests.cpp
    results_tests.cpp
    str_tests.cpp
    sync_test_utils.hpp
    test_objects.hpp
)

target_link_libraries(cpprealm-tests cpprealm Catch2::Catch2)

set(TEST_RESOURCES
    setup_baas.rb
    dependencies.list
    config_overrides.json
)
source_group("Resources" FILES ${TEST_RESOUCES})
target_sources(cpprealm-tests PRIVATE ${TEST_RESOURCES})

add_custom_command(TARGET cpprealm-tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TEST_RESOURCES} $<TARGET_FILE_DIR:cpprealm-tests> WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

if(CMAKE_SYSTEM_NAME MATCHES "^Windows")
    # Windows doesn't have anything like RPATH so in order to run the tests we need the shared library dependencies to be copied
    add_custom_command(TARGET cpprealm-tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:cpprealm-tests> $<TARGET_FILE_DIR:cpprealm-tests>
        COMMAND_EXPAND_LISTS
    )
endif()

add_test(cpprealm-tests cpprealm-tests)
enable_testing()
