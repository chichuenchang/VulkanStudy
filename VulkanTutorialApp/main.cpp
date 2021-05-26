#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>
#include <iostream>

#include "VulkanRenderer.h"

GLFWwindow* window;
VulkanRenderer vulkanRenderer;

void initWindow(std::string wName = "Test Window", const int width = 800, const int height = 450) {

	//init glfw
	glfwInit();

	//set glfw to not work with opengl
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, wName.c_str(), nullptr, nullptr);
}

void update() {

	static float angle = 0.0f;
	angle += 1.0f * getDeltaTime();
	if (angle > 360.0f) angle -= 360.0f;

	glm::mat4 model1(1.0f), model2(1.0f);
	model1 = glm::translate(model1, glm::vec3(-2.0f, 0.0f, -5.0f));
	model1 = glm::rotate(model1, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

	model2 = glm::translate(model2, glm::vec3(2.0f, 0.0f, -5.0f));
	model2 = glm::rotate(model2, glm::radians(-angle * 100), glm::vec3(0.0f, 0.0f, 1.0f));

	vulkanRenderer.updateModel(0, model1);
	vulkanRenderer.updateModel(1, model2);
	//vulkanRenderer.updateModel(glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f)));
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

		update();
		vulkanRenderer.draw();
	}
	
	//free memory
	vulkanRenderer.cleanup();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}