#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "utils.hpp"

const int kWindowWidth = 800;
const int kWindowHeight = 600;
const char* kWindowTitle = "Title";

const GLsizei kViewportWidth = kWindowWidth;
const GLsizei kViewportHeight = kWindowHeight;

const std::string kPathPrefix = "lab8/data/";
const std::string kVertexShaderPath = kPathPrefix + "vertex_shader.glsl";
const std::string kFragmentShaderPath = kPathPrefix + "fragment_shader.glsl";
const std::string kTexturePath = kPathPrefix + "texture.jpg";
  
int main() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

  GLFWwindow* window
    = glfwCreateWindow(kWindowWidth, kWindowHeight, kWindowTitle, NULL, NULL);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
  
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
    return -1;
  }
  
  glViewport(0, 0, kViewportWidth, kViewportHeight);

  unsigned int shader_program = CreateShaderProgram(kVertexShaderPath.c_str(),
      kFragmentShaderPath.c_str());
  if (shader_program == -1) {
    glfwTerminate();
    return -1;
  }

  unsigned int texture = CreateTexture(kTexturePath.c_str());
  if (texture == -1) {
    glfwTerminate();
    return -1;
  }

  float vertices[] = {
     // positions       // texture
     0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
  };
  
  unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
  }; 

  unsigned int vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  unsigned int vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  unsigned int ebo = 0;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
      GL_STATIC_DRAW);

  const std::size_t kStride = 5 * sizeof(float);
  
  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, kStride,
      reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  // texture attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, kStride,
      reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1); 

  glUseProgram(shader_program);

  while (!glfwWindowShouldClose(window)) {
    ProcessInput(window);
    
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
  glDeleteProgram(shader_program);

  glfwTerminate();

  return 0;
}

