#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

#include <GLFW/glfw3.h>

enum class Direction {
    kClockwise,
    kCounterclockwise
};

enum class State {
    kSubject,
    kSubjectHole,
    kCLipper,
    kCLipperHole,
    kUnion
};

struct Point {
    GLdouble x, y;
    int index_figure, index_clipper;
    bool enter, exit;
    
    Point();
    Point(GLdouble x, GLdouble y, int index_figure, int index_clipper);
};

struct Polygon {
    std::vector<Point> points;
    bool intersection;
    
    Polygon();
};

const size_t kMonitorWidth = 1920;
const size_t kMonitorHeight = 1080;

const size_t kScreenWidth = 800;
const size_t kScreenHeight = 600;

const char * kTitle = "Clipping";

const GLfloat kBackgroundRed = 0.0f;
const GLfloat kBackgroundGreen = 0.0f;
const GLfloat kBackgroundBlue = 0.0f;
const GLfloat kBackgroundAplha = 1.0f;

const GLfloat kSubjectRed = 1.0f;
const GLfloat kSubjectGreen = 0.0f;
const GLfloat kSubjectBlue = 0.0f;

const GLfloat kClipperRed = 0.0f;
const GLfloat kClipperGreen = 0.0f;
const GLfloat kClipperBlue = 1.0f;

const GLfloat kResultRed = 1.0f;
const GLfloat kResultGreen = 1.0f;
const GLfloat kResultBlue = 1.0f;

int window_width;
int window_height;

State state = State::kSubject;

bool need_to_fill = false;

GLfloat *frame_buffer;

std::vector<Polygon> subject;
std::vector<Polygon> clipper;
std::vector<Polygon> result;

GLFWwindow *CreateWindow();

void ResetViewProjectionBuffer();

void FramebufferSizeCallback(GLFWwindow * window, int width, int height);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseCallback(GLFWwindow* window, int button, int action, int mods);

std::vector<Polygon> WeilerAtherton(std::vector<Polygon> figure,
                                    std::vector<Polygon> clipper);

void ReverseDirectionIfNeeded(Polygon &polygon, Direction direction);
int FindPointInPolygon(const Point &point, const Polygon &polygon);
void Insert(std::vector<Point> &points, Point check_point, Point point);
bool IsPointInsidePolygon(const Point &point, const Polygon &polygon);

void Draw();
void Colorify();

void SetPixelColor(int x, int y, GLubyte r, GLubyte g, GLubyte b);
int GetPixelIndex(int x, int y);

void ClearFrameBuffer();

void DrawLines(Polygon polygon, GLfloat R, GLfloat G, GLfloat B);
void DrawLine(GLint x1, GLint y1, GLint x2, GLint y2, GLfloat R, GLfloat G, GLfloat B);

template <typename T>
int Sign(T val);
void SetPixelColor(int x, int y, GLfloat R, GLfloat G, GLfloat B);
int GetPixelIndex(int x, int y);

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    GLFWwindow *window = CreateWindow();
    if (window == nullptr) {
        glfwTerminate();
        return 1;
    }

    glfwGetFramebufferSize(window, &window_width, &window_height);
    ResetViewProjectionBuffer();
    
    while (!glfwWindowShouldClose(window)) {
        glClearColor(kBackgroundRed, kBackgroundGreen, 
                     kBackgroundBlue, kBackgroundAplha);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawPixels(window_width, window_height, GL_RGB, GL_FLOAT, frame_buffer);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}

Point::Point() : x(-1.0), y(-1.0), index_figure(-1), index_clipper(-1),
        enter(false), exit(false) {
}

Point::Point(GLdouble x, GLdouble y, int index_figure, int index_clipper) : 
        x(x), y(y), 
        index_figure(index_figure), index_clipper(index_clipper),
        enter(false), exit(false) {
}

Polygon::Polygon() : intersection(false) {
}

GLFWwindow *CreateWindow() {
    if (! glfwInit()) {
        return nullptr;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2); // Основная версия.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0); // Суб-версия.

    GLFWwindow* window = glfwCreateWindow(kScreenWidth, kScreenHeight, kTitle, nullptr, nullptr);    
    if (window == nullptr) {
        return nullptr;
    }
    
    glfwSetWindowPos(window, (kMonitorWidth - kScreenWidth) / 2, (kMonitorHeight - kScreenHeight) / 2);
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseCallback);

    return window;
}

