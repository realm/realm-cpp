add_executable(cpprealm-tests
    admin_utils.cpp
    admin_utils.hpp
    list_tests.cpp
    query_tests.cpp
    str_tests.cpp
    sync_test_utils.hpp
    test_objects.hpp
    test_utils.cpp
    test_utils.hpp
    tests.cpp
)

target_link_libraries(cpprealm-tests cpprealm)

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

add_test(cpprealm-tests cpprealm-tests)
enable_testing()
