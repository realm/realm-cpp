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
  GIT_TAG        b6c2b8be5b620c92eaa8ba76d96d2d16ea73501a

)

# The MakeAvailable command ensures the named dependencies have been populated
FetchContent_MakeAvailable(cpprealm)

add_executable(myApp helloworld.cpp)

if(MSVC)
    set_property(TARGET myApp PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

target_link_libraries(myApp PUBLIC cpprealm)