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
target_compile_definitions(DebugBoardUI PRIVATE IMGUI_USER_CONFIG="imgui_config.h")



if (UNIX)
	target_compile_options(DebugBoardUI PUBLIC -fsanitize=address)
	target_link_options(DebugBoardUI PUBLIC -fsanitize=address)
	target_compile_options(DebugBoardUI PRIVATE -Wnarrowing)
endif (UNIX)


target_include_directories(
	DebugBoardUI
  	PRIVATE 
	src/
)

target_link_libraries(DebugBoardUI PRIVATE VendorDeps Widgets)

target_sources(
	DebugBoardUI
	PRIVATE
        src/main.cpp
		src/platform/glfw_opengl_glue.cpp
		src/Workspace.cpp
		src/Types.cpp
		src/datasources/DebugBoard.cpp


)
