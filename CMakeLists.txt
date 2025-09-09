cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

## project
project (
	DebugBoardUI
	DESCRIPTION "DebugBoardUI using ImGui with GLFW, OpenGL3"
	LANGUAGES C CXX)

# Try to do IPO (LTO)
include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported OUTPUT error)

if( lto_supported AND (NOT CMAKE_BUILD_TYPE MATCHES Debug))
    message(STATUS "IPO / LTO enabled")
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
elseif(lto_supported)
	message(STATUS "LTO Supported but not enabled for debug build")
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()

set(GLFW_BUILD_DOCS OFF)
# ImGui, ImPlot, GLFW, JSON, CmakeRC
add_subdirectory(vendor)
add_subdirectory(src/widgets)
add_subdirectory(src/datasources)
add_subdirectory(src/platform)


## Create main executable
add_executable(DebugBoardUI)
target_compile_definitions(DebugBoardUI PRIVATE IMGUI_USER_CONFIG="imgui_config.h")


cmrc_add_resource_library(assets
    assets/icon.png assets/RealLogo.jpg.png.png
    ALIAS assets::rc
    NAMESPACE assets)
target_link_libraries(DebugBoardUI PRIVATE assets::rc)



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

target_link_libraries(DebugBoardUI PRIVATE VendorDeps Widgets Datasources Platform)

target_sources(
	DebugBoardUI
	PRIVATE
        src/main.cpp
		src/platform/glfw_opengl_glue.cpp
		src/Workspace.cpp
		src/Types.cpp
		src/datasources/DebugBoard.cpp

)
