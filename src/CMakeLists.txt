add_subdirectory(cpprealm/internal/bridge/generator)

include(GetVersion)
# Configure source code to use right version number
configure_file(cpprealm/internal/version_numbers.hpp.in cpprealm/version_numbers.hpp)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cpprealm/version_numbers.hpp
        DESTINATION include/cpprealm
        COMPONENT devel)

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

set(SOURCES
    cpprealm/analytics.cpp
    cpprealm/app.cpp
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
    cpprealm/logger.cpp
    cpprealm/scheduler.cpp
    cpprealm/sdk.cpp) # REALM_SOURCES

set(HEADERS
    cpprealm/analytics.hpp
    cpprealm/app.hpp
    cpprealm/accessors.hpp
    cpprealm/db.hpp
    cpprealm/client_reset.hpp
    cpprealm/link.hpp
    cpprealm/macros.hpp
    cpprealm/managed_binary.hpp
    cpprealm/managed_decimal.hpp
    cpprealm/managed_dictionary.hpp
    cpprealm/managed_list.hpp
    cpprealm/managed_mixed.hpp
    cpprealm/managed_numeric.hpp
    cpprealm/managed_objectid.hpp
    cpprealm/managed_primary_key.hpp
    cpprealm/managed_set.hpp
    cpprealm/managed_string.hpp
    cpprealm/managed_timestamp.hpp
    cpprealm/managed_uuid.hpp
    cpprealm/observation.hpp
    cpprealm/results.hpp
    cpprealm/types.hpp
    cpprealm/flex_sync.hpp
    cpprealm/internal/bridge/async_open_task.hpp
    cpprealm/internal/bridge/binary.hpp
    cpprealm/internal/bridge/col_key.hpp
    cpprealm/internal/bridge/decimal128.hpp
    cpprealm/internal/bridge/dictionary.hpp
    cpprealm/internal/bridge/list.hpp
    cpprealm/internal/bridge/lnklst.hpp
    cpprealm/internal/bridge/mixed.hpp
    cpprealm/internal/bridge/obj.hpp
    cpprealm/internal/bridge/obj_key.hpp
    cpprealm/internal/bridge/object.hpp
    cpprealm/internal/bridge/object_id.hpp
    cpprealm/internal/bridge/object_schema.hpp
    cpprealm/internal/bridge/property.hpp
    cpprealm/internal/bridge/query.hpp
    cpprealm/internal/bridge/realm.hpp
    cpprealm/internal/bridge/results.hpp
    cpprealm/internal/bridge/schema.hpp
    cpprealm/internal/bridge/set.hpp
    cpprealm/internal/bridge/status.hpp
    cpprealm/internal/bridge/sync_error.hpp
    cpprealm/internal/bridge/sync_manager.hpp
    cpprealm/internal/bridge/sync_session.hpp
    cpprealm/internal/bridge/table.hpp
    cpprealm/internal/bridge/thread_safe_reference.hpp
    cpprealm/internal/bridge/timestamp.hpp
    cpprealm/internal/bridge/utils.hpp
    cpprealm/internal/bridge/uuid.hpp
    cpprealm/internal/generic_network_transport.hpp
    cpprealm/internal/type_info.hpp
    cpprealm/logger.hpp
    cpprealm/notifications.hpp
    cpprealm/rbool.hpp
    cpprealm/scheduler.hpp
    cpprealm/schema.hpp
    cpprealm/thread_safe_reference.hpp
    cpprealm/sdk.hpp) # REALM_INSTALL_HEADERS

include(GNUInstallDirs)

add_library(cpprealm ${SOURCES} ${HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/cpprealm/internal/bridge/bridge_types.hpp)

install(TARGETS ${PROJECT_NAME} ObjectStore Storage QueryParser Sync
        EXPORT "${PROJECT_NAME}Targets"
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # include
        )

foreach(header ${HEADERS})
    get_filename_component(dir ${header} DIRECTORY)
    install(FILES ${header}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${dir}
            COMPONENT devel)
endforeach()
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cpprealm/internal/bridge/bridge_types.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cpprealm/internal/bridge
        COMPONENT devel)
install()
