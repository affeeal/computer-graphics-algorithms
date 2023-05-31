#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <cstddef>
#include <iostream>

const size_t kWidth = 600;
const size_t kHeight = 600;

const char* kTitle = "Light and animation";

const GLdouble kAlphaDelta = 4;
const GLdouble kBetaDelta = 4;

const GLint kEdgesCountDelta = 1;
const GLint kEdgesCountLowerBound = 6;

GLdouble beta = 0;
GLdouble alpha = 0;
GLint edgesCount = 6;

void KeyCallback(GLFWwindow*, int, int, int, int);
void DrawCylinder(GLdouble radius, GLdouble distance);

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

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glRotated(alpha, 1, 0, 0);
    glRotated(beta, 0, 1, 0);
    DrawCylinder(0.5, 0.5);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

void DrawCylinder(GLdouble radius, GLdouble distance) {
  glBegin(GL_TRIANGLE_STRIP);

  for (double i = 0; i <= edgesCount; i++) {
    double x = radius * cos(i * 2 * M_PI / edgesCount);
    double y = radius * sin(i * 2 * M_PI / edgesCount);

    glColor3d(1 - i / edgesCount, i / edgesCount, 1);
    glVertex3d(x, y, - 0.5 * distance);
    glVertex3d(x, y,   0.5 * distance);
  }

  glEnd();
}

void KeyCallback(GLFWwindow* window, int key, int, int action, int) {
  if (action == GLFW_PRESS) {
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
  } else if (action == GLFW_REPEAT) {
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
     default:
      std::cerr << "No action for repeated key" << std::endl;
    }
  }
}
