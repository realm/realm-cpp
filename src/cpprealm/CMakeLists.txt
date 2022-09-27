set(SOURCES
    sdk.cpp
) # REALM_SOURCES

set(HEADERS
    app.hpp
    db.hpp
    flex_sync.hpp
    notifications.hpp
    object.hpp
    persisted.hpp
    results.hpp
    schema.hpp
    sdk.hpp
    task.hpp
    thread_safe_reference.hpp
    type_info.hpp
) # REALM_INSTALL_HEADERS

add_library(cpprealm SHARED ${SOURCES} ${HEADERS})

if(CMAKE_SYSTEM_NAME MATCHES "^Windows")
    target_compile_definitions(cpprealm PRIVATE
        WIN32_LEAN_AND_MEAN # include minimal Windows.h for faster builds
        UNICODE # prefer Unicode variants of Windows APIs over ANSI variants
        _UNICODE # prefer Unicode variants of C runtime APIs over ANSI variants
    )
    if(NOT WINDOWS_STORE)
        # for regular Windows target Windows 8.1 as the minimum version
        # see https://docs.microsoft.com/en-us/windows/win32/WinProg/using-the-windows-headers
        target_compile_definitions(cpprealm PRIVATE
            _WIN32_WINNT=0x0603
            WINVER=0x603
            NTDDI_VERSION=0x06030000
        )
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES GNU AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 10 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11)
    target_compile_options(cpprealm PUBLIC -fcoroutines)
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(cpprealm PUBLIC -stdlib=libc++)
    target_link_options(cpprealm PUBLIC -stdlib=libc++)
endif()

# Network transport dependencies
if(APPLE)
    target_link_options(cpprealm PUBLIC "SHELL:-framework CFNetwork")
    target_sources(cpprealm PRIVATE
        internal/apple/default_network_transport.cpp
    )
else()
    if(VCPKG_TOOLCHAIN)
        find_package(libuv CONFIG)
        set(UV_LIBRARY uv_a)
    endif()

    if(NOT libuv_FOUND)
        list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/realm-core/tools/cmake")
        find_package(LibUV REQUIRED)
        list(POP_BACK CMAKE_MODULE_PATH)
        set(UV_LIBRARY LibUV::LibUV)
    endif()
    
    find_package(CURL REQUIRED)

    target_compile_definitions(cpprealm PUBLIC
        REALM_DISABLE_METADATA_ENCRYPTION=1
        REALM_HAVE_UV=1
    )

    target_link_libraries(cpprealm PUBLIC 
        ${UV_LIBRARY}
        CURL::libcurl
    )

    target_sources(cpprealm PRIVATE
        internal/curl/default_network_transport.cpp
    )
endif()

target_link_libraries(cpprealm PUBLIC Realm::ObjectStore)

set_target_properties(cpprealm PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS cpprealm
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cpprealm)
