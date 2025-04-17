#include <iostream>
#include <string>

#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"

#include "main.h"
#include "shaders.h"
#include "oldmaterials.h"
#include "maths.h"
#include "oldscene.h"
#include "textures.h"
#include "mesh.h"
#include "scene.h"

Scene* scene;

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

	// v-sync
	glfwSwapInterval(1);

	// enable depth test
	glEnable(GL_DEPTH_TEST);

	// Set up the scene
	scene = new Scene();
	Object* chair = new Object("models/crate.obj", scene);
	scene->add_object(chair);
	chair->transformation.position(maths::vec3f(-0.67091, -0.049841, 0));
	chair->transformation.rotation(maths::unit_quaternion(0.935994, -0.022166, -0.008317, 0.351219));
	
	Object* crate = new Object("models/chair.obj", scene);
	scene->add_object(crate);
	crate->transformation.position(maths::vec3f(0.531813, -0.474596, 0.008897));
	crate->transformation.rotation(maths::unit_quaternion(1, 0, 0, 0));
	crate->transformation.scale(maths::vec3f(1.809, 1.809, 1.809));

	Camera* camera = new Camera(scene);
	camera->transformation.position(maths::vec3f(0, -5, 2.5));
	camera->transformation.rotation(maths::unit_quaternion(sqrtf(2.0f) / 2.0f, sqrtf(2.0f) / 2.0f, 0, 0));
	scene->activeCamera = camera;

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
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render the scene
		scene->render();

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
	scene->activeCamera->aspectRatio((float)width / height);
}

static void sterling_process_inputs(GLFWwindow* window, float deltaTime)
{
	// oldcamera movement
	float cameraSpeed = 3.0f;
	Camera* camera = scene->activeCamera;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera->transformation.position(camera->transformation.position() + camera->transformation.down() * cameraSpeed * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera->transformation.position(camera->transformation.position() + camera->transformation.up() * cameraSpeed * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera->transformation.position(camera->transformation.position() + camera->transformation.left() * cameraSpeed * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera->transformation.position(camera->transformation.position() + camera->transformation.right() * cameraSpeed * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		camera->transformation.position(camera->transformation.position() + camera->transformation.forward() * cameraSpeed * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		camera->transformation.position(camera->transformation.position() + camera->transformation.backward() * cameraSpeed * deltaTime);
	}

	// oldcamera rotation
	float cameraSensitivity = 1.5f;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		camera->transformation.rotation(
			(maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->transformation.forward()), cameraSensitivity * deltaTime) * camera->transformation.rotation()).normalise()
		);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		camera->transformation.rotation(
			(maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->transformation.backward()), cameraSensitivity * deltaTime) * camera->transformation.rotation()).normalise()
		);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		camera->transformation.rotation(
			(maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->transformation.right()), cameraSensitivity * deltaTime) * camera->transformation.rotation()).normalise()
		);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		camera->transformation.rotation(
			(maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->transformation.left()), cameraSensitivity * deltaTime) * camera->transformation.rotation()).normalise()
		);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera->transformation.rotation(
			(maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->transformation.up()), cameraSensitivity * deltaTime) * camera->transformation.rotation()).normalise()
		);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera->transformation.rotation(
			(maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->transformation.down()), cameraSensitivity * deltaTime) * camera->transformation.rotation()).normalise()
		);
	}
}