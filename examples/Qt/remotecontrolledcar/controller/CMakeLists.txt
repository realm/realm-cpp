cmake_minimum_required(VERSION 3.16)
project(controller LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
  set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/realm/remotecontrolledcar/controller")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_add_executable(controller
    ../car/car.h ../car/car.cpp
    controller.cpp controller.h controller.ui
    main.cpp
)
set_target_properties(controller PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(controller PRIVATE
    cpprealm
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

install(TARGETS controller
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
