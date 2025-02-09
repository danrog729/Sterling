#include <iostream>
#include <string>

#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"

#include "main.h"
#include "shaders.h"
#include "maths.h"
#include "scene.h"

Camera* camera;

int main()
{
#ifdef _DEBUG
	// wait for user input
	std::cout << "Type anything and press enter to start (For connection to RenderDoc): ";
	std::string enter;
	std::cin >> enter;
#endif

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

	// set up the shader
	Shader* shader = new Shader("shaders/vertex.vert", "shaders/fragment.frag");

	// Set up the camera
	camera = new Camera(maths::vec3f(0.0f, 0.0f, 3.5f), maths::unit_quaternion(1.0f, 0.0f, 0.0f, 0.0f), maths::PI / 3.0f, 0.1f, 100.0f, 800.0f / 600.0f);

	Axes* axes = new Axes(maths::vec3f(0.0f, 0.0f, 3.5f), maths::unit_quaternion(1.0f, 0.0f, 0.0f, 0.0f), maths::vec3f(1.0f, 1.0f, 1.0f));
	Camera* dummyCam = new Camera(maths::vec3f(0.0f, 0.0f, -3.5f), maths::unit_quaternion(1.0f, 0.0f, 0.0f, 0.0f), maths::PI / 3.0f, 0.1f, 100.0f, 800.0f / 600.0f);
	Cube* cube = new Cube(maths::vec3f(0.0f, 0.0f, 0.0f), maths::unit_quaternion(1.0f, 0.0f, 0.0f, 0.0f), maths::vec3f(1.0f, 1.0f, 1.0f));
	Plane* plane = new Plane(maths::vec3f(0.0f, 5.0f, 0.0f), maths::unit_quaternion(0.866025403784f, 0.333333333333f, 0.333333333333f, 0.333333333333f), maths::vec3f(1.0f, 1.0f, 1.0f));

	// Render loop
	double previousTime = 0;
	while (!glfwWindowShouldClose(window))
	{
		// Calculate delta time
		double currentTime = glfwGetTime();
		float deltaTime = (float)(currentTime - previousTime);

		// process inputs
		sterling_process_inputs(window, deltaTime);

		// clear screen
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// calculate the transformation matrix
		maths::mat4f worldToScreenMatrix = camera->orthographic_matrix() * camera->perspective_matrix() * maths::mat4f::stretch_z(-1.0f) * camera->cameraspace_matrix();

		// render cube
		shader->use();

		shader->setMat4f("worldToScreen", worldToScreenMatrix * axes->local_to_world());
		axes->render();

		shader->setMat4f("worldToScreen", worldToScreenMatrix * dummyCam->local_to_world());
		dummyCam->render();

		shader->setMat4f("worldToScreen", worldToScreenMatrix * cube->local_to_world());
		cube->render();

		shader->setMat4f("worldToScreen", worldToScreenMatrix * plane->local_to_world());
		plane->render();

		// Swap buffers
		glfwSwapBuffers(window);
		// Poll events
		glfwPollEvents();

		previousTime = currentTime;
	}

	glfwTerminate();
	return 0;
}

static void sterling_glfw_error_callback(int id, const char* description)
{
	throw "ERROR::GLFW::" + std::to_string(id) + "::" + description;
}

static inline int sterling_initialise_glfw(int majorVersion, int minorVersion, int profile)
{
	if (!glfwInit())
	{
		throw "ERROR::GLFW_INITIALISATION_FAILED";
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
		throw "ERROR::GLFW::WINDOW_CREATION_FAILED";
	}
	glfwMakeContextCurrent(window);
	*target = window;
	return 0;
}

static inline int sterling_initialise_glad()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw "ERROR::GLAD_INITIALISATION_FAILED";
	}
	return 0;
}

static void sterling_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	camera->aspectRatio = ((float)width / height);
}

static void sterling_process_inputs(GLFWwindow* window, float deltaTime)
{
	// camera movement
	float cameraSpeed = 3.0f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera->position = camera->position + camera->front() * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera->position = camera->position + camera->back() * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera->position = camera->position + camera->left() * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera->position = camera->position + camera->right() * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		camera->position = camera->position + camera->up() * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		camera->position = camera->position + camera->down() * cameraSpeed * deltaTime;
	}

	// camera rotation
	float cameraSensitivity = 0.60f;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		camera->rotation = (maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->up()), cameraSensitivity * deltaTime) * camera->rotation).normalise();
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		camera->rotation = (maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->down()), cameraSensitivity * deltaTime) * camera->rotation).normalise();
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		camera->rotation = (maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->right()), cameraSensitivity * deltaTime) * camera->rotation).normalise();
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		camera->rotation = (maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->left()), cameraSensitivity * deltaTime) * camera->rotation).normalise();
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera->rotation = (maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->back()), cameraSensitivity * deltaTime) * camera->rotation).normalise();
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera->rotation = (maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->front()), cameraSensitivity * deltaTime) * camera->rotation).normalise();
	}
}