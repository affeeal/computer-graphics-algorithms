#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

constexpr GLint kWidth = 640;
constexpr GLint kHeight = 640;
constexpr GLint kPartitionX = 320;

struct Point {
  int x;
  int y;

  explicit Point(int x, int y) : x(x), y(y) {}
};

GLfloat frame_buffer[3 * kWidth * kHeight];
std::vector<Point> points;

void Plot(Point p, GLfloat r = 0, GLfloat g = 0, GLfloat b = 0) {
  auto i = 3 * (p.y * kWidth + p.x - 1);
  frame_buffer[i] = r;
  frame_buffer[i + 1] = g;
  frame_buffer[i + 2] = b;
}

void Invert(Point p) {
  auto i = 3 * (p.y * kWidth + p.x - 1);
  // TODO: исправить уродство ниже
  frame_buffer[i] = float(int(frame_buffer[i]) ^ 1);
  frame_buffer[i + 1] = float(int(frame_buffer[i + 1]) ^ 1);
  frame_buffer[i + 2] = float(int(frame_buffer[i + 2]) ^ 1);
}

void InvertHorizontal(Point p) {
  // TODO: убрать прорез
  auto sign_x = (p.x < kPartitionX) ? 1 : -1;
  for (auto x = p.x; x != kPartitionX; x += sign_x) {
    Invert(Point(x, p.y));
  }
}

void ClearFrameBuffer() {
  for (auto x = 0; x < kWidth; x++) {
    for (auto y = 0; y < kHeight; y++) {
      Plot(Point(x, y));
    }
  }
}

void PlotLine(Point p0, Point p1) {
  auto delta_x = std::abs(p1.x - p0.x);
  auto delta_y = std::abs(p1.y - p0.y);
  auto low = true;
  if (delta_y > delta_x) {
    low = false;
    std::swap(delta_x, delta_y);
  }

  auto e = - 0.5;
  auto delta_e = double(delta_y) / delta_x;

  auto sign_x = (p1.x > p0.x) ? 1 : -1;
  auto sign_y = (p1.y > p0.y) ? 1 : -1;

  auto x = p0.x;
  auto y = p0.y;
  for (auto i = 0; i < delta_x; i++) {
    Plot(Point(x, y), 1, 0, 0);

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

void PlotLines() {
  if (points.size() == 2) {
    PlotLine(points[0], points[1]);
  } else if (points.size() > 2) {
    for (unsigned long i = 0; i < points.size() - 1; i++) {
      PlotLine(points[i], points[i + 1]);
    }
    PlotLine(points.back(), points.front());
  }
}

void FillFace(Point p0, Point p1, std::map<int, int>& overlap) {
  auto delta_x = std::abs(p1.x - p0.x);
  auto delta_y = std::abs(p1.y - p0.y);
  auto low = true;
  if (delta_y > delta_x) {
    low = false;
    std::swap(delta_x, delta_y);
  }

  auto error = - 0.5;
  auto delta_error = double(delta_y) / delta_x;

  auto sign_x = (p1.x > p0.x) ? 1 : -1;
  auto sign_y = (p1.y > p0.y) ? 1 : -1;

  auto x = p0.x;
  auto y = p0.y;
  auto previous_y = y;
  for (auto i = 0; i < delta_x - 1; i++) {
    if (low)
      x += sign_x;
    else
     y += sign_y;
    error += delta_error;

    if (error >= 0) {
      if (low)
        y += sign_y;
      else
        x += sign_x;
      error--;
    }

    if (y != previous_y) {
      overlap[y]++;
      InvertHorizontal(Point(x, y));
    }

    previous_y = y;
  }
}

void FillFaces() {
  if (points.size() <= 2) {
    return;
  }
  
  std::map<int, int> overlap;
  for (unsigned long i = 0; i < points.size() - 1; i++) {
    FillFace(points[i], points[i + 1], overlap);
  }
  FillFace(points.back(), points.front(), overlap);

  for (auto p : points) {
    if (overlap[p.y] % 2 == 1) {
      InvertHorizontal(p);
    }
  }
}

void MouseButtonCallback(GLFWwindow* window,
                         int button,
                         int action,
                         int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    ClearFrameBuffer();
    
    GLdouble x, y;
    glfwGetCursorPos(window, &x, &y);
    points.push_back(Point(int(x), kHeight - int(y)));
    
    Plot(points.back(), 1, 0, 0);
    FillFaces();
    PlotLines();
  }
}

void KeyCallback(GLFWwindow* window,
                 int key,
                 int scancode,
                 int action,
                 int mods) {
  if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
    ClearFrameBuffer();
    points.clear();
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
  glfwSetKeyCallback(window, KeyCallback);

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
