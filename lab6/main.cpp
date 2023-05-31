#include <cmath>
#include <cstddef>
#include <iostream>

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

const size_t kWidth = 600;
const size_t kHeight = 600;
const char* kTitle = "Light and animation";

const GLfloat kDistance = 0.5;
const GLfloat kAlphaDelta = 4;
const GLfloat kBetaDelta = 4;
const GLint kEdgesCountDelta = 1;
const GLint kEdgesCountLowerBound = 6;

GLfloat alpha = 0;
GLfloat beta = 0;
GLfloat radius = 0.5f;
GLint edgesCount = 20;

// Animation
GLfloat x_velocity = 0.015f;
GLfloat y_velocity = 0.012f;
GLfloat x_position = 0.0f;
GLfloat y_position = 0.0f;

// Light
GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat light_duffusion[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

// Texture
GLuint texture_id = 0;

void KeyCallback(GLFWwindow*, int, int, int, int);
void DrawCylinder(GLfloat radius, GLfloat distance,
    GLfloat x_position, GLfloat y_position);
void UpdatePosition();
void SetTexture (const char* name);

int main() {
  if (!glfwInit())
    return -1;

  GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, kTitle, NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, KeyCallback);

  SetTexture("lab6/brick.jpg");

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_duffusion);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glRotated(alpha, 1, 0, 0);
    glRotated(beta, 0, 1, 0);
    UpdatePosition();
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    DrawCylinder(radius, kDistance, x_position, y_position);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

void UpdatePosition() {
  x_position += x_velocity;
  y_position += y_velocity;
  
  if (radius - x_position <= 0.0f || radius - x_position >= 1.0f) {
    x_velocity *= -1;
  }
  
  if (radius - y_position <= 0.0f || radius - y_position >= 1.0f) {
    y_velocity *= -1;
  }
}

void DrawCylinder(GLfloat radius, GLfloat distance,
    GLfloat x_position, GLfloat y_position) {
  glBegin(GL_TRIANGLE_STRIP);

  for (float i = 0; i <= edgesCount; i++) {
    float x = radius * cos(i * 2 * M_PI / edgesCount) + x_position;
    float y = radius * sin(i * 2 * M_PI / edgesCount) + y_position;
    
    glTexCoord2f(x, -0.5);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(x, y, -0.5 * distance);
    
    glTexCoord2f(x, 0.5);
    glVertex3f(x, y, 0.5 * distance);
  }

  glEnd();
}

void SetTexture(const char* filename) {
  int x = 0;
  int y = 0;
  int channels = 0;

  unsigned char* texture = stbi_load(filename, &x, &y, &channels, 0);
  if (!texture) {
    std::cerr << "Failed loading texture" << std::endl;
    return;
  }

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

  stbi_image_free(texture);
}

void KeyCallback(GLFWwindow* window, int key, int, int action, int) {
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    switch (key) {
     case GLFW_KEY_DOWN:
      alpha -= kAlphaDelta;
      break;
     case GLFW_KEY_UP:
      alpha += kAlphaDelta;
      break;
     case GLFW_KEY_RIGHT:
      beta -= kBetaDelta;
      break;
     case GLFW_KEY_LEFT:
      beta += kBetaDelta;
      break;
     case GLFW_KEY_KP_ADD:
      edgesCount += kEdgesCountDelta;
      break;
     case GLFW_KEY_KP_SUBTRACT:
      if (edgesCount > kEdgesCountLowerBound)
        edgesCount -= kEdgesCountDelta;
      break;
     case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GLFW_TRUE);
      break;
     default:
      std::cerr << "No action for pressed key" << std::endl;
    }
  }
}
