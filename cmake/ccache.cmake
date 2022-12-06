find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Found Ccache: ${CCACHE_PROGRAM}")
    if(CMAKE_GENERATOR STREQUAL "Xcode")
        # Set up wrapper scripts
        configure_file("${CMAKE_CURRENT_LIST_DIR}/launch-c.in" "${CMAKE_BINARY_DIR}/launch-c" @ONLY)
        configure_file("${CMAKE_CURRENT_LIST_DIR}/launch-cxx.in" "${CMAKE_BINARY_DIR}/launch-cxx" @ONLY)
        execute_process(COMMAND chmod a+rx
                        "${CMAKE_BINARY_DIR}/launch-c"
                        "${CMAKE_BINARY_DIR}/launch-cxx"
        )

        # Set Xcode project attributes to route compilation and linking
        # through our scripts
        set(CMAKE_XCODE_ATTRIBUTE_CC         "${CMAKE_BINARY_DIR}/launch-c")
        set(CMAKE_XCODE_ATTRIBUTE_CXX        "${CMAKE_BINARY_DIR}/launch-cxx")
        set(CMAKE_XCODE_ATTRIBUTE_LD         "${CMAKE_BINARY_DIR}/launch-c")
        set(CMAKE_XCODE_ATTRIBUTE_LDPLUSPLUS "${CMAKE_BINARY_DIR}/launch-cxx")
    else()
        # Support Unix Makefiles and Ninja
        set(CMAKE_C_COMPILER_LAUNCHER   "${CCACHE_PROGRAM}")
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    endif()
else()
    message(WARNING "ccache was not found")
endif()
