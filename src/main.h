#ifndef STERLING_MAIN_H
#define STERLING_MAIN_H

/// <summary>
/// Entry point for the program
/// </summary>
/// <returns>0 if exitted successfully</returns>
int main();

/// <summary>
/// Allows GLFW to print error messages to the console
/// </summary>
/// <param name="id">The ID of the error</param>
/// <param name="description">The description of the error</param>
static void sterling_glfw_error_callback(int id, const char* description);

/// <summary>
/// Initialises GLFW with a given version and profile
/// </summary>
/// <param name="majorVersion">The major version</param>
/// <param name="minorVersion">The minor version</param>
/// <param name="profile">The profile</param>
static inline int sterling_initialise_glfw(int majorVersion, int minorVersion, int profile);

/// <summary>
/// Creates a GLFW window and makes it the current context
/// </summary>
/// <param name="target">The pointer to set to the address of the window</param>
/// <param name="width">The width of the window</param>
/// <param name="height">The height of the window</param>
/// <param name="title">The title of the window</param>
/// <param name="monitor">The monitor for the window to open on. Defaults to primary monitor if NULL</param>
/// <param name="share">Another window to share a context with. Defaults to no other window if NULL</param>
/// <returns>0 if window successfully created</returns>
static int sterling_create_glfw_window(GLFWwindow** target, int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);

/// <summary>
/// Initialises GLAD
/// </summary>
/// <returns>0 if successful</returns>
static inline int sterling_initialise_glad();

/// <summary>
/// Callback for changing the size of the window. Sets the OpenGL Viewport to be the new size.
/// </summary>
/// <param name="window">Pointer to the GLFW window</param>
/// <param name="width">The new width</param>
/// <param name="height">The new height</param>
static void sterling_framebuffer_size_callback(GLFWwindow* window, int width, int height);

/// <summary>
/// Handles the logic for key presses
/// </summary>
/// <param name="window">The window that the key press happened on</param>
static void sterling_process_inputs(GLFWwindow* window, float deltaTime);

#endif