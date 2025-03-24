cmake_minimum_required(VERSION 3.31)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_subdirectory(vendor)
## project
project (
	DebugBoardUI
	DESCRIPTION "DebugBoardUI using ImGui with GLFW, OpenGL3"
	LANGUAGES CXX)


## Create main executable
add_executable(DebugBoardUI)

target_include_directories(
	DebugBoardUI
  	PRIVATE 
	src/
)

target_include_directories(DebugBoardUI PUBLIC vendor/imgui/ vendor/imgui/backends vendor/)


target_link_libraries(DebugBoardUI PRIVATE VendorDeps)
target_sources(
	DebugBoardUI
	PRIVATE
        src/main.cpp
		src/platform/glfw_opengl_glue.cpp

        # ${CMAKE_SOURCE_DIR}/implot/implot.cpp
        # ${CMAKE_SOURCE_DIR}/implot/implot_items.cpp
        # ${CMAKE_SOURCE_DIR}/implot/implot_demo.cpp

        # ${CMAKE_SOURCE_DIR}/imgui-knobs/imgui-knobs.cpp
)