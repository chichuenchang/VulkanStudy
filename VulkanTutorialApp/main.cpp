#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>
#include <iostream>

#include "VulkanRenderer.h"

GLFWwindow* window;
VulkanRenderer vulkanRenderer;

const std::vector<const char*> validationLayers = { "VK_LAYER_CHRONOS_validation" };

#ifdef NDEBUG // use this marcro to check if is in debug mode
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

void initWindow(std::string wName = "Test Window", const int width = 800, const int height = 450) {

	//init glfw
	glfwInit();

	//set glfw to not work with opengl
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, wName.c_str(), nullptr, nullptr);
}

int main() {
	//create window
	initWindow("Test WIndow", 800, 450);


	//create vulkan renderer instance
	if (vulkanRenderer.init(window) == EXIT_FAILURE)
	{
		return EXIT_FAILURE;
	}


	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();
	}
	
	//free memory
	vulkanRenderer.cleanup();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}