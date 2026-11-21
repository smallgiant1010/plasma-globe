cmake_minimum_required(VERSION 4.4.3)

project(pg4g)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)

add_subdirectory(external/glfw)
add_subdirectory(external/glm SYSTEM)

add_library(glad STATIC
	external/glad/src/glad.c
)
add_library(imgui STATIC
	${IMGUI_DIR}/imgui.cpp
	${IMGUI_DIR}/imgui_demo.cpp
	${IMGUI_DIR}/imgui_draw.cpp
	${IMGUI_DIR}/imgui_tables.cpp
	${IMGUI_DIR}/imgui_widgets.cpp
	${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
	${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_custom_target(copy_assets ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)

target_include_directories(glad 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
)
target_include_directories(imgui PUBLIC
	${IMGUI_DIR}
	${IMGUI_DIR}/backends
)

target_link_libraries(imgui PRIVATE glfw)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(pg4g
	${SOURCES}
)

add_dependencies(pg4g copy_assets)

target_compile_options(pg4g PRIVATE
	-Wall
	-Wextra
)

target_include_directories(pg4g 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(pg4g
	PRIVATE glfw glad glm::glm imgui
)


