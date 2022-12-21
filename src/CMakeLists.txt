set(SOURCES
    cpprealm/app.cpp
    cpprealm/sdk.cpp
    cpprealm/analytics.cpp
    cpprealm/type_info.cpp
    cpprealm/thread_safe_reference.cpp
    cpprealm/internal/bridge/obj.hpp
    cpprealm/internal/bridge/obj.cpp
    cpprealm/persisted_uuid.hpp
    cpprealm/persisted_string.hpp
    cpprealm/persisted_string.cpp
    cpprealm/persisted_list.hpp
    cpprealm/persisted_custom.hpp
    cpprealm/internal/type_info.hpp
    cpprealm/internal/bridge/utils.hpp
    cpprealm/internal/bridge/col_key.hpp
    cpprealm/internal/bridge/col_key.cpp
    cpprealm/internal/bridge/object.cpp
    cpprealm/internal/bridge/object.hpp
    cpprealm/internal/bridge/property.cpp
    cpprealm/internal/bridge/property.hpp
    cpprealm/internal/bridge/object_schema.cpp
    cpprealm/internal/bridge/object_schema.hpp
    cpprealm/internal/bridge/mixed.cpp
    cpprealm/internal/bridge/mixed.hpp
    cpprealm/internal/bridge/uuid.cpp
    cpprealm/internal/bridge/uuid.hpp
    cpprealm/internal/bridge/binary.cpp
    cpprealm/internal/bridge/binary.hpp
    cpprealm/internal/bridge/timestamp.cpp
    cpprealm/internal/bridge/timestamp.hpp
    cpprealm/internal/bridge/obj_key.cpp
    cpprealm/internal/bridge/obj_key.hpp
    cpprealm/internal/bridge/table.cpp
    cpprealm/internal/bridge/table.hpp
    cpprealm/internal/bridge/query.cpp
    cpprealm/internal/bridge/query.hpp
    cpprealm/persisted_uuid.cpp
    cpprealm/internal/bridge/scheduler.cpp
    cpprealm/internal/bridge/scheduler.hpp
    cpprealm/scheduler.hpp
    cpprealm/internal/bridge/realm.cpp
    cpprealm/internal/bridge/realm.hpp
    cpprealm/internal/bridge/dictionary.cpp
    cpprealm/internal/bridge/dictionary.hpp
    cpprealm/internal/bridge/results.cpp
    cpprealm/internal/bridge/results.hpp
    cpprealm/internal/bridge/schema.cpp
    cpprealm/internal/bridge/schema.hpp
    cpprealm/scheduler.cpp
    cpprealm/internal/bridge/thread_safe_reference.cpp
    cpprealm/internal/bridge/thread_safe_reference.hpp
    cpprealm/internal/bridge/lnklst.cpp
    cpprealm/internal/bridge/lnklst.hpp
    cpprealm/internal/bridge/list.cpp
    cpprealm/internal/bridge/list.hpp
    cpprealm/persisted_binary.cpp
    cpprealm/internal/type_info.cpp
    cpprealm/persisted_int.cpp
    cpprealm/flex_sync.cpp
    cpprealm/persisted_bool.cpp
    cpprealm/internal/bridge/sync_session.cpp
    cpprealm/internal/bridge/sync_session.hpp
    cpprealm/internal/bridge/sync_error.cpp
    cpprealm/internal/bridge/sync_error.hpp
    cpprealm/internal/bridge/sync_manager.cpp
    cpprealm/internal/bridge/sync_manager.hpp
    cpprealm/internal/bridge/async_open_task.cpp
    cpprealm/internal/bridge/async_open_task.hpp) # REALM_SOURCES

set(HEADERS
    cpprealm/sdk.hpp
    cpprealm/app.hpp
    cpprealm/db.hpp
    cpprealm/notifications.hpp
    cpprealm/object.hpp
    cpprealm/persisted.hpp
    cpprealm/results.hpp
    cpprealm/schema.hpp
    cpprealm/task.hpp
    cpprealm/thread_safe_reference.hpp
    cpprealm/type_info.hpp
    cpprealm/flex_sync.hpp
    cpprealm/persisted_string.hpp
    cpprealm/persisted_bool.hpp
    cpprealm/persisted_int.hpp
    cpprealm/persisted_double.hpp
    cpprealm/persisted_optional.hpp
    cpprealm/persisted_enum.hpp
    cpprealm/persisted_timestamp.hpp
    cpprealm/persisted_uuid.hpp
    cpprealm/persisted_list.hpp
    cpprealm/persisted_embedded.hpp
    cpprealm/persisted_binary.hpp
    cpprealm/persisted_mixed.hpp
    cpprealm/persisted_map.hpp
    cpprealm/analytics.hpp
    cpprealm/internal/generic_network_transport.hpp) # REALM_INSTALL_HEADERS

add_library(cpprealm SHARED ${SOURCES} ${HEADERS})

set_target_properties(cpprealm PROPERTIES PUBLIC_HEADER "${HEADERS}")

foreach(FILE ${HEADERS})
    get_filename_component(DIR ${FILE} DIRECTORY)
    INSTALL(FILES ${FILE}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${DIR}
            COMPONENT devel)
endforeach()

install(TARGETS cpprealm
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cpprealm)
