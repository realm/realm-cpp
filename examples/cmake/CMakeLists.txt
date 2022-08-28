cmake_minimum_required(VERSION 3.15)

project (hello)

set(CMAKE_CXX_STANDARD 20)

add_executable(hello helloworld.cpp)
target_include_directories(hello PUBLIC /usr/local/include)

if(APPLE)
    target_compile_definitions(hello PUBLIC REALM_DISABLE_METADATA_ENCRYPTION=1 REALM_ENABLE_SYNC=1 REALM_NO_CONFIG=1 REALM_ENABLE_ASSERTIONS=1 REALM_ENABLE_ENCRYPTION=1)
else()
    target_compile_definitions(hello PUBLIC REALM_HAVE_UV=1 REALM_DISABLE_METADATA_ENCRYPTION=1 REALM_ENABLE_SYNC=1 REALM_NO_CONFIG=1 REALM_ENABLE_ASSERTIONS=1 REALM_ENABLE_ENCRYPTION=1)
endif()

find_library (
    REALM_CORE_LIB
    NAMES realm-dbg librealm-dbg
)
message(STATUS "REALM_CORE_LIB: [${REALM_CORE_LIB}]")

find_library (
    REALM_SYNC_LIB
    NAMES realm-sync-dbg librealm-sync-dbg
)
message(STATUS "REALM_SYNC_LIB: [${REALM_SYNC_LIB}]")

find_library (
    REALM_CORE_PARSER_LIB
    NAMES realm-parser-dbg librealm-parser-dbg
)
message(STATUS "REALM_CORE_PARSER_LIB: [${REALM_CORE_PARSER_LIB}]")

find_library (
    REALM_OBJECT_STORE_LIB
    NAMES realm-object-store-dbg librealm-object-store-dbg
)
message(STATUS "REALM_OBJECT_STORE_LIB: [${REALM_OBJECT_STORE_LIB}]")

find_library (
    CPP_REALM_LIB
    NAMES cpprealm libcpprealm
)
message(STATUS "CPP_REALM_LIB: [${CPP_REALM_LIB}]")

if(APPLE)
    target_link_libraries(hello "-lcompression" "-framework Foundation" "-framework Security")
    target_link_libraries(hello ${CPP_REALM_LIB} ${REALM_OBJECT_STORE_LIB} ${REALM_SYNC_LIB} ${REALM_CORE_LIB} ${REALM_CORE_PARSER_LIB} z curl stdc++)
else()

    find_package(LibUV)

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

    get_property(libuv_include_dir TARGET ${libuv_target} PROPERTY INCLUDE_DIRECTORIES)
    target_include_directories(hello PRIVATE ${libuv_include_dir})

    set(OPENSSL_USE_STATIC_LIBS ON)
    # Use our own prebuilt OpenSSL
    if(NOT OpenSSL_DIR)
        if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/openssl/lib/cmake/OpenSSL/OpenSSLConfig.cmake)
            set(OPENSSL_URL "http://static.realm.io/downloads/openssl/1.1.1n/Linux/x86_64/openssl.tar.gz")

            message(STATUS "Getting ${OPENSSL_URL}...")
            file(DOWNLOAD "${OPENSSL_URL}" "${CMAKE_CURRENT_BINARY_DIR}/openssl/openssl.tar.gz" STATUS download_status)

            list(GET download_status 0 status_code)
            if (NOT "${status_code}" STREQUAL "0")
                message(FATAL_ERROR "Downloading ${url}... Failed. Status: ${download_status}")
            endif()

            message(STATUS "Uncompressing OpenSSL...")
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar xfz "openssl.tar.gz"
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/openssl"
            )
        endif()

        set(OpenSSL_DIR "${CMAKE_CURRENT_BINARY_DIR}/openssl/lib/cmake/OpenSSL")
    endif()
    find_package(OpenSSL REQUIRED CONFIG)
    target_link_libraries(OpenSSL::SSL INTERFACE OpenSSL::Crypto)

    set(REALM_HAVE_OPENSSL ON)

    target_link_libraries(hello PUBLIC ${CPP_REALM_LIB} ${REALM_OBJECT_STORE_LIB} ${REALM_SYNC_LIB} ${REALM_CORE_LIB} ${REALM_CORE_PARSER_LIB} OpenSSL::SSL z curl ${libuv_target} stdc++)
endif()


