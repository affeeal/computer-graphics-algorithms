cmake_minimum_required(VERSION 3.15)

set(CMAKE_PROJECT_NAME graphics)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${CMAKE_PROJECT_NAME})

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_subdirectory(external)

add_subdirectory(lab8)

