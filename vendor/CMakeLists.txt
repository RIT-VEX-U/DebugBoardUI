cmake_minimum_required(VERSION 3.20)

project (
	VendorDeps
	DESCRIPTION "vendor dependencies"
	LANGUAGES CXX)
add_library(VendorDeps)


# Typically you don't care so much for a third party library's tests to be
# run from your own project's code.
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)
target_link_libraries(VendorDeps PUBLIC nlohmann_json::nlohmann_json)


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



target_include_directories(VendorDeps PUBLIC imgui/ imgui/backends)
target_sources(VendorDeps PRIVATE 
    imgui/imgui.cpp 
    imgui/imgui_draw.cpp 
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_demo.cpp
    imgui/backends/imgui_impl_glfw.cpp 
    imgui/backends/imgui_impl_opengl3.cpp
    imgui-knobs/imgui-knobs.cpp
    implot/implot.cpp
    implot/implot_demo.cpp
    implot/implot_items.cpp

    easywsclient/easywsclient.cpp
)