void ResetViewProjectionBuffer() {
    glViewport(0, 0, window_width, window_height);

	  glMatrixMode(GL_PROJECTION);
	  glLoadIdentity();
    
	  glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ClearFrameBuffer();
    
    for (size_t i=0; i<subject.size(); i++) {
        subject[i].points.clear();
    }
    subject.clear();
    
    for (size_t i=0; i<clipper.size(); i++) {
        clipper[i].points.clear();
    }
    clipper.clear();
    
    for (size_t i = 0; i < result.size(); i++) {
        result[i].points.clear();
    }
    result.clear();
    
    state = State::kSubject;
    need_to_fill = false;
}

void FramebufferSizeCallback([[maybe_unused]]GLFWwindow * window,
                             int width, int height) {
    window_width = width;
    window_height = height;
    ResetViewProjectionBuffer();
}

void KeyCallback(GLFWwindow* window, int key, [[maybe_unused]] int scancode,
                 int action, [[maybe_unused]] int mods) {
    if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS)) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }
    
    if ((key == GLFW_KEY_BACKSPACE) && (action == GLFW_PRESS)) {
        ResetViewProjectionBuffer();
        return;
    }

    if ((key == GLFW_KEY_ENTER) && (action == GLFW_PRESS)) {
        switch (state) {
        case State::kSubject: {
            subject.emplace_back(Polygon());
            state = State::kSubjectHole;
            break;
        }
        case State::kSubjectHole: {
            subject.emplace_back(Polygon());
            break;
        }
        case State::kCLipper: {
            clipper.emplace_back(Polygon());
            state = State::kCLipperHole;
            break;
        }
        case State::kCLipperHole: {
            clipper.emplace_back(Polygon());
            break;
        }
        default: {
        }
        }
    } 

    if ((key == GLFW_KEY_SPACE) && (action == GLFW_PRESS)) {
        switch (state) {
        case State::kSubject: {
            subject.emplace_back(Polygon());
            state = State::kSubjectHole;
            break;
        }
        case State::kSubjectHole: {
            state = State::kCLipper;
            break;
        }
        case State::kCLipper: {
            clipper.emplace_back(Polygon());
            state = State::kCLipperHole;
            break;
        }
        default: {
        }
        }
    }

    if ((key == GLFW_KEY_U) && (action == GLFW_PRESS)
        && (state == State::kCLipperHole || state == State::kUnion)) {
        if (subject[subject.size() - 1].points.empty()) {
            subject.pop_back();
        }
        if (clipper[clipper.size() - 1].points.empty()) {
            clipper.pop_back();
        }
        state = State::kUnion;
        result = WeilerAtherton(subject, clipper);
    }
    if ((key == GLFW_KEY_C) && (action == GLFW_PRESS)) {
        need_to_fill = !need_to_fill;
    }

    ClearFrameBuffer();
    Draw();
}

void MouseCallback(GLFWwindow* window, int button, int action,
                   [[maybe_unused]] int mods) {
    GLdouble x = 0;
    GLdouble y = 0;
    glfwGetCursorPos(window, &x, &y);

    if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS)) {
        switch (state) {
        case State::kSubject: {
            if (subject.empty()) {
                subject.emplace_back(Polygon());
                subject[0].points.emplace_back(Point(x, y, 0, 0));
            } else {
                subject[0].points.emplace_back(Point(x, y, 0, 0));
            }
            break;
        }
        case State::kSubjectHole: {
            subject[subject.size() - 1].points.emplace_back(Point(x, y, subject.size() - 1, 0));
            break;
        }
        case State::kCLipper: {
            if (clipper.empty()) {
                clipper.emplace_back(Polygon());
                clipper[0].points.emplace_back(Point(x, y, 0, 0));
            } else {
                clipper[0].points.emplace_back(Point(x, y, 0, 0));
            }
            break;
        }
        case State::kCLipperHole: {
            clipper[clipper.size() - 1].points.emplace_back(Point(x, y, 0, clipper.size() - 1));
            break;
        }
        default: {
            return;
        }
        }
    }

    ClearFrameBuffer();
    Draw();
}

std::vector<Polygon> WeilerAtherton(std::vector<Polygon> figure, std::vector<Polygon> clipper) {
    ReverseDirectionIfNeeded(figure[0], Direction::kClockwise);
    for (size_t i = 1; i < figure.size(); i++) {
        ReverseDirectionIfNeeded(figure[i], Direction::kCounterclockwise);
    }
    
    ReverseDirectionIfNeeded(clipper[0], Direction::kClockwise);
    for (size_t i = 1; i < clipper.size(); i++) {
        ReverseDirectionIfNeeded(clipper[i], Direction::kCounterclockwise);
    }
    
    std::vector<Point> enters;
    std::vector<Point> exits;
    std::vector<Polygon> updated_subject;
    std::vector<Polygon> updated_clipper;
    updated_clipper = clipper;
    
    for (size_t k = 0; k < figure.size(); k++) {
        updated_subject.emplace_back(figure[k]);
        for (size_t i = 0; i < figure[k].points.size(); i++) {
            int next1 = (i == figure[k].points.size() - 1) ? 0 : (i + 1);
            GLdouble a1 = figure[k].points[i].y - figure[k].points[next1].y;
            GLdouble b1 = figure[k].points[next1].x - figure[k].points[i].x;
            GLdouble c1 = figure[k].points[i].x * figure[k].points[next1].y - 
                figure[k].points[next1].x * figure[k].points[i].y;
            for (size_t l = 0; l < clipper.size(); l++){
                for (size_t j = 0; j < clipper[l].points.size(); j++) {
                    int next2  = (j == clipper[l].points.size() - 1) ? 0 : (j + 1);
                    GLdouble a2 = clipper[l].points[j].y - clipper[l].points[next2].y;
                    GLdouble b2 = clipper[l].points[next2].x - clipper[l].points[j].x;
                    GLdouble c2 = clipper[l].points[j].x * clipper[l].points[next2].y - 
                        clipper[l].points[next2].x * clipper[l].points[j].y;
                    GLdouble k1 = a2 * figure[k].points[i].x + b2 * figure[k].points[i].y + c2;
                    GLdouble k2 = a2 * figure[k].points[next1].x + b2 * figure[k].points[next1].y  + c2;
                    GLdouble k3 = a1 * clipper[l].points[j].x + b1 * clipper[l].points[j].y  + c1;
                    GLdouble k4 = a1 * clipper[l].points[next2].x + b1 * clipper[l].points[next2].y  + c1;
                    if ((k1 * k2 >= 0) || (k3 * k4 >= 0)) {
                        continue;
                    }
                    GLdouble vec = k1 / (k1 - k2);
                    Point intersection(figure[k].points[i].x + vec * b1, figure[k].points[i].y - vec * a1, k, l);
                    updated_subject.back().intersection = true;
                    updated_clipper[l].intersection = true;
                    Insert(updated_clipper[l].points, clipper[l].points[j], intersection);
                    Insert(updated_subject.back().points, figure[k].points[i], intersection);
                }
            }
        }
    }
    
    bool exit = false, flag = false, inside = false;
    for (size_t j = 0; j < updated_subject.size(); j++) {
        for (size_t i = 0; i < updated_subject[j].points.size(); i++) {
            int index = FindPointInPolygon(updated_subject[j].points[i], figure[j]);
            if (index >= 0) {
                for (size_t k = 0; k < updated_clipper.size(); k++) {
                    inside = IsPointInsidePolygon(updated_subject[j].points[i], clipper[k]);
                    if (k > 0) {
                        if (inside) {
                            flag = true;
                            exit = false;
                            break;
                        }
                    } else if(inside) {
                        flag = exit = true;
                    }
                }
                if (!flag) {
                    exit = false;
                }
                flag = false;
            } else {
                index = FindPointInPolygon(updated_subject[j].points[i], updated_clipper[updated_subject[j].points[i].index_clipper]);
                if (exit) {
                    updated_subject[j].points[i].exit = true;
                    updated_clipper[updated_subject[j].points[i].index_clipper].points[index].exit = true;
                    exits.emplace_back(updated_subject[j].points[i]);
                } else {
                    updated_subject[j].points[i].enter = true;
                    updated_clipper[updated_subject[j].points[i].index_clipper].points[index].enter = true;
                    enters.emplace_back(updated_subject[j].points[i]);
                }
                exit = !exit;
            }
        }
    }
    
    clipper = updated_clipper;
    figure = updated_subject;

    std::vector<Polygon> result;
    bool in_polygon = false;
    bool in_hole = false;

    for (size_t i = 0; i < figure.size(); i++) {
        if (!figure[i].intersection) {
            in_polygon = in_hole = false;
            for (size_t j = 0; j < clipper.size(); j++) {
                if (j == 0 && IsPointInsidePolygon(figure[i].points[0], clipper[j])) {
                    in_polygon = true;
                } else if (j > 0 && IsPointInsidePolygon(figure[i].points[0], clipper[j])) {
                    in_hole = true;
                }
            }
            if (!(in_polygon && !in_hole)) {
                result.emplace_back(figure[i]);
            }
        }
    }
    

    for (size_t i = 0; i < clipper.size(); i++){
        if (!clipper[i].intersection) {
            in_polygon = in_hole = false;
            for (size_t j = 0; j < figure.size(); j++) {
                if (j == 0 && IsPointInsidePolygon(clipper[i].points[0], figure[j])) {
                    in_polygon = true;
                } else if (j > 0 && IsPointInsidePolygon(clipper[i].points[0], figure[j])) {
                    in_hole = true;
                }
            }
            if (!(in_polygon && !in_hole)) {
                result.emplace_back(clipper[i]);
            }
        }
    }

    while (!exits.empty()) {
        std::vector<Point> points;
        Point start = exits.back();
        Point point;
        exits.pop_back();
        points.emplace_back(start);
        Polygon s = figure[start.index_figure];
        int index = FindPointInPolygon(start, s);
        if (index < (int) s.points.size() - 1) {
            point = s.points[index+1];
        } else {
            point = s.points[0];
        }
        while (!((point.x == start.x) && (point.y == start.y))) {
            index = FindPointInPolygon(point, s);
            point = s.points[index];
            while (point.enter == 0) {
                if (point.x == start.x && point.y == start.y) {
                    break;
                }
                points.emplace_back(point);
                index += 1;
                if (index < (int) s.points.size()) {
                    point = s.points[index];
                } else{
                    point = s.points[0];
                    index = 0;
                }
            }
            Polygon c = clipper[point.index_clipper];
            index = FindPointInPolygon(point, c);
            point = c.points[index];
            while (point.exit == 0) {
                if (point.x == start.x && point.y == start.y) {
                    break;
                }
                points.emplace_back(point);
                index += 1;
                if (index < (int) c.points.size()) {
                    point = c.points[index];
                } else {
                    point = c.points[0];
                    index = 0;
                }
            }
            s = figure[point.index_figure];
        }
        Polygon polygon;
        polygon.points = points;
        result.emplace_back(polygon);
    }
    
    return result;
}

