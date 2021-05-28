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

// Custom data 
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
}

//// Custom Data
void createTestMesh() {
	// Vertex Data
	std::vector<Vertex> meshVertices1 = {
			{ { -0.4, 0.4, 0.0 },{ 1.0f, 0.0f, 0.0f } },	// 0
			{ { -0.4, -0.4, 0.0 },{ 0.0f, 1.0f, 0.0f } },	    // 1
			{ { 0.4, -0.4, 0.0 },{ 0.0f, 0.0f, 1.0f } },    // 2
			{ { 0.4, 0.4, 0.0 },{ 1.0f, 1.0f, 0.0f } },   // 3
	};
	std::vector<Vertex> meshVertices2 = {
			{ { -0.25, 0.6, 0.0 },{ 1.0f, 0.0f, 0.0f } },	// 0
			{ { -0.25, -0.6, 0.0 },{ 0.0f, 1.0f, 0.0f } },	    // 1
			{ { 0.25, -0.6, 0.0 },{ 0.0f, 0.0f, 1.0f } },    // 2
			{ { 0.25, 0.6, 0.0 },{ 1.0f, 1.0f, 0.0f } },   // 3
	};

	std::vector<uint32_t> indices = {
		0, 1, 2,
		2, 3, 0
	};

	glm::mat4 projectionMat = glm::perspective(glm::radians(45.0f), (float)vulkanRenderer.getSwapChainExtent().width / (float)vulkanRenderer.getSwapChainExtent().height,
		0.1f, 100.0f);
	projectionMat[1][1] *= -1;
	glm::mat4 viewMat = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));



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