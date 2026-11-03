cmake_minimum_required(VERSION 4.4.3)

project(bobmath)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} INTERFACE)

# set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER adder.hpp)

# install( TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib
#		PUBLIC_HEADER DESTINATION include 	
#	)


target_sources(${PROJECT_NAME} INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/adder.hpp
)

target_include_directories(${PROJECT_NAME} INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}
)
