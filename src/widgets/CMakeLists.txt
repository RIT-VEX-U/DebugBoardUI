cmake_minimum_required(VERSION 3.20)

project (
	Widgets
	DESCRIPTION "Different types of Widgets"
	LANGUAGES CXX)

add_library(Widgets)

target_include_directories(Widgets PRIVATE ${CMAKE_SOURCE_DIR}/vendor/imgui/ ${CMAKE_SOURCE_DIR}/vendor)
target_include_directories(Widgets PUBLIC ./)

target_sources(Widgets PRIVATE 
    ControlSystems/PidVizWidget.cpp
    Visualization/FieldMapWidget.cpp
    Visualization/GenericGrapher.cpp
    Widget.cpp
    WidgetRegistry.cpp
    RegisteredWidgets.cpp
)
