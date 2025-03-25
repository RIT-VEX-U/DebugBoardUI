cmake_minimum_required(VERSION 3.31)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g") # For GCC/Clang

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

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
	vendor/json/include/)


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