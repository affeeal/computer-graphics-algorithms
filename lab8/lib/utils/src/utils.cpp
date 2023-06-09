#include "utils.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <stb/stb_image.h>

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow *window) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

unsigned int CreateShaderProgram(
    const char* vertex_shader_path, const char* fragment_shader_path) {
  std::string vertex_shader_source_string { };
  std::string fragment_shader_source_string { };
  
  std::ifstream vertex_shader_file;
  std::ifstream fragment_shader_file;
  
  vertex_shader_file.exceptions(
      std::ifstream::failbit | std::ifstream::badbit);
  fragment_shader_file.exceptions(
      std::ifstream::failbit | std::ifstream::badbit);
  
  try {
    vertex_shader_file.open(vertex_shader_path);
    fragment_shader_file.open(fragment_shader_path);
    
    std::stringstream vertex_shader_stream, fragment_shader_stream;
    vertex_shader_stream << vertex_shader_file.rdbuf();
    fragment_shader_stream << fragment_shader_file.rdbuf();		
    
    vertex_shader_file.close();
    fragment_shader_file.close();
    
    vertex_shader_source_string = vertex_shader_stream.str();
    fragment_shader_source_string = fragment_shader_stream.str();		
  } catch(std::ifstream::failure& e) {
    std::cerr << "Failed to read shader files" << std::endl;
    return -1;
  }
  
  const char* vertex_shader_source = vertex_shader_source_string.c_str();
  const char* fragment_shader_source = fragment_shader_source_string.c_str();
    
  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);

  const std::size_t kInfoLogSize = 512;
  char* info_log = new char[kInfoLogSize];
  int success = 0;
  
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex_shader, kInfoLogSize, NULL, info_log);
    std::cerr << "Failed to compile the vertex shader"
              << info_log << std::endl;
    return -1;
  }
  
  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment_shader, kInfoLogSize, NULL, info_log);
    std::cerr << "Failed to compile the fragment shader"
              << info_log << std::endl;
    return -1;
  }
  
  unsigned int shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader_program, kInfoLogSize, NULL, info_log);
    std::cerr << "Failed to compile the shader program"
              << info_log << std::endl;
    return -1;
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  
  delete[] info_log;

  return shader_program;
}

unsigned int CreateTexture(const char* texture_path) {
  unsigned int texture = 0;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
      GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
      GL_LINEAR);
  
  int width = 0, height = 0, channels = 0;
  unsigned char* data = stbi_load(texture_path, &width, &height,
      &channels, 0); 
  
  if (!data) {
    std::cerr << "Failed to load the texture" << std::endl;
    return -1;
  }
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
      GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);

  return texture;
}

std::vector<double> CreateCylinderCoordinates(
    unsigned int sector_count, double radius, double height) {
  assert(sector_count >= 3);
  
  // Для каждой вершины последовательно записываются 3 позиционные и 2
  // текстурные координаты. На каждой итерации цикла ниже в вектор заносятся
  // по 5 координат двух вершин, имеющих одинаковые позиционные координаты x и
  // y, но противоположные z. Отсюда 10 = 2 * (3 + 2).
  std::vector<double> coordinates(10 * (sector_count + 1));
  
  const double kSectorStep {2.0f * M_PI / sector_count};
  
  double sector_angle {0};
  double position_x {0};
  double position_y {0};
  double texture_x {0};
  for (unsigned int i {0}, j {0}; i <= sector_count; i++) {
    sector_angle = i * kSectorStep;
    position_x = radius * std::cos(sector_angle);
    position_y = radius * std::sin(sector_angle);
    texture_x = static_cast<double>(i) / sector_count;
    std::cout << position_x << ' ' << position_y << ' ' << texture_x << std::endl;
    
    // позиционные координаты верхней вершины
    coordinates[j++] = position_x;
    coordinates[j++] = position_y;
    coordinates[j++] = 0.5f * height;

    // текстурные координаты верхней вершины
    coordinates[j++] = texture_x;
    coordinates[j++] = 1.0f;
    
    // позиционные координаты нижней вершины
    coordinates[j++] = position_x;
    coordinates[j++] = position_y;
    coordinates[j++] = -0.5f * height;

    // текстурные координаты нижней вершины
    coordinates[j++] = texture_x;
    coordinates[j++] = 0.0f;
  }

  return coordinates;
}

std::vector<unsigned int> CreateCylinderIndices(unsigned int sector_count) {
  assert(sector_count >= 3);
  
  // Грань цилиндра строится из двух треугольников, каждый из которых задаётся
  // тремя индексами. Отсюда 6 = 2 * 3.
  std::vector<unsigned int> indices(6 * sector_count);
  
  unsigned int double_i {0};
  for (unsigned int i {0}, j {0}; i < sector_count; i++) {
    double_i = i * 2;

    // верхняя левая вершина
    indices[j++] = double_i;
    // нижняя левая вершина
    indices[j++] = double_i + 1;
    // верхняя правая вершина
    indices[j++] = double_i + 2;

    // нижняя левая вершина
    indices[j++] = double_i + 1;
    // верхняя правая вершина
    indices[j++] = double_i + 2;
    // нижняя правая вершина
    indices[j++] = double_i + 3;
  }

  return indices;
}