void ReverseDirectionIfNeeded(Polygon &polygon, Direction direction) {
    GLdouble sum = 0.0;
    for (size_t i = 0; i < polygon.points.size() - 1; i++) {
        sum += polygon.points[i].x * polygon.points[i + 1].y - polygon.points[i + 1].x * polygon.points[i].y;
    }
    sum += polygon.points[polygon.points.size() - 1].x * polygon.points[0].y - polygon.points[0].x * polygon.points[polygon.points.size() - 1].y;

    if (((sum > 0.0) && (direction == Direction::kCounterclockwise)) || ((sum < 0.0) && (direction == Direction::kClockwise))) {
        std::reverse(polygon.points.begin(), polygon.points.end());
    }
}

int FindPointInPolygon(const Point &point, const Polygon &polygon) {
    for (size_t i = 0; i < polygon.points.size(); i++) {
        if ((polygon.points[i].x == point.x) && (polygon.points[i].y == point.y)) {
            return i;
        }
    }
    return -1;
}

bool IsPointInsidePolygon(const Point &point, const Polygon &polygon) {
    bool inside = false;
    for (size_t i = 0, j = polygon.points.size() - 1; i < polygon.points.size(); j = i++) {
        if ((((polygon.points[i].y <= point.y) && (point.y < polygon.points[j].y)) ||
             ((polygon.points[j].y <= point.y) && (point.y < polygon.points[i].y))) &&
            (point.x > (polygon.points[j].x - polygon.points[i].x) * (point.y - polygon.points[i].y) / 
             (polygon.points[j].y - polygon.points[i].y) + polygon.points[i].x)) {
            inside = !inside;
        }
    }
    return inside;
}

void Insert(std::vector<Point> &points, Point check_point, Point point) {
    size_t i;
    for (i = 0; i < points.size(); i++) {
        if ((points[i].x == check_point.x) && (points[i].y == check_point.y)) {
            break;
        }
    }
    while ((i + 1 != points.size()) && 
           (std::sqrt((point.x - check_point.x) * (point.x - check_point.x) +
                      (point.y - check_point.y) * (point.y-check_point.y)) >
            std::sqrt((points[i + 1].x - check_point.x) * (points[i + 1].x - check_point.x) +
                      (points[i + 1].y - check_point.y) * (points[i + 1].y - check_point.y)))) {
        i++;
    }
    points.insert(points.begin() + (i + 1), point);
}

void Draw() {
    for (size_t i = 0; i < subject.size(); i++) {
        DrawLines(subject[i], kSubjectRed, kSubjectGreen, kSubjectBlue);
    }

    for (size_t i = 0; i < clipper.size(); i++) {
        DrawLines(clipper[i], kClipperRed, kClipperGreen, kClipperBlue);
    }
    
    for (size_t i = 0; i < result.size(); i++) {
        DrawLines(result[i], kResultRed, kResultGreen, kResultBlue);
    }

    if (need_to_fill && result.size() != 0) {
        Colorify();
    }
}

