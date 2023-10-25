# FindCppRealm.cmake

# This module is responsible for finding and configuring the CppRealm library.

# Define a user-configurable option to specify the installation directory of CppRealm.
option(cpprealm_DIR "Path to CppRealm installation directory" "")

find_library(cpprealm_LIBRARY
  NAMES cpprealm
  HINTS ${cpprealm_DIR}
)

# Provide the results of the package search and configuration.
if (cpprealm_INCLUDE_DIR AND cpprealm_LIBRARY)
  set(CppRealm_FOUND TRUE)
  set(cpprealm_INCLUDE_DIRS ${cpprealm_INCLUDE_DIR})
  set(cpprealm_LIBRARIES ${cpprealm_LIBRARY})
else ()
  set(cpprealm_FOUND FALSE)
  set(cpprealm_INCLUDE_DIRS)
  set(cpprealm_LIBRARIES)
endif ()

# Inform CMake about the results of the search and configuration.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(cpprealm
  REQUIRED_VARS cpprealm_INCLUDE_DIR cpprealm_LIBRARY
)

# Optionally, you can provide additional package-specific logic here.
