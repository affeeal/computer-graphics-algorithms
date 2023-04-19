add_executable(lab4 main.cpp)

target_link_libraries(lab4 PRIVATE 
                      OpenGL::GL
                      glfw
                      graphics_compiler_flags)
