cmake_minimum_required(VERSION 3.31)

project (
	VendorDeps
	DESCRIPTION "vendor dependencies"
	LANGUAGES CXX)

add_library(VendorDeps)

## Find dependencies
find_package(glfw3 REQUIRED)

# Use  cmake -S . -B build/ -DOpenGL_GL_PREFERENCE=GLVND
# to skip legacy OpenGL library
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

IF (WIN32)
    target_link_libraries(VendorDeps PRIVATE glfw opengl32.lib)
ELSE()
	target_link_libraries(VendorDeps PRIVATE glfw OpenGL)
ENDIF()



target_include_directories(VendorDeps PUBLIC imgui/)
target_sources(VendorDeps PRIVATE imgui/imgui.cpp)