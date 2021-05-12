#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

#include "Utility.h"
#include "ValidationLayers.h"

class VulkanRenderer
{
public:
	VulkanRenderer();

	int init(GLFWwindow* newWindow);
	void cleanup();

	~VulkanRenderer();

private:
	GLFWwindow* window;

	//// Vulkan Components
	VkInstance instance;
	struct {
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	} mainDevice;
	VkQueue graphicsQueue;


	// Vulkan Functions
	// - Create Functions
	void createInstance();
	void createLogicalDevice();

	// - Get Functions
	void getPhysicalDevice();

	// - Support Functions
	// -- Checker Functions
	bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions); //to check if the extensions are supported by vk
	bool checkDeviceSuitable(VkPhysicalDevice device); // check if the available physical device is suitable

	// -- Get Family Queue indices
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);

	// - validation layers
	ValidationLayers validationLayers;

};

