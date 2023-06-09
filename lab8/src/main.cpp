#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

  GLFWwindow* window {
    glfwCreateWindow(kWindowWidth, kWindowHeight, kWindowTitle, NULL, NULL)
  };
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
  glEnable(GL_DEPTH_TEST);

  unsigned int shader_program {
    CreateShaderProgram(kVertexShaderPath.c_str(), kFragmentShaderPath.c_str())
  };
  if (shader_program == -1) {
    glfwTerminate();
    return -1;
  }

  unsigned int texture {
    CreateTexture(kTexturePath.c_str())
  };
  if (texture == -1) {
    glfwTerminate();
    return -1;
  }

  const double kCylinderHeight {0.6f};
  const double kCylinderRadius {0.4f};
  unsigned int cylinder_sector_count {8};
  
  std::vector<double> coordinates {
    CreateCylinderCoordinates(cylinder_sector_count, kCylinderRadius,
                              kCylinderHeight)
  };

  std::vector<unsigned int> indices {
    CreateCylinderIndices(cylinder_sector_count)
  };
  
  unsigned int vao {0};
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  unsigned int vbo {0};
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, coordinates.size() * sizeof(double),
               coordinates.data(), GL_DYNAMIC_DRAW);

  unsigned int ebo {0};
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(double),
               indices.data(), GL_DYNAMIC_DRAW);

  const unsigned int kStride = 5 * sizeof(double);
  
  // позиционные координаты
  glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, kStride,
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  // текстурные координаты
  glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, kStride,
                        reinterpret_cast<void*>(3 * sizeof(double)));
  glEnableVertexAttribArray(1); 

  while (!glfwWindowShouldClose(window)) {
    ProcessInput(window);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBindTexture(GL_TEXTURE_2D, texture);

    glm::mat4 transform {glm::mat4(1.0f)};
    transform = glm::rotate(
        transform,
        static_cast<float>(glfwGetTime()),
        glm::vec3(1.0f, 1.0f, 0.0f));

    glUseProgram(shader_program);
    int transform_location {
      glGetUniformLocation(shader_program, "transform")
    };
    
    if (transform_location == -1) {
      std::cerr << "Failed to get uniform location" << std::endl;
      break;
    }
    
    glUniformMatrix4fv(transform_location, 1, GL_FALSE,
        glm::value_ptr(transform));
    
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

