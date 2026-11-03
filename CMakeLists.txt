cmake_minimum_required(VERSION 4.4.3)

project(pg4g)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external/glfw)
add_subdirectory(external/glm)

add_library(glad STATIC
	external/glad/src/glad.c
)

target_include_directories(glad 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(pg4g
	${SOURCES}
)

target_include_directories(pg4g 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(pg4g
	PRIVATE glfw glad glm::glm 
)


