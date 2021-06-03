#define GLM_FORCE_DEPTH_ZERO_TO_ONE

//#include <GLFW/glfw3.h>
//#include <vulkan/vulkan.h>

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

// Update uniform every frame
void update() {

	static float angle = 0.0f;
	angle += 1.0f * getDeltaTime();
	if (angle > 360.0f) angle -= 360.0f;

	glm::mat4 model1(1.0f), model2(1.0f);
	model1 = glm::translate(model1, glm::vec3( 0.0f, 0.0f, -2.0f));
	model1 = glm::rotate(model1, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

	model2 = glm::translate(model2, glm::vec3( 0.0f, 0.0f, -1.87f));
	model2 = glm::rotate(model2, glm::radians(-angle * 100), glm::vec3(0.0f, 0.0f, 1.0f));

	vulkanRenderer.updateModel(0, model1, glm::vec3(1.0f));
	vulkanRenderer.updateModel(1, model2, glm::vec3(1.0f));
}

// Custom Data
void createTestMesh() {
	// Vertex Data
	std::vector<Vertex> meshVertices1 = {
			{ { -0.4,	 0.4,	0.0 },		{ 1.0f, 0.0f, 0.0f } },		// 0
			{ { -0.4,	-0.4,	0.0 },		{ 1.0f, 0.0f, 0.0f } },		// 1
			{ {  0.4,	-0.4,	0.0 },		{ 1.0f, 0.0f, 0.0f } },     // 2
			{ {  0.4,	 0.4,	0.0 },		{ 1.0f, 0.0f, 0.0f } },		// 3
	};
	std::vector<Vertex> meshVertices2 = {
			{ { -0.25,	 0.6,	0.0 },		{ 0.0f, 0.0f, 1.0f } },		// 0
			{ { -0.25,	-0.6,	0.0 },		{ 0.0f, 0.0f, 1.0f } },		// 1
			{ {  0.25,	-0.6,	0.0 },		{ 0.0f, 0.0f, 1.0f } },		// 2
			{ {  0.25,	 0.6,	0.0 },		{ 0.0f, 0.0f, 1.0f } },		// 3
	};

	std::vector<std::vector<Vertex>> meshVerticesList;
	meshVerticesList.push_back(meshVertices1), meshVerticesList.push_back(meshVertices2);
	vulkanRenderer.setMeshVertexData(meshVerticesList);
	
	// Indices
	std::vector<uint32_t> indices = {
		0, 1, 2,
		2, 3, 0
	};

	std::vector<std::vector<uint32_t>> meshIndicesList;
	meshIndicesList.push_back(indices), meshIndicesList.push_back(indices);
	vulkanRenderer.setMeshIndicesData(meshIndicesList);

	// View Projection matrices
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glm::mat4 projectionMat = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	projectionMat[1][1] *= -1;
	glm::mat4 viewMat = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, -4.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	vulkanRenderer.setViewProjectionMat(viewMat, projectionMat);
}

void init() {

	//create window
	initWindow("Test WIndow", 800, 450);

	createTestMesh();
	//create vulkan renderer instance
	if (vulkanRenderer.init(window) == EXIT_FAILURE)
	{
		//return EXIT_FAILURE;
		exit(999);
	}

}

int main() {

	init();

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