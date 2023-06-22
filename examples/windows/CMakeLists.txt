cmake_minimum_required(VERSION 3.15)

project(MyRealmCppProject)

# Minimum C++ standard
set(CMAKE_CXX_STANDARD 17)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:preprocessor /bigobj")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

# Include the FetchContent module so you can download the Realm C++ SDK
Include(FetchContent)

FetchContent_Declare(
  cpprealm
  GIT_REPOSITORY https://github.com/realm/realm-cpp.git
  GIT_TAG        8969e8c947b8fdc8ede4660b1d22ee676a45fbf4

)

# The MakeAvailable command ensures the named dependencies have been populated
FetchContent_MakeAvailable(cpprealm)

add_executable(myApp helloworld.cpp)

if(MSVC)
    set_property(TARGET myApp PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

target_link_libraries(myApp PUBLIC cpprealm)