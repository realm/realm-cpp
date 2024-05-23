cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH SOURCE_PATH)

find_program(GIT git)

vcpkg_execute_required_process(
  COMMAND ${GIT} submodule update --init --recursive
  WORKING_DIRECTORY ${SOURCE_PATH}
  LOGNAME submodules
)

set(CPPREALM_CMAKE_OPTIONS -DREALM_CPP_NO_TESTS=ON -DREALM_CORE_SUBMODULE_BUILD=OFF)

if (ANDROID OR WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND CPPREALM_CMAKE_OPTIONS -DREALM_USE_SYSTEM_OPENSSL=ON)
endif()

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    DISABLE_PARALLEL_CONFIGURE
    OPTIONS
        ${CPPREALM_CMAKE_OPTIONS}
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(PACKAGE_NAME "cpprealm" CONFIG_PATH "cmake")
vcpkg_cmake_config_fixup(PACKAGE_NAME "Realm" CONFIG_PATH "share/cmake/Realm")

file(READ ${CURRENT_PACKAGES_DIR}/debug/include/cpprealm/internal/bridge/bridge_types.hpp DEBUG_TYPE_HEADER_CONTENTS)
set(REGEX_PATTERN "\\{([^()]*)\\}")
string(REGEX MATCHALL "${REGEX_PATTERN}" MATCHED_CONTENT "${DEBUG_TYPE_HEADER_CONTENTS}")
set(MATCHED_DEBUG_TYPE_HEADER_CONTENTS "${CMAKE_MATCH_1}")

file(READ ${CURRENT_PACKAGES_DIR}/include/cpprealm/internal/bridge/bridge_types.hpp RELEASE_TYPE_HEADER_CONTENTS)
set(REGEX_PATTERN "\\{([^()]*)\\}")
string(REGEX MATCHALL "${REGEX_PATTERN}" MATCHED_CONTENT "${RELEASE_TYPE_HEADER_CONTENTS}")
set(MATCHED_RELEASE_TYPE_HEADER_CONTENTS "${CMAKE_MATCH_1}")

string(REGEX REPLACE "\\{([^()]*)\\}" "
{
        #ifdef REALM_DEBUG
        ${MATCHED_DEBUG_TYPE_HEADER_CONTENTS}
        #else
        ${MATCHED_RELEASE_TYPE_HEADER_CONTENTS}
        #endif
}
    " MODIFIED_HEADER "${DEBUG_TYPE_HEADER_CONTENTS}")

file(WRITE ${CURRENT_PACKAGES_DIR}/include/cpprealm/internal/bridge/bridge_types.hpp "${MODIFIED_HEADER}")

file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug/include"
    "${CURRENT_PACKAGES_DIR}/debug/share"
)

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
