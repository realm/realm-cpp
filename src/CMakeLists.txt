add_subdirectory(cpprealm/internal/bridge/generator)

include(GetVersion)
# Configure source code to use right version number
configure_file(cpprealm/internal/version_numbers.hpp.in cpprealm/version_numbers.hpp)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cpprealm/version_numbers.hpp
        DESTINATION include/cpprealm
        COMPONENT devel)

set(SOURCES
    cpprealm/analytics.cpp
    cpprealm/app.cpp
    cpprealm/bson.cpp
    cpprealm/db.cpp
    cpprealm/client_reset.cpp
    cpprealm/managed_binary.cpp
    cpprealm/managed_decimal.cpp
    cpprealm/managed_numeric.cpp
    cpprealm/managed_objectid.cpp
    cpprealm/managed_primary_key.cpp
    cpprealm/managed_string.cpp
    cpprealm/managed_timestamp.cpp
    cpprealm/managed_uuid.cpp
    cpprealm/types.cpp
    cpprealm/flex_sync.cpp
    cpprealm/internal/bridge/async_open_task.cpp
    cpprealm/internal/bridge/binary.cpp
    cpprealm/internal/bridge/col_key.cpp
    cpprealm/internal/bridge/decimal128.cpp
    cpprealm/internal/bridge/dictionary.cpp
    cpprealm/internal/bridge/list.cpp
    cpprealm/internal/bridge/lnklst.cpp
    cpprealm/internal/bridge/mixed.cpp
    cpprealm/internal/bridge/obj.cpp
    cpprealm/internal/bridge/obj_key.cpp
    cpprealm/internal/bridge/object.cpp
    cpprealm/internal/bridge/object_id.cpp
    cpprealm/internal/bridge/object_schema.cpp
    cpprealm/internal/bridge/property.cpp
    cpprealm/internal/bridge/query.cpp
    cpprealm/internal/bridge/realm.cpp
    cpprealm/internal/bridge/results.cpp
    cpprealm/internal/bridge/schema.cpp
    cpprealm/internal/bridge/set.cpp
    cpprealm/internal/bridge/status.cpp
    cpprealm/internal/bridge/sync_error.cpp
    cpprealm/internal/bridge/sync_manager.cpp
    cpprealm/internal/bridge/sync_session.cpp
    cpprealm/internal/bridge/table.cpp
    cpprealm/internal/bridge/thread_safe_reference.cpp
    cpprealm/internal/bridge/timestamp.cpp
    cpprealm/internal/bridge/uuid.cpp
    cpprealm/internal/networking/network_transport.cpp
    cpprealm/internal/networking/shims.cpp
    cpprealm/internal/networking/utils.cpp
    cpprealm/internal/scheduler/realm_core_scheduler.cpp
    cpprealm/networking/http.cpp
    cpprealm/networking/websocket.cpp
    cpprealm/schedulers/default_scheduler.cpp
    cpprealm/logger.cpp
    cpprealm/sdk.cpp) # REALM_SOURCES

set(HEADERS
    ../include/cpprealm/analytics.hpp
    ../include/cpprealm/app.hpp
    ../include/cpprealm/accessors.hpp
    ../include/cpprealm/bson.hpp
    ../include/cpprealm/db.hpp
    ../include/cpprealm/client_reset.hpp
    ../include/cpprealm/link.hpp
    ../include/cpprealm/macros.hpp
    ../include/cpprealm/managed_binary.hpp
    ../include/cpprealm/managed_decimal.hpp
    ../include/cpprealm/managed_dictionary.hpp
    ../include/cpprealm/managed_list.hpp
    ../include/cpprealm/managed_mixed.hpp
    ../include/cpprealm/managed_numeric.hpp
    ../include/cpprealm/managed_objectid.hpp
    ../include/cpprealm/managed_primary_key.hpp
    ../include/cpprealm/managed_set.hpp
    ../include/cpprealm/managed_string.hpp
    ../include/cpprealm/managed_timestamp.hpp
    ../include/cpprealm/managed_uuid.hpp
    ../include/cpprealm/observation.hpp
    ../include/cpprealm/results.hpp
    ../include/cpprealm/types.hpp
    ../include/cpprealm/flex_sync.hpp
    ../include/cpprealm/internal/bridge/async_open_task.hpp
    ../include/cpprealm/internal/bridge/binary.hpp
    ../include/cpprealm/internal/bridge/col_key.hpp
    ../include/cpprealm/internal/bridge/decimal128.hpp
    ../include/cpprealm/internal/bridge/dictionary.hpp
    ../include/cpprealm/internal/bridge/list.hpp
    ../include/cpprealm/internal/bridge/lnklst.hpp
    ../include/cpprealm/internal/bridge/mixed.hpp
    ../include/cpprealm/internal/bridge/obj.hpp
    ../include/cpprealm/internal/bridge/obj_key.hpp
    ../include/cpprealm/internal/bridge/object.hpp
    ../include/cpprealm/internal/bridge/object_id.hpp
    ../include/cpprealm/internal/bridge/object_schema.hpp
    ../include/cpprealm/internal/bridge/property.hpp
    ../include/cpprealm/internal/bridge/query.hpp
    ../include/cpprealm/internal/bridge/realm.hpp
    ../include/cpprealm/internal/bridge/results.hpp
    ../include/cpprealm/internal/bridge/schema.hpp
    ../include/cpprealm/internal/bridge/set.hpp
    ../include/cpprealm/internal/bridge/status.hpp
    ../include/cpprealm/internal/bridge/sync_error.hpp
    ../include/cpprealm/internal/bridge/sync_manager.hpp
    ../include/cpprealm/internal/bridge/sync_session.hpp
    ../include/cpprealm/internal/bridge/table.hpp
    ../include/cpprealm/internal/bridge/thread_safe_reference.hpp
    ../include/cpprealm/internal/bridge/timestamp.hpp
    ../include/cpprealm/internal/bridge/utils.hpp
    ../include/cpprealm/internal/bridge/uuid.hpp
    ../include/cpprealm/internal/type_info.hpp
    ../include/cpprealm/internal/networking/shims.hpp
    ../include/cpprealm/internal/networking/utils.hpp
    ../include/cpprealm/internal/scheduler/realm_core_scheduler.hpp
    ../include/cpprealm/schedulers/default_scheduler.hpp
    ../include/cpprealm/networking/http.hpp
    ../include/cpprealm/networking/websocket.hpp
    ../include/cpprealm/logger.hpp
    ../include/cpprealm/notifications.hpp
    ../include/cpprealm/rbool.hpp
    ../include/cpprealm/scheduler.hpp
    ../include/cpprealm/schema.hpp
    ../include/cpprealm/thread_safe_reference.hpp
    ../include/cpprealm/sdk.hpp) # REALM_INSTALL_HEADERS

if (NOT REALM_DISABLE_ALIGNED_STORAGE)
    add_custom_command(
            COMMAND ${CMAKE_COMMAND} -D SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/cpprealm/internal/bridge
            -D BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}/cpprealm/internal/bridge
            -D BRIDGE_TYPE_INFO_BIN=$<TARGET_FILE:BridgeTypeInfoGenerator>
            -P ${CMAKE_CURRENT_SOURCE_DIR}/cpprealm/internal/bridge/generator/bridge_type_info_parser.cmake
            DEPENDS BridgeTypeInfoGenerator
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cpprealm/internal/bridge/generator/bridge_type_info_parser.cmake
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cpprealm/internal/bridge/bridge_types.hpp.in
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cpprealm/internal/bridge/bridge_types.hpp
    )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cpprealm/internal/bridge/bridge_types.hpp
            DESTINATION include/cpprealm/internal/bridge
            COMPONENT devel)

    set(BRIDGING_HEADER ${CMAKE_CURRENT_BINARY_DIR}/cpprealm/internal/bridge/bridge_types.hpp)
endif()

include(GNUInstallDirs)

add_library(cpprealm ${SOURCES} ${HEADERS} ${BRIDGING_HEADER})

if (NOT VCPKG_TOOLCHAIN)
    set(INSTALL_TARGETS ObjectStore Storage QueryParser)
    if (REALM_ENABLE_SYNC)
        list(APPEND INSTALL_TARGETS Sync)
    endif()
endif()

install(TARGETS ${PROJECT_NAME} ${INSTALL_TARGETS}
        EXPORT "${PROJECT_NAME}Targets"
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # include
        )

foreach(header ${HEADERS})
    get_filename_component(dir ${header} DIRECTORY)
    install(FILES ${header}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${dir}
            COMPONENT devel)
endforeach()

if (NOT REALM_DISABLE_ALIGNED_STORAGE)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cpprealm/internal/bridge/bridge_types.hpp
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cpprealm/internal/bridge
            COMPONENT devel)
endif()

install()
