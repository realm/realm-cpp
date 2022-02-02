cmake_minimum_required(VERSION 3.15)

project(realm)

# Project-wide build flags
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std=c++2a")

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

set(SOURCES
    src/cpprealm/sdk.cpp
    src/cpprealm/object.cpp
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
) # REALM_INSTALL_HEADERS

add_library(cpprealm STATIC ${SOURCES} ${HEADERS})
add_executable(cpprealm_exe_tests tests/tests.cpp tests/str_tests.cpp tests/list_tests.cpp tests/query_tests.cpp tests/test_utils.hpp tests/test_objects.hpp tests/test_utils.cpp)
#add_test(cpprealm_tests)

target_include_directories(cpprealm PRIVATE realm-core/src)
target_include_directories(cpprealm PUBLIC /usr/local/include)
target_include_directories(cpprealm PUBLIC /usr/include/c++/10)
target_include_directories(cpprealm PUBLIC src)
target_include_directories(cpprealm_exe_tests PUBLIC src)
target_include_directories(cpprealm_exe_tests PUBLIC realm-core/src)

target_sources(cpprealm PRIVATE ${SOURCES})
set_property(TARGET cpprealm PROPERTY CXX_STANDARD 20)
set_property(TARGET Storage PROPERTY REALM_BUILD_LIB_ONLY 1)
target_compile_options(cpprealm PUBLIC -std=c++2a)

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 10 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11)
target_compile_options(cpprealm PUBLIC -fcoroutines)
endif()

target_link_libraries(cpprealm Storage ObjectStore Sync z curl)

set_target_properties(cpprealm PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS cpprealm
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cpprealm)
target_link_libraries(cpprealm_exe_tests PUBLIC cpprealm z curl ObjectStore Sync Storage)
add_test(cpprealm_tests cpprealm_exe_tests)
enable_testing()