#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <map>
#include <utility>
#include <vector>

constexpr GLint kWidth = 640;
constexpr GLint kHeight = 640;

constexpr GLint kPartitionX = 640;

enum class Stages {
  kPlotSubject = 0,
  kPlotClipper,
  kDoSubtraction,
} current_stage = Stages::kPlotSubject;

struct Point {
  int x;
  int y;

  explicit Point(int x, int y) : x(x), y(y) {}
};

std::vector<Point> subject;
std::vector<Point> clipper;

GLfloat frame_buffer[3 * kWidth * kHeight];

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
    Plot(Point(x, y), 1, 1, 1);

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

void PlotLines(std::vector<Point>& points) {
  if (points.size() == 2) {
    PlotLine(points[0], points[1]);
  } else if (points.size() > 2) {
    for (unsigned long i = 0; i < points.size() - 1; i++) {
      PlotLine(points[i], points[i + 1]);
    }
    PlotLine(points.back(), points.front());
  }
}

void MouseButtonCallback(
    GLFWwindow* window, int button, int action, int mods) {
  if (current_stage == Stages::kPlotSubject) {
    GLdouble x { 0 }, y { 0 };
    glfwGetCursorPos(window, &x, &y);
    subject.push_back(Point(int(x), kHeight - int(y)));

    ClearFrameBuffer();
    Plot(subject.back(), 1, 1, 1);
    PlotLines(subject);
  } else if (current_stage == Stages::kPlotClipper) {
    GLdouble x { 0 }, y { 0 };
    glfwGetCursorPos(window, &x, &y);
    clipper.push_back(Point(int(x), kHeight - int(y)));

    ClearFrameBuffer();
    Plot(clipper.back(), 1, 1, 1);
    PlotLines(subject);
    PlotLines(clipper);
  } else if (current_stage == Stages::kDoSubtraction) {
    // ...
  }
}

Point FindIntersection(Point p1, Point p2, Point p3, Point p4) {
  auto u_numerator = double(
      (p4.x - p3.x) * (p1.y - p3.y) - (p4.y - p3.y) * (p1.x - p3.x));
  auto u_denominator = double(
      (p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y));
  auto u = u_numerator / u_denominator;
  // TODO: u_denominator == 0

  auto t_numerator = double(
      (p2.x - p1.x) * (p1.y - p3.y) - (p2.y - p1.y) * (p1.x - p3.x));
  auto t_denominator = double(
      (p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y));
  auto t = t_numerator / t_denominator;
  // TODO: t_denominator == 0;

  if (0 <= u && u <= 1) {
    return Point(
        p1.x + u * (p2.x - p1.x),
        p1.y + u * (p2.y - p1.y));
  } else if (0 <= t && t <= 1) {
    return Point(
        p3.x + t * (p4.x - p3.x),
        p3.y + t * (p4.y - p3.y));
  } else {
    // Point can't accept negative values normally
    return Point(-1, -1);
  }
}

void CalculateIntersectionPoints(
    std::vector<Point>& subject, std::vector<Point>& clipper) {
  // TODO: make n*log(m) compexity
  for (unsigned long i = 0; i < subject.size() - 1; i++) {
    for (unsigned long j = 0; j < subject.size() - 1; j++) {
      auto p = FindIntersection(subject[i], subject[i + 1],
                                clipper[j], clipper[j + 1]);
    }
  }
}

void KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
  // TODO: delet
  if (current_stage == Stages::kPlotSubject) {
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
      current_stage = Stages::kPlotClipper;
      std::cout << "current stage: plot clipper" << std::endl;
    }
  } else if (current_stage == Stages::kPlotClipper) {
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
      current_stage = Stages::kDoSubtraction;
      std::cout << "current stage: do subtraction" << std::endl;
    }
  } else if (current_stage == Stages::kDoSubtraction) {
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
      current_stage = Stages::kPlotSubject;
      std::cout << "current stage: plot subject" << std::endl;
      ClearFrameBuffer();
      CalculateIntersectionPoints(subject, clipper);
      subject.clear();
      clipper.clear();
    }
  }
}

                                 

int main() {
  if (!glfwInit())
    return -1;

  GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "lab5", NULL, NULL);
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
