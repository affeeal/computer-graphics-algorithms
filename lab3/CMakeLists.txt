cmake_minimum_required(VERSION 3.20)

project("lab3")

add_executable(lab3 main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL glfw)
