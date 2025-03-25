cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(vendor)
add_subdirectory(src/widgets)

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

target_include_directories(DebugBoardUI PUBLIC 
	vendor/imgui/ 
	vendor/imgui/backends/ 
	vendor/ 
	vendor/json/include/
	vendor/glfw/include/)


target_link_libraries(DebugBoardUI PRIVATE VendorDeps Widgets)

target_sources(
	DebugBoardUI
	PRIVATE
        src/main.cpp
		src/platform/glfw_opengl_glue.cpp
		src/Workspace.cpp

		src/datasources/ProgrammingDataSource.cpp
		# src/datasources/DebugBoard.cpp


)
