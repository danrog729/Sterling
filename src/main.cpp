#include <iostream>

#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"

#include "main.h"
#include "shaders.h"
#include "maths.h"
#include "scene.h"

Camera* camera;

int main()
{
	// Set up GLFW error handling and initialise GLFW
	glfwSetErrorCallback(sterling_glfw_error_callback);
	if (sterling_initialise_glfw(4, 6, GLFW_OPENGL_CORE_PROFILE) != 0)
	{
		glfwTerminate();
		return 1;
	}

	// Create a GLFW window
	GLFWwindow* window;
	if (sterling_create_glfw_window(&window, 800, 600, "Sterling", NULL, NULL) != 0)
	{
		glfwTerminate();
		return 1;
	}

	// Initialise GLAD
	if (sterling_initialise_glad() != 0)
	{
		glfwTerminate();
		return 1;
	}

	// Set up the OpenGL viewport
	glViewport(0, 0, 800, 600);

	// Set the framebuffer size callback
	glfwSetFramebufferSizeCallback(window, sterling_framebuffer_size_callback);

	// Create the model
	float vertices[] = {
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f
	};
	int faces[] = {
		0,2,1,
		0,1,4,
		0,4,2,
		1,2,3,
		1,3,5,
		1,5,4,
		2,6,3,
		2,4,6,
		3,7,5,
		3,6,7,
		4,5,6,
		5,7,6
	};
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	// set up the shader
	Shader* shader = new Shader("shaders/vertex.vert", "shaders/fragment.frag");

	// Set up the camera
	camera = new Camera(maths::vec3f(0.0f, 0.0f, 3.5f), maths::unit_quaternion(1.0f, 0.0f, 0.0f, 0.0f), maths::PI / 3.0f, 0.1, 10.0f, 800.0f / 600.0f);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// clear screen
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// calculate the transformation matrix
		maths::mat4f worldToScreenMatrix = camera->orthographic_matrix() * camera->perspective_matrix() * maths::mat4f::stretch_z(-1.0f) * camera->cameraspace_matrix();

		// render cube
		shader->use();
		shader->setMat4f("worldToScreen", worldToScreenMatrix);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Swap buffers
		glfwSwapBuffers(window);
		// Poll events
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
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

static void sterling_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	camera->aspectRatio = ((float)width / height);
}