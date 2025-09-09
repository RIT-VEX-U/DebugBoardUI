cmake_minimum_required(VERSION 3.20)

project (
	Widgets
	DESCRIPTION "Different types of Widgets"
	LANGUAGES CXX)

add_library(Widgets)

# theres gotta be a better way to do this
target_include_directories(Widgets PRIVATE
    ${CMAKE_SOURCE_DIR}/vendor/json/include
    ${CMAKE_SOURCE_DIR}/vendor/imgui/
    ${CMAKE_SOURCE_DIR}/vendor/imgui/backends
    ${CMAKE_SOURCE_DIR}/vendor/stb/
    ${CMAKE_SOURCE_DIR}/vendor/glfw/include/
    ${CMAKE_SOURCE_DIR}/vendor
    ${CMAKE_SOURCE_DIR}/vendor/eigen
    ${CMAKE_SOURCE_DIR}/src # for type defs
)
target_include_directories(Widgets PUBLIC ./)

target_sources(Widgets PRIVATE 
    ControlSystems/PidVizWidget.cpp
    Visualization/FieldMapWidget.cpp
    Visualization/GenericGrapher.cpp
    ControlSystems/PidVizWidget.hpp
    Visualization/FieldMapWidget.hpp
    Visualization/GenericGrapher.hpp
    Connection/ConnectWidget.cpp
    Connection/ConnectWidget.hpp
    Widget.cpp
    WidgetRegistry.cpp
    RegisteredWidgets.cpp
)
