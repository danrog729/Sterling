#include <iostream>

#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"

int main()
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initialise GLFW";
	}

}