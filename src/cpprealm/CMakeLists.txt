set(SOURCES
    sdk.cpp
)

set(HEADERS
    app.hpp
    db.hpp
    flex_sync.hpp
    notifications.hpp
    object.hpp
    persisted/binary.hpp
    persisted/bool.hpp
    persisted/embedded.hpp
    persisted/enum.hpp
    persisted/int.hpp
    persisted/list.hpp
    persisted/mixed.hpp
    persisted/optional.hpp
    persisted/string.hpp
    persisted/timestamp.hpp
    persisted/uuid.hpp
    persisted.hpp
    results.hpp
    schema.hpp
    sdk.hpp
    task.hpp
    thread_safe_reference.hpp
    type_info.hpp
)

add_library(cpprealm SHARED ${SOURCES} ${HEADERS})

target_link_libraries(cpprealm PUBLIC Realm::ObjectStore)

set_target_properties(cpprealm PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS cpprealm
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cpprealm
)

if (APPLE)
    target_sources(cpprealm PRIVATE 
        internal/apple/network_transport.mm
    )
else()
    if(VCPKG_TOOLCHAIN)
        find_package(libuv CONFIG)
        set(UV_LIBRARY $<IF:$<TARGET_EXISTS:uv_a>,uv_a,uv>)
    endif()

    if(NOT libuv_FOUND)
        list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/realm-core/tools/cmake")
        find_package(LibUV REQUIRED)
        list(POP_BACK CMAKE_MODULE_PATH)
        set(UV_LIBRARY LibUV::LibUV)
    endif()

    find_package(CURL REQUIRED)
    target_link_libraries(cpprealm PUBLIC
        CURL::libcurl
        ${UV_LIBRARY}
    )
    target_sources(cpprealm PRIVATE
        internal/curl/network_transport.cpp
    )

    target_compile_definitions(cpprealm PUBLIC
        CPPREALM_USE_LIBUV=1
    )
endif()