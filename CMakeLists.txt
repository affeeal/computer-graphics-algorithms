cmake_minimum_required(VERSION 3.15)

project(graphics)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_library(graphics_compiler_flags INTERFACE)
target_compile_features(graphics_compiler_flags INTERFACE
                        cxx_std_17)

target_compile_options(graphics_compiler_flags INTERFACE
  "$<BUILD_INTERFACE:-g;-Wall;-Wextra;-Wpedantic;-Wformat=2;-Wunused>"
)

add_subdirectory(lab1)
add_subdirectory(lab2)
add_subdirectory(lab3)
add_subdirectory(lab4)
add_subdirectory(lab5)
add_subdirectory(lab6)
add_subdirectory(lab7)

