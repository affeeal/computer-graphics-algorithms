add_executable(lab7 main.cpp)
target_link_libraries(lab7 PRIVATE 
                      OpenGL::GL
                      glfw
                      stb
                      graphics_compiler_flags)
