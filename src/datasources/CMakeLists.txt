cmake_minimum_required(VERSION 3.20)

project (
	Datasources
	DESCRIPTION "Different types of Data Sources"
	LANGUAGES CXX)

add_library(Datasources)

# theres gotta be a better way to do this
target_include_directories(Datasources PRIVATE 
    ${CMAKE_SOURCE_DIR}/vendor/json/include
    ${CMAKE_SOURCE_DIR}/vendor/
    ${CMAKE_SOURCE_DIR}/vendor/eigen
    ${CMAKE_SOURCE_DIR}/vendor/imgui/
    ${CMAKE_SOURCE_DIR}/src # for type defs
)
target_include_directories(Datasources PUBLIC ./)

target_sources(
    Datasources
    PRIVATE
        DebugBoard.cpp
        DebugBoard.hpp
)