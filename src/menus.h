#ifndef STERLING_MENUS_H
#define STERLING_MENUS_H

#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "scene.h"
#include "object.h"

/// <summary>
/// Provides wrapper functions for ImGui, integrating them into the Sterling system
/// </summary>
namespace menus
{
	/// <summary>
	/// Setup ImGui
	/// </summary>
	/// <param name="window">The GLFW window to set up ImGui for</param>
	void setup(GLFWwindow* window);

	/// <summary>
	/// Shutdown ImGui
	/// </summary>
	void shutdown();

	/// <summary>
	/// Refresh ImGui
	/// </summary>
	void refresh();

	/// <summary>
	/// Render the ImGui windows to the screen
	/// </summary>
	void render();

	/// <summary>
	/// Create the scene tree window and populate it
	/// </summary>
	/// <param name="scene">The scene to show the contents of</param>
	void scene_tree(Scene* scene);

	/// <summary>
	/// Add an object and its children to the scene tree
	/// </summary>
	/// <param name="object">The object to add</param>
	void object_tree(Object* object, unsigned int* ID);
	extern Object* selectedObject;
	extern Camera* selectedCamera;
	extern Light* selectedLight;
	extern AmbientLight* selectedAmbientLight;
	extern PointLight* selectedPointLight;
	extern Spotlight* selectedSpotlight;
	extern DirectionalLight* selectedDirectionalLight;

	/// <summary>
	/// Display a window for the properties of the selected object
	/// </summary>
	void properties();

	/// <summary>
	/// Show the settings for the rendering
	/// </summary>
	void settings();
	extern bool wireframe;
}

#endif