file(STRINGS ${BRIDGE_TYPE_INFO_BIN} compiled_string_literals REGEX "^(REALM_TYPE_INFO|arch_key_)")

set(regex_realm "REALM_TYPE_INFO:(.+)\\[0*(.*),0*(.*)\\]")
set(regex_arch "^arch_key_(.+)")

set(BRIDGE_TYPE_DECLS "")
set(BRIDGE_TYPE_DECLS_TYPES "")
set(BRIDGE_TYPE_DECLS_REQUIRES_ELSE_IF OFF)

foreach(i ${compiled_string_literals})
    message(STATUS "${i}")
    if("${i}" MATCHES "${regex_realm}")
        set(BRIDGE_TYPE_DECLS_TYPES "${BRIDGE_TYPE_DECLS_TYPES}    using ${CMAKE_MATCH_1} = std::aligned_storage<${CMAKE_MATCH_2}, ${CMAKE_MATCH_3}>::type;\n")
    elseif("${i}" MATCHES "${regex_arch}")
        if(BRIDGE_TYPE_DECLS_REQUIRES_ELSE_IF)
        set(BRIDGE_TYPE_DECLS "${BRIDGE_TYPE_DECLS}#elif defined(${CMAKE_MATCH_1})\n${BRIDGE_TYPE_DECLS_TYPES}")
        else()
        set(BRIDGE_TYPE_DECLS "#if defined(${CMAKE_MATCH_1})\n${BRIDGE_TYPE_DECLS_TYPES}")
        endif()
        set(BRIDGE_TYPE_DECLS_REQUIRES_ELSE_IF ON)
        set(BRIDGE_TYPE_DECLS_TYPES "")
    else()
        message(FATAL_ERROR "Unrecognized type info string: ${i}")
    endif()
endforeach()

set(BRIDGE_TYPE_DECLS "${BRIDGE_TYPE_DECLS}#endif")

configure_file(${SOURCE_DIR}/bridge_types.hpp.in ${BINARY_DIR}/bridge_types.hpp)
