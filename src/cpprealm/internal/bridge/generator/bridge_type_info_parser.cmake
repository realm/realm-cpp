file(STRINGS ${BRIDGE_TYPE_INFO_BIN} compiled_string_literals REGEX "^REALM_TYPE_INFO")

set(regex "REALM_TYPE_INFO:(.+)\\[0*(.*),0*(.*)\\]")

foreach(i ${compiled_string_literals})
    if("${i}" MATCHES "${regex}")
        list(APPEND BRIDGE_TYPE_DECLS "using ${CMAKE_MATCH_1} = std::aligned_storage<${CMAKE_MATCH_2}, ${CMAKE_MATCH_3}>::type;\n")
    else()
        message(FATAL_ERROR "Unrecognized type info string: " ${h})
    endif()
endforeach()

configure_file(${SOURCE_DIR}/bridge_types.hpp.in ${BINARY_DIR}/bridge_types.hpp)
