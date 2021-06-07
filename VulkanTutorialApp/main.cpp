#define STB_IMAGE_IMPLEMENTATION	// Need this define to activate stb library
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // opengl the depth value (-1.0, 1.0), in vulkan it's (0.0, 1.0)

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

	glm::mat4 model1(1.0f);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -20.0f, -30.5f));
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	model1 = translate * rotate * scale;

	vulkanRenderer.updateModel(0, model1);
	//vulkanRenderer.updateModel(1, model2, glm::vec3(1.0f));
}

// Custom Data
void createTestMesh() {
	// Vertex Data
	//std::vector<Vertex> meshVertices1 = {
	//		{ { -0.4,	 0.5,	0.0 },		{ 0.0f, 0.0f, 1.0f },	{0.0f, 1.0f} },		// 0
	//		{ { -0.4,	-0.5,	0.0 },		{ 1.0f, 1.0f, 0.0f },	{0.0f, 0.0f} },		// 1
	//		{ {  0.4,	-0.5,	0.0 },		{ 1.0f, 0.0f, 1.0f },	{1.0f, 0.0f} },     // 2
	//		{ {  0.4,	 0.5,	0.0 },		{ 0.0f, 1.0f, 0.0f },	{1.0f, 1.0f} },		// 3
	//};
	//std::vector<Vertex> meshVertices2 = {
	//		{ { -0.25,	 0.3,	0.0 },		{ 0.0f, 0.0f, 1.0f },	{0.0f, 1.0f} },		// 0
	//		{ { -0.25,	-0.3,	0.0 },		{ 1.0f, 0.0f, 1.0f },	{0.0f, 0.0f} },		// 1
	//		{ {  0.25,	-0.3,	0.0 },		{ 0.0f, 1.0f, 1.0f },	{1.0f, 0.0f} },		// 2
	//		{ {  0.25,	 0.3,	0.0 },		{ 0.0f, 0.0f, 1.0f },	{1.0f, 1.0f} },		// 3
	//};
	//std::vector<std::vector<Vertex>> meshVerticesList;
	//meshVerticesList.push_back(meshVertices1), meshVerticesList.push_back(meshVertices2);
	//vulkanRenderer.setMeshVertexData(meshVerticesList);
	//
	//// Indices
	//std::vector<uint32_t> indices = {
	//	0, 1, 2,
	//	2, 3, 0
	//};
	//std::vector<std::vector<uint32_t>> meshIndicesList;
	//meshIndicesList.push_back(indices), meshIndicesList.push_back(indices);
	//vulkanRenderer.setMeshIndicesData(meshIndicesList);

	// View Projection matrices
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glm::mat4 projectionMat = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	projectionMat[1][1] *= -1;
	glm::mat4 viewMat = glm::lookAt(glm::vec3(0.0f, 10.0f, 15.0), glm::vec3(0.0f, 0.0f, -4.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	vulkanRenderer.setViewProjectionMat(viewMat, projectionMat);

	// Model Matrix and Init Import Mesh
	glm::mat4 model1(1.0f);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -100.5f));
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f))
		*glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model1 = translate * rotate * scale;
	//vulkanRenderer.createImportMesh("uh60.obj", model1);

	//scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
	//translate = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, -2.0f, -40.5f));
	//rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//model1 = translate * rotate * scale;
	//vulkanRenderer.createImportMesh("Seahawk.obj", model1);

	//scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
	//translate = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, -2.0f, -40.5f));
	//rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//model1 = translate * rotate * scale;
	vulkanRenderer.createImportMesh("Old House 2 3D Models.obj", model1);

}

void init() {

	//create window
	initWindow("Test WIndow", 1600, 900);

	//create vulkan renderer instance
	if (vulkanRenderer.init(window) == EXIT_FAILURE)
	{
		//return EXIT_FAILURE;
		exit(999);
	}
	createTestMesh();
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