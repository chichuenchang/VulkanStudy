#pragma once

//#define VK_USE_PLATFORM_WIN32_KHR //def this to create surface for win32 system
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include <set>

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
	VkQueue presentationQueue;
	VkSurfaceKHR surface; //A CHRONOS extension


	// - validation layers
	ValidationLayers validationLayers;

	// - setup debug messenger
	// -- create debug messenger ext
	// -- destroy debug messenger ext
	VkDebugUtilsMessengerEXT debugMessenger;
	void setupDebugMessenger();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
		VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestoryDebugUtilsMessengerEXT(VkInstance instantce, VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator);

	// -- get Extension
	std::vector<const char*> getRequiredExtensions();

	// Vulkan Functions
	// - Create Functions
	void createInstance();
	void createLogicalDevice();
	void createSuface();

	// - Get Functions
	void getPhysicalDevice();

	// - Support Functions
	// -- Checker Functions
	bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions);		// to check if the extensions are supported by vk, this includes validation layer ext
	bool checkDeviceSuitable(VkPhysicalDevice device);									// check if the available physical device is suitable
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	// -- Get Family Queue indices
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);

};

