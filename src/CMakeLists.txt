add_subdirectory(cpprealm/internal/bridge/generator)

include(GetVersion)
# Configure source code to use right version number
configure_file(${RealmCxx_SOURCE_DIR}/src/cpprealm/internal/version_numbers.hpp.in ${RealmCxx_SOURCE_DIR}/src/cpprealm/internal/version_numbers.hpp)
install(FILES ${RealmCxx_SOURCE_DIR}/src/cpprealm/internal/version_numbers.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cpprealm/internal
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

set(SOURCES
    cpprealm/analytics.cpp
    cpprealm/app.cpp
    cpprealm/experimental/db.cpp
    cpprealm/experimental/managed_binary.cpp
    cpprealm/experimental/managed_decimal.cpp
    cpprealm/experimental/managed_numeric.cpp
    cpprealm/experimental/managed_objectid.cpp
    cpprealm/experimental/managed_primary_key.cpp
    cpprealm/experimental/managed_string.cpp
    cpprealm/experimental/managed_timestamp.cpp
    cpprealm/experimental/managed_uuid.cpp
    cpprealm/experimental/observation.cpp
    cpprealm/experimental/types.cpp
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
    cpprealm/asymmetric_object.hpp
    cpprealm/experimental/accessors.hpp
    cpprealm/experimental/db.hpp
    cpprealm/experimental/link.hpp
    cpprealm/experimental/macros.hpp
    cpprealm/experimental/managed_binary.hpp
    cpprealm/experimental/managed_decimal.hpp
    cpprealm/experimental/managed_dictionary.hpp
    cpprealm/experimental/managed_list.hpp
    cpprealm/experimental/managed_mixed.hpp
    cpprealm/experimental/managed_numeric.hpp
    cpprealm/experimental/managed_objectid.hpp
    cpprealm/experimental/managed_primary_key.hpp
    cpprealm/experimental/managed_set.hpp
    cpprealm/experimental/managed_string.hpp
    cpprealm/experimental/managed_timestamp.hpp
    cpprealm/experimental/managed_uuid.hpp
    cpprealm/experimental/observation.hpp
    cpprealm/experimental/results.hpp
    cpprealm/experimental/sdk.hpp
    cpprealm/experimental/types.hpp
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
    cpprealm/object.hpp
    cpprealm/persisted.hpp
    cpprealm/rbool.hpp
    cpprealm/scheduler.hpp
    cpprealm/schema.hpp
    cpprealm/thread_safe_reference.hpp
    cpprealm/sdk.hpp
    cpprealm/alpha_support.hpp) # REALM_INSTALL_HEADERS

if(ENABLE_ALPHA_SDK)
    list(APPEND HEADERS
            cpprealm/db.hpp
            cpprealm/persisted_binary.hpp
            cpprealm/persisted_bool.hpp
            cpprealm/persisted_decimal128.hpp
            cpprealm/persisted_double.hpp
            cpprealm/persisted_embedded.hpp
            cpprealm/persisted_enum.hpp
            cpprealm/persisted_int.hpp
            cpprealm/persisted_list.hpp
            cpprealm/persisted_map.hpp
            cpprealm/persisted_mixed.hpp
            cpprealm/persisted_object_id.hpp
            cpprealm/persisted_optional.hpp
            cpprealm/persisted_string.hpp
            cpprealm/persisted_timestamp.hpp
            cpprealm/persisted_uuid.hpp
            cpprealm/results.hpp)

    list(APPEND SOURCES
            cpprealm/persisted_binary.cpp
            cpprealm/persisted_bool.cpp
            cpprealm/persisted_decimal128.cpp
            cpprealm/persisted_double.cpp
            cpprealm/persisted_int.cpp
            cpprealm/persisted_object_id.cpp
            cpprealm/persisted_string.cpp
            cpprealm/persisted_uuid.cpp)
endif()

include(GNUInstallDirs)

add_library(cpprealm ${SOURCES} ${HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/cpprealm/internal/bridge/bridge_types.hpp)
target_include_directories(cpprealm PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS cpprealm RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime)
install(TARGETS cpprealm LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime)
install(TARGETS cpprealm ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel)

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
