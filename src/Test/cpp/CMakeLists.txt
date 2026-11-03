cmake_minimum_required(VERSION 4.4.3)

project(test)

add_executable(${PROJECT_NAME} main.cpp)

target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../../SomeLibExample/out/build/)

target_link_libraries(${PROJECT_NAME} PRIVATE bobmath)
