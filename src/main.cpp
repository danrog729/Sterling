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

	// Create the model
	float vertices[] = {
		-1.0f, -1.0f, -1.0f, // // //
		-1.0f, -1.0f,  1.0f, // //    //
		-1.0f,  1.0f, -1.0f, //    // //
		-1.0f,  1.0f,  1.0f,          //
		 1.0f, -1.0f, -1.0f,    // //
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// set up the shader
	Shader* shader = new Shader("shaders/vertex.vert", "shaders/fragment.frag");

	// Set up the camera
	camera = new Camera(maths::vec3f(0.0f, 0.0f, 3.5f), maths::unit_quaternion(1.0f, 0.0f, 0.0f, 0.0f), maths::PI / 3.0f, 0.1f, 100.0f, 800.0f / 600.0f);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// process inputs
		sterling_process_inputs(window);

		float magnitude = sqrtf(camera->rotation.r * camera->rotation.r + camera->rotation.i * camera->rotation.i + camera->rotation.j * camera->rotation.j + camera->rotation.k * camera->rotation.k);
		std::cout << magnitude << "\n";

		// clear screen
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// calculate the transformation matrix
		maths::mat4f worldToScreenMatrix = camera->orthographic_matrix() * camera->perspective_matrix() * maths::mat4f::stretch_z(-1.0f) * camera->cameraspace_matrix();

		// render cube
		shader->use();
		shader->setMat4f("worldToScreen", worldToScreenMatrix);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, sizeof(faces) / sizeof(int), GL_UNSIGNED_INT, 0);

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

static void sterling_process_inputs(GLFWwindow* window)
{
	// camera movement
	float cameraSpeed = 0.05f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera->position = camera->position + camera->front() * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera->position = camera->position + camera->back() * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera->position = camera->position + camera->left() * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera->position = camera->position + camera->right() * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		camera->position = camera->position + camera->up() * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		camera->position = camera->position + camera->down() * cameraSpeed;
	}

	// camera rotation
	float cameraSensitivity = 0.01f;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		camera->rotation = (maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->up()), cameraSensitivity) * camera->rotation).normalise();
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		camera->rotation = (maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->down()), cameraSensitivity) * camera->rotation).normalise();
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		camera->rotation = (maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->right()), cameraSensitivity) * camera->rotation).normalise();
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		camera->rotation = (maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->left()), cameraSensitivity) * camera->rotation).normalise();
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera->rotation = (maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->back()), cameraSensitivity) * camera->rotation).normalise();
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera->rotation = (maths::unit_quaternion::from_axis_angle(maths::vec3f(camera->front()), cameraSensitivity) * camera->rotation).normalise();
	}
}