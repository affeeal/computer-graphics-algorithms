add_executable(lab5 main.cpp)

target_link_libraries(lab5 PRIVATE 
                      OpenGL::GL
                      glfw
                      graphics_compiler_flags)