void Colorify() {
    for (int y = 0; y < window_height; y++) {
        bool flag = false;
        int x = 0;
        while (x < window_width) {
            int index = GetPixelIndex(x, y);
            GLfloat R = frame_buffer[index];
            GLfloat G = frame_buffer[index + 1];
            GLfloat B = frame_buffer[index + 2];
            if ((R == 1.0f) && (G == 1.0f) && (B == 1.0f)) {
                while (true) {
                    x++;
                    index = GetPixelIndex(x, y);
                    R = frame_buffer[index];
                    G = frame_buffer[index + 1];
                    B = frame_buffer[index + 2];
                    if ((R != kResultRed) || (G != kResultGreen) || (B != kResultBlue)) {
                        break;
                    }
                }
                flag = !flag;
            }
            if (flag) {
                bool flag1 = false;
                int x_tmp = x;
                while (x_tmp < window_width) {
                    index = GetPixelIndex(x_tmp, y);
                    R = frame_buffer[index];
                    G = frame_buffer[index + 1];
                    B = frame_buffer[index + 2];
                    if ((R == kResultRed) && (G == kResultGreen) && (B == kResultBlue)) {
                        flag1 = true;
                    }
                    x_tmp++;
                }
                if (flag1) {
                    SetPixelColor(x, y, kResultRed, kResultGreen, kResultBlue);
                }
            }
            x++;
        }
    }
}

void ClearFrameBuffer() {
    if (frame_buffer != nullptr) {
        delete[] frame_buffer;
    }
    int frame_buffer_size = window_width * window_height * 3;
    frame_buffer = new GLfloat[frame_buffer_size];
    for (int i = 0; i < frame_buffer_size; i += 3) {
        frame_buffer[i] = kBackgroundRed;
        frame_buffer[i + 1] = kBackgroundGreen;
        frame_buffer[i + 2] = kBackgroundBlue;
    }
}

void DrawLines(Polygon polygon, GLfloat R, GLfloat G, GLfloat B) {
    size_t num = polygon.points.size();
    if (num > 0) {
        SetPixelColor(polygon.points[0].x, polygon.points[0].y, R, G, B);
    }
    if (num > 1) {
        for (size_t i = 0; i < num - 1; i++) {
            DrawLine(polygon.points[i].x, polygon.points[i].y, polygon.points[i + 1].x, polygon.points[i + 1].y, R, G, B);
        }
    }
    if (num > 2) {
        DrawLine(polygon.points[num - 1].x, polygon.points[num - 1].y, polygon.points[0].x, polygon.points[0].y, R, G, B);                
    }
}

void DrawLine(GLint x1, GLint y1, GLint x2, GLint y2, GLfloat R, GLfloat G, GLfloat B) {
    glPixelZoom(1, 1);

    GLint x = x1;
    GLint y = y1;
    GLint dx = std::abs(x2 - x1);
    GLint dy = std::abs(y2 - y1);
    GLint sign_x = Sign(x2 - x1);
    GLint sign_y = Sign(y2 - y1);
    GLint e = 2 * dy - dx;

    if (dy == 0) {
        for (GLint i = x1; std::abs(x2 - i) > 0; i += sign_x) {
            SetPixelColor(i, y, R, G, B);
        }
        return;
    }
    if (dx == 0) {
        for (GLint i = y1; std::abs(y2 - i) > 0; i += sign_y) {
            SetPixelColor(x, i, R, G, B);
        }
        return;
    }

    bool ch = false;
    if (dy >= dx) {
        std::swap(dx, dy);
        ch = true;
    }
    
    int i = 0;
    while(true) {
        SetPixelColor(x, y, R, G, B);
        if (e < dx) {
            if (ch) {
                y += sign_y;
            } else {
                x += sign_x;
            }
            e += 2 * dy;
        } else {
            if (ch) {
                x += sign_x;
            } else {
                y += sign_y;
            }
            e -= 2 * dx;
        }
        i++;
        if (i > dx + dy) {
            SetPixelColor(x, y, R, G, B);
            break;
        }
    } 
}

template <typename T> int Sign(T val) {
    return (T(0) < val) - (val < T(0));
}

void SetPixelColor(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
    int index = (window_height - y) * 3 * window_width + 3 * x;
    frame_buffer[index] = r;
    frame_buffer[index + 1] = g;
    frame_buffer[index + 2] = b;
}

int GetPixelIndex(int x, int y) {
    return (window_height - y) * 3 * window_width + 3 * x;
}
