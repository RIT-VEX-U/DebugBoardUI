cmake_minimum_required(VERSION 3.20)

project (
	Platform
	DESCRIPTION "Platform for the Imgui Windows"
	LANGUAGES CXX)

add_library(Platform)

# theres gotta be a better way to do this
target_include_directories(Platform PRIVATE
    ${CMAKE_SOURCE_DIR}/vendor/glfw/include
    ${CMAKE_SOURCE_DIR}/vendor/imgui/
    ${CMAKE_SOURCE_DIR}/vendor/
    ${CMAKE_SOURCE_DIR}/vendor/imgui/backends/
    ${CMAKE_SOURCE_DIR}/build/_cmrc/include/
    ${CMAKE_SOURCE_DIR}/src # for type defs
)
target_include_directories(Platform PUBLIC ./)

target_sources(Platform PRIVATE
    glfw_opengl_glue.cpp
    glue.hpp
)