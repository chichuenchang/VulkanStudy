//#define GLFW_INCLUDE_VULKAN
//this macro is defined in the glfw header to help include vulkan
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //apply to vk, because vk scale depth from 0 to 1, whereas opengl use from -1 to 1
#include <glm/glm.hpp>
//#include <glm/mat4x4.hpp>

#include <iostream>
using namespace std;


int main() {

	glfwInit();
									//not using opengl api, no ES (embedded system) api
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(800, 450, "display", nullptr, nullptr);


	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	//cout << "extension count:" << extensionCount << endl;

	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();



	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}