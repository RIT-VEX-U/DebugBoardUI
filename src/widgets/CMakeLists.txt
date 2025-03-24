cmake_minimum_required(VERSION 3.31)

project (
	Widgets
	DESCRIPTION "Different types of Widgets"
	LANGUAGES CXX)

add_library(Widgets)

target_include_directories(Widgets PRIVATE ${CMAKE_SOURCE_DIR}/vendor/imgui/)
target_include_directories(Widgets PUBLIC ./)

target_sources(Widgets PRIVATE 
    ControlSystems/PidVizWidget.cpp
    Visualization/FieldMapWidget.cpp
    Widget.cpp
    WidgetRegistry.cpp
    RegisteredWidgets.cpp
)