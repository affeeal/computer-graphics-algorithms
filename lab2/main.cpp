#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>

#define MATRIX_SIZE 16

using std::sqrt;
using std::asin;
using std::sin;
using std::cos;

GLdouble f = 0.5;

GLdouble beta = 0;
GLdouble alpha = 0;

GLboolean fill_mode = true;

void keyCallback(GLFWwindow*, int, int, int, int);
void drawCube(GLdouble size);

GLdouble* newTranslation(GLdouble, GLdouble, GLdouble);
GLdouble* newAxonometric(GLdouble, GLdouble);

int main()
{
	if (!glfwInit())
		return -1;
	
	GLFWwindow* window = glfwCreateWindow(640, 640, "lab2", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLdouble* translation = newTranslation(0.7, 0.7, 0);

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		if (fill_mode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		

		// additional cube
		GLdouble theta = asin(0.5 / sqrt(2));
		GLdouble phi = asin(0.5 / sqrt(2 - 0.5 * 0.5));
		GLdouble* isometric = newAxonometric(theta, phi);
		glLoadIdentity();
		glMultMatrixd(translation);
		glMultMatrixd(isometric);
		drawCube(0.3);
		delete[] isometric;

		// main cube
		theta = asin(f / sqrt(2));
		phi = asin(f / sqrt(2 - f * f));
		isometric = newAxonometric(theta + alpha, phi + beta);
		glLoadIdentity();
		glMultMatrixd(isometric);
		drawCube(0.7);
		delete[] isometric;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete[] translation;
	
	glfwDestroyWindow(window);
	glfwTerminate();
	
	return 0;
}

void keyCallback(GLFWwindow* window, int key, int, int action, int)
{
	if (action == GLFW_PRESS)
		switch (key) {
		case GLFW_KEY_DOWN:
			alpha += 0.05;
			break;
		case GLFW_KEY_UP:
			alpha -= 0.05;
			break;
		case GLFW_KEY_RIGHT:
			beta += 0.05;
			break;
		case GLFW_KEY_LEFT:
			beta -= 0.05;
			break;
		case GLFW_KEY_KP_ADD:
			f += 0.01;
			if (f >= 1) f = -1;
			break;
		case GLFW_KEY_KP_SUBTRACT:
			f -= 0.01;
			if (f <= -1) f = 1;
			break;
		case GLFW_KEY_SPACE:
			fill_mode = !fill_mode;
			break;
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		default:
			std::cout << "no action for pressed key" << std::endl;
		}
	else if (action == GLFW_REPEAT)
		switch (key) {
		case GLFW_KEY_DOWN:
			alpha += 0.05;
			break;
		case GLFW_KEY_UP:
			alpha -= 0.05;
			break;
		case GLFW_KEY_RIGHT:
			beta += 0.05;
			break;
		case GLFW_KEY_LEFT:
			beta -= 0.05;
			break;
		case GLFW_KEY_KP_ADD:
			f += 0.01;
			if (f >= 1) f = -1;
			break;
		case GLFW_KEY_KP_SUBTRACT:
			f -= 0.01;
			if (f <= -1) f = 1;
			break;
		default:
			std::cout << "no action for repeated key" << std::endl;
		}
}

void drawCube(GLdouble size)
{
	glBegin(GL_QUADS);

	glColor3d(1, 0, 0);
	glVertex3d( -size / 2, -size / 2, -size / 2);
	glVertex3d( -size / 2,  size / 2, -size / 2);
	glVertex3d( -size / 2,  size / 2,  size / 2);
	glVertex3d( -size / 2, -size / 2,  size / 2);

	glColor3d(0, 1, 0);
	glVertex3d( size / 2, -size / 2, -size / 2);
	glVertex3d( size / 2, -size / 2,  size / 2);
	glVertex3d( size / 2,  size / 2,  size / 2);
	glVertex3d( size / 2,  size / 2, -size / 2);
	
	glColor3d(0, 0, 1);
	glVertex3d( -size / 2, -size / 2, -size / 2);
	glVertex3d( -size / 2, -size / 2,  size / 2);
	glVertex3d(  size / 2, -size / 2,  size / 2);
	glVertex3d(  size / 2, -size / 2, -size / 2);
	
	glColor3d(1, 0, 1);
	glVertex3d( -size / 2, size / 2, -size / 2);
	glVertex3d( -size / 2, size / 2,  size / 2);
	glVertex3d(  size / 2, size / 2,  size / 2);
	glVertex3d(  size / 2, size / 2, -size / 2);
	
	glColor3d(1, 1, 0);
	glVertex3d( -size / 2, -size / 2, -size / 2);
	glVertex3d(  size / 2, -size / 2, -size / 2);
	glVertex3d(  size / 2,  size / 2, -size / 2);
	glVertex3d( -size / 2,  size / 2, -size / 2);
	
	glColor3d(0, 1, 1);
	glVertex3d( -size / 2, -size / 2,  size / 2);
	glVertex3d(  size / 2, -size / 2,  size / 2);
	glVertex3d(  size / 2,  size / 2,  size / 2);
	glVertex3d( -size / 2,  size / 2,  size / 2);
	
	glEnd();
}

GLdouble* newTranslation(GLdouble x, GLdouble y, GLdouble z)
{
	return new GLdouble[MATRIX_SIZE]
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1
	};
}

GLdouble* newAxonometric(GLdouble theta, GLdouble phi)
{
	return new GLdouble[MATRIX_SIZE]
	{
		cos(phi),  sin(phi) * sin(theta),  sin(phi) * cos(theta), 0, 
		0,         cos(theta),            -sin(theta),            0,
		sin(phi), -cos(phi) * sin(theta), -cos(phi) * cos(theta), 0,
		0,         0,                      0,                     1
	};
}
