set(SOURCES
    cpprealm/sdk.cpp
    cpprealm/analytics.cpp) # REALM_SOURCES

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
