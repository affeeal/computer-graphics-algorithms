#pragma once

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow *window);

unsigned int CreateShaderProgram(
    const char* vertex_shader_path, const char* fragment_shader_path);
unsigned int CreateTexture(const char* texture_path);

std::vector<double> CreateCylinderCoordinates(
    unsigned int sector_count, double radius, double height);
std::vector<unsigned int> CreateCylinderIndices(unsigned int sector_count);

