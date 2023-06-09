#pragma once

#include <vector>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

extern const int kWindowWidth;
extern const int kWindowHeight;
extern const char* kWindowTitle;

extern const GLsizei kViewportWidth;
extern const GLsizei kViewportHeight;

extern const std::string kPathPrefix;
extern const std::string kVertexShaderPath;
extern const std::string kFragmentShaderPath;
extern const std::string kTexturePath;

extern unsigned int kCylinderSectorCount;

extern const float kCylinderHeight;
extern const float kCylinderHalfHeight;
extern const float kCylinderRadius;

extern const float kAlphaChanging;
extern const float kBetaChanging;

extern const float kXVelocity;
extern const float kYVelocity;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window, float& alpha, float& beta);

unsigned int CreateShaderProgram(const char* vertex_shader_path,
                                 const char* fragment_shader_path);
unsigned int CreateTexture(const char* texture_path);

std::vector<double> CreateCylinderCoordinates(unsigned int sector_count,
                                              double radius, double height);
std::vector<unsigned int> CreateCylinderIndices(unsigned int sector_count);

void UpdatePosition(float& x_offset, float& y_offset,
                    int& x_direction, int& y_direction);

