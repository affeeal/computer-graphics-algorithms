add_executable(lab6 main.cpp)

target_link_libraries(lab6 PRIVATE 
                      OpenGL::GL
                      glfw
                      stb
                      graphics_compiler_flags)
