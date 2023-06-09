#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils.hpp"

int main() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

  GLFWwindow* window {
    glfwCreateWindow(kWindowWidth, kWindowHeight, kWindowTitle,
                     nullptr, nullptr)
  };
  
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  
  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

  glfwMakeContextCurrent(window);
  
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
    return -1;
  }
  
  glViewport(0, 0, kViewportWidth, kViewportHeight);

  unsigned int shader_program {
    CreateShaderProgram(kVertexShaderPath.c_str(), kFragmentShaderPath.c_str())
  };
  
  if (shader_program == -1) {
    glfwTerminate();
    return -1;
  }
  
  glUseProgram(shader_program);

  int transform_loc {
    glGetUniformLocation(shader_program, "transform")
  };
  
  if (transform_loc == -1) {
    std::cerr << "Failed to get transform uniform location" << std::endl;
    glfwTerminate();
    return -1;
  }
  
  unsigned int texture { CreateTexture(kTexturePath.c_str()) };
  if (texture == -1) {
    glfwTerminate();
    return -1;
  }
  
  glBindTexture(GL_TEXTURE_2D, texture);

  std::vector<double> coordinates {
    CreateCylinderCoordinates(kCylinderSectorCount, kCylinderRadius,
                              kCylinderHeight)
  };

  std::vector<unsigned int> indices {
    CreateCylinderIndices(kCylinderSectorCount)
  };

  unsigned int vao { 0 };
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  unsigned int vbo { 0 };
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, coordinates.size() * sizeof(double),
               coordinates.data(), GL_DYNAMIC_DRAW);

  unsigned int ebo { 0 };
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(double),
               indices.data(), GL_DYNAMIC_DRAW);

  const unsigned int kStride { 5 * sizeof(double) };
  void* position_offset { reinterpret_cast<void*>(0) };
  glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, kStride, position_offset);
  glEnableVertexAttribArray(0);

  void* texture_offset { reinterpret_cast<void*>(3 * sizeof(double)) };
  glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, kStride, texture_offset);
  glEnableVertexAttribArray(1); 
  
  // углы поворота вокруг векторов (1, 0, 0) и (0, 1, 0) соответственно
  float alpha { 0 };
  float beta { 0 };
  
  // центр фигуры
  float x_offset { 0 };
  float y_offset { 0 };
  
  // направление движения фигуры
  int x_direction { 1 };
  int y_direction { 1 };
  
  glEnable(GL_DEPTH_TEST);
  
  while (!glfwWindowShouldClose(window)) {
    ProcessInput(window, alpha, beta);
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    UpdatePosition(x_offset, y_offset, x_direction, y_direction);
    
    glm::mat4 transform { glm::mat4(1.0f) };
    transform = glm::translate(transform, glm::vec3(x_offset, y_offset, 0.0f));
    transform = glm::rotate(transform, alpha, glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, beta, glm::vec3(0.0f, 1.0f, 0.0f));
    
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(transform));
    
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

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

