#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>

float angle = 45.f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		if (key == GLFW_KEY_RIGHT)
			angle -= 2.5f;
		else if (key == GLFW_KEY_LEFT)
			angle += 2.5f;
		else if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, true);
	}
}

void display (GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	
	glLoadIdentity();
	glRotated(angle, 0.f, 0.f, 1.f);
	
	glBegin(GL_POLYGON);
	glColor3f(1.f, 0.75f, 0.2f);
	glVertex2f(-0.25f, 0.f);
	glVertex2f(-0.5f, -0.5f);
	
	glColor3f(1.f, 0.85f, 0.3f);
	glVertex2f(0.f, -0.25f);
	glVertex2f(0.5f, -0.5f);
	
	glColor3f(1.f, 0.75f, 0.2f);
	glVertex2f(0.25f, 0.f);
	glVertex2f(0.5f, 0.5f);
	
	glColor3f(1.f, 0.65f, 0.1f);
	glVertex2f(0.f, 0.25f);
	glVertex2f(-0.5f, 0.5f);
	glEnd();
	
	glfwSwapBuffers(window);
	glfwPollEvents();
}

int main()
{
	if (!glfwInit())
		return -1;

	GLFWwindow* window = glfwCreateWindow(640, 640, "lab1", NULL, NULL);
	if (window == NULL)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	while (!glfwWindowShouldClose(window))
		display(window);

	glfwDestroyWindow(window);
	glfwTerminate();
	
    return 0;
}
