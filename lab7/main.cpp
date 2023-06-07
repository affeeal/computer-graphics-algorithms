#include <chrono>
#include <cmath>
#include <cstddef>
#include <iostream>

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <string>

#define DISPLAY_LIST_OPTIMISATION
#define VERTEX_ARRAY_OPTIMISATION

#undef BACK_FACES_CLIPPING
#define DOUBLE_BUFFERING
#define IN_DEPTH_TESTING

const size_t kWidth = 600;
const size_t kHeight = 600;
const char* kTitle = "Light and animation";
const char* kTexture = "lab7/diamond.jpg";

const GLfloat kDistance = 0.5;
const GLfloat kAlphaDelta = 4;
const GLfloat kBetaDelta = 4;
const GLint kEdgesCountDelta = 1;
const GLint kEdgesCountLowerBound = 6;

GLfloat alpha = 0;
GLfloat beta = 0;
GLfloat radius = 0.5f;
GLint edges_count = 6;

// Animation
GLfloat x_velocity = 0.015f;
GLfloat y_velocity = 0.012f;
GLfloat x_0 = 0.0f;
GLfloat y_0 = 0.0f;

// Light
GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat light_duffusion[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

// Texture
GLuint texture_id = 0;

// Optimisation
#ifdef DISPLAY_LIST_OPTIMISATION
const GLsizei kDisplayListsRange = 1;

GLuint display_list = 0;
#endif

const int kCounterBound = 60;
int counter = 0;
std::chrono::high_resolution_clock::time_point previous
    = std::chrono::high_resolution_clock::now();

// Function declarations 
void KeyCallback(GLFWwindow*, int, int, int, int);
void UpdatePosition();
bool SetTexture (const char* name);
void ShowTimeDifference();
void DrawCylinder(GLfloat radius, GLfloat distance,
    int edges_count, GLfloat x_0, GLfloat y_0);
void DrawCylinderVertexArray(GLfloat radius, GLfloat distance,
    int edges_count, GLfloat x_0, GLfloat y_0);

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

  if (!SetTexture(kTexture)) {
    std::cerr << "Failed loading texture" << std::endl;
    return -1;
  }

#ifdef BACK_FACES_CLIPPING
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
#endif

#ifdef DOUBLE_BUFFERING 
  glfwSwapInterval(1);
#endif

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_duffusion);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  
#ifdef DISPLAY_LIST_OPTIMISATION
  display_list = glGenLists(kDisplayListsRange);
  if (!display_list) {
    std::cerr << "Failed creating display list" << std::endl;
    return -1;
  }
#endif

  while (!glfwWindowShouldClose(window)) {
#ifdef IN_DEPTH_TESTING
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
#endif

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glLoadIdentity();
    glRotated(alpha, 1, 0, 0);
    glRotated(beta, 0, 1, 0);

#ifdef DISPLAY_LIST_OPTIMISATION
    glCallList(display_list);
#endif
    
    UpdatePosition();
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id);

#ifdef VERTEX_ARRAY_OPTIMISATION
    DrawCylinderVertexArray(radius, kDistance, edges_count, x_0, y_0);
#else
    DrawCylinder(radius, kDistance, edges_count, x_0, y_0);
#endif

    glfwSwapBuffers(window);
    glfwPollEvents();
    
    if (counter == kCounterBound) {
      ShowTimeDifference();
    }
    
    counter++;
  }

#ifdef DISPLAY_LIST_OPTIMISATION
  glDeleteLists(display_list, 1);
#endif

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

void UpdatePosition() {
  x_0 += x_velocity;
  y_0 += y_velocity;
  
  if (radius - x_0 <= 0.0f || radius - x_0 >= 1.0f) {
    x_velocity *= -1;
  }
  
  if (radius - y_0 <= 0.0f || radius - y_0 >= 1.0f) {
    y_velocity *= -1;
  }
}

void DrawCylinder(GLfloat radius, GLfloat distance,
    int edges_count, GLfloat x_0, GLfloat y_0) {
#ifdef DISPLAY_LIST_OPTIMISATION
  glNewList(display_list, GL_COMPILE);
#endif
  
  glBegin(GL_TRIANGLE_STRIP);

  for (int i = 0; i <= edges_count; i++) {
    float x = radius * cos(i * 2 * M_PI / edges_count) + x_0;
    float y = radius * sin(i * 2 * M_PI / edges_count) + y_0;
    
    glTexCoord2f(x, 0);
    glVertex3f(x, y, -0.5 * distance);
    
    glTexCoord2f(x, 1);
    glVertex3f(x, y, 0.5 * distance);
  }

  glEnd();

#ifdef DISPLAY_LIST_OPTIMISATION
  glEndList();
#endif
}

void DrawCylinderVertexArray(GLfloat radius, GLfloat distance,
    int edges_count, GLfloat x_0, GLfloat y_0) {
  GLfloat* vertices = new GLfloat[(edges_count + 1) * 3 * 2];
  GLfloat* tex_coords = new GLfloat[(edges_count + 1) * 3 * 2];

  for (int i = 0; i <= edges_count; i++) {
    float x = radius * cos(i * 2.0f * M_PI / edges_count) + x_0;
    float y = radius * sin(i * 2.0f * M_PI / edges_count) + y_0;

    int vertices_index = i * 3 * 2;
    // int tex_coords_index = i * 2;

    vertices[vertices_index++] = x;
    vertices[vertices_index++] = y;
    vertices[vertices_index++] = -0.5f * distance;

    vertices[vertices_index++] = x;
    vertices[vertices_index++] = y;
    vertices[vertices_index++] = 0.5f * distance;
  }

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glVertexPointer(3, GL_FLOAT, 0, vertices);
  glTexCoordPointer(2, GL_FLOAT, 0, tex_coords);
  
  glDrawArrays(GL_TRIANGLE_STRIP, 0, (edges_count + 1) * 2);

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  delete[] vertices;
  delete[] tex_coords;
}

bool SetTexture(const char* filename) {
  int x = 0;
  int y = 0;
  int channels = 0;

  unsigned char* texture = stbi_load(filename, &x, &y, &channels, 0);
  if (!texture) {
    return false;
  }

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

  stbi_image_free(texture);

  return true;
}

void ShowTimeDifference() {
  counter = 0;
  auto current = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> delta = current - previous;
  std::cout << delta.count() << std::endl;
  previous = current;
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
      edges_count += kEdgesCountDelta;
      break;
     case GLFW_KEY_KP_SUBTRACT:
      if (edges_count > kEdgesCountLowerBound)
        edges_count -= kEdgesCountDelta;
      break;
     case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GLFW_TRUE);
      break;
     default:
      std::cerr << "No action for pressed key" << std::endl;
    }
  }
}

