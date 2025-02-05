#include <iostream>

#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"

#include "main.h"

int main()
{
	// Set up GLFW error handling and initialise GLFW
	glfwSetErrorCallback(sterling_glfw_error_callback);
	if (sterling_initialise_glfw(4, 6, GLFW_OPENGL_CORE_PROFILE) != 0)
	{
		return 1;
	}

	// Create a GLFW window
	GLFWwindow* window;
	if (sterling_create_glfw_window(&window, 800, 600, "Sterling", NULL, NULL) != 0)
	{
		return 1;
	}

	// Initialise GLAD
	if (sterling_initialise_glad() != 0)
	{
		return -1;
	}

	// Set up the OpenGL viewport
	glViewport(0, 0, 800, 600);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// Swap buffers
		glfwSwapBuffers(window);
		// Poll events
		glfwPollEvents();
	}
}

static void sterling_glfw_error_callback(int id, const char* description)
{
	std::cerr << "ERROR::GLFW::" << id << "::" << description << "\n";
}

static inline int sterling_initialise_glfw(int majorVersion, int minorVersion, int profile)
{
	if (!glfwInit())
	{
		std::cerr << "ERROR::GLFW_INITIALISATION_FAILED";
		return 1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
	glfwWindowHint(GLFW_OPENGL_PROFILE, profile);
	return 0;
}

static int sterling_create_glfw_window(GLFWwindow** target, int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
	GLFWwindow* window = glfwCreateWindow(width, height, title, monitor, share);
	if (window == NULL)
	{
		std::cerr << "ERROR::GLFW::WINDOW_CREATION_FAILED" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	*target = window;
	return 0;
}

static inline int sterling_initialise_glad()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "ERROR::GLAD_INITIALISATION_FAILED" << std::endl;
		return -1;
	}
	return 0;
}