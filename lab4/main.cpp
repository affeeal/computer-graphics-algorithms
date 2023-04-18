#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

constexpr GLint kWidth = 640;
constexpr GLint kHeight = 480;

GLfloat frame_buffer[3 * kWidth * kHeight];
std::vector<std::pair<int, int>> points;

void Plot(GLint x, GLint y, GLfloat r, GLfloat g, GLfloat b) {
  auto i = 3 * (y * kWidth + x);
  frame_buffer[i] = r;
  frame_buffer[i + 1] = g;
  frame_buffer[i + 2] = b;
}

void ClearFrameBuffer() {
  for (auto x = 0; x < kWidth; x++) {
    for (auto y = 0; y < kHeight; y++) {
      Plot(x, y, 1, 1, 1);
    }
  }
}

void PlotLine(int x0, int y0, int x1, int y1) {
  auto delta_x = std::abs(x1 - x0);
  auto delta_y = std::abs(y1 - y0);
  auto low = true;
  if (delta_y > delta_x) {
    low = false;
    std::swap(delta_x, delta_y);
  }

  auto e = - 0.5;
  auto delta_e = double(delta_y) / delta_x;

  auto sign_x = (x1 > x0) ? 1 : -1;
  auto sign_y = (y1 > y0) ? 1 : -1;

  auto x = x0;
  auto y = y0;
  for (auto i = 0; i < delta_x; i++) {
    Plot(x, y, 0, 0, 0);

    if (low)
      x += sign_x;
    else
     y += sign_y;
    e += delta_e;

    if (e >= 0) {
      if (low)
        y += sign_y;
      else
        x += sign_x;
      e--;
    }
  }
}

void DrawLines() {
  if (points.size() == 2) {
    PlotLine(points[0].first, points[0].second,
              points[1].first, points[1].second); 
  } else if (points.size() > 2) {
    for (unsigned long i = 0; i < points.size() - 1; i++) {
      PlotLine(points[i].first, points[i].second,
               points[i + 1].first, points[i + 1].second); 
    }
  }
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    ClearFrameBuffer();
    GLdouble x, y;
    glfwGetCursorPos(window, &x, &y);
    auto actual_x = int(x);
    auto actual_y = kHeight - int(y);
    Plot(actual_x, actual_y, 0, 0, 0);
    points.push_back(std::make_pair(actual_x, actual_y));
    DrawLines();
  }
}

int main() {
  if (!glfwInit())
    return -1;

  GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "lab4", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  ClearFrameBuffer();
  glfwMakeContextCurrent(window);
  glfwSetMouseButtonCallback(window, MouseButtonCallback);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawPixels(kWidth, kHeight, GL_RGB, GL_FLOAT, frame_buffer);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
