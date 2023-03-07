cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

set(PROJECT_NAME "Computer graphics algorithms")

project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -pedantic -Wformat=2 -Wfloat-equal -Wconversion \
        -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align")

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_subdirectory(lab1)
add_subdirectory(lab2)
