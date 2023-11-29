cmake_minimum_required(VERSION 3.16)
project(coffee LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)
add_subdirectory(../../../../realm-cpp realm-cpp)

qt_add_executable(coffee
    main.cpp
)

set_target_properties(coffee PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

qt_add_qml_module(Barista
    URI Barista
    VERSION 1.0
    SOURCES coffee_manager.hpp realm_models.hpp coffee_manager.cpp imports/Coffee/Constants.qml
    NO_PLUGIN
    OUTPUT_TARGETS BARISTA_DEPS
)

target_link_libraries(Barista PUBLIC cpprealm)

target_link_libraries(coffee PUBLIC
    Barista
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)

# Resources:
set(qml_resource_files
    "ApplicationFlow.qml"
    "ApplicationFlowForm.ui.qml"
    "Brewing.qml"
    "BrewingForm.ui.qml"
    "ChoosingCoffee.ui.qml"
    "CoffeeButton.qml"
    "CupForm.ui.qml"
    "EmptyCupForm.ui.qml"
    "NavigationButton.ui.qml"
    "SideBar.qml"
    "images/cup structure/coffee_cup_large.png"
    "images/cup structure/coffee_cup_outline.png"
    "images/cup structure/cup elements/coffee_cup_back.png"
    "images/cup structure/cup elements/coffee_cup_coverplate.png"
    "images/cup structure/cup elements/coffee_cup_front.png"
    "images/cup structure/liquids/liquid_coffee.png"
    "images/cup structure/liquids/liquid_foam.png"
    "images/cup structure/liquids/liquid_milk.png"
    "images/icons/coffees/Americano.png"
    "images/icons/coffees/Espresso.png"
    "images/icons/coffees/Latte.png"
    "images/icons/coffees/Macchiato.png"
    "images/icons/coffees/cappucino.png"
    "images/icons/contents/coffee.png"
    "images/icons/contents/milk.png"
    "images/icons/contents/sugar.png"
    "images/ui controls/buttons/back/white.png"
    "images/ui controls/buttons/go/white.png"
    "images/ui controls/line.png"
    "imports/Coffee/Constants.qml"
    "imports/Coffee/TitilliumWeb-Regular.ttf"
    "imports/Coffee/qmldir"
    "main.qml"
    "qtquickcontrols2.conf"
)

qt6_add_resources(coffee "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)
