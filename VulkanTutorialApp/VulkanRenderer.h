#pragma once

//#define VK_USE_PLATFORM_WIN32_KHR //def this to create surface for win32 system
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include <set>
#include <algorithm>
#include <array>

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
	QueueFamilyIndices queueFamilyIndices;
	VkQueue graphicsQueue;
	VkQueue presentationQueue;
	VkSurfaceKHR surface;		//A CHRONOS extension
	VkSwapchainKHR swapchain;
	std::vector<SwapChainImage> swapChainImages;	//swap chain holds multiple images

	// - Pipeline
	VkPipeline graphicsPipeline;
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;

	// - utility
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

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
	void createSwapChain();
	void createRenderPass();
	void createGraphicsPipeline();

	// - Get Functions
	void getPhysicalDevice();

	// - Support Functions
	// -- Checker Functions
	bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions);		// to check if the extensions are supported by vk, this includes validation layer ext
	bool checkPhysicalDeviceSuitable(VkPhysicalDevice device);									// check if the available physical device is suitable
	bool checkPhysicalDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& deviceExtensionsNeeded);

	// -- Get Family Queue indices
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);	//queue families are obtained from physical device, and the assign to logical device
	SwapChainInfo getSwapChainInfo(VkPhysicalDevice device);		//swap chain infos are obtained from physical device

	// -- choose best setting for swapchain
	VkSurfaceFormatKHR chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
	VkPresentModeKHR chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& modes);
	VkExtent2D chooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

	// -- create 
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	VkShaderModule createShaderModule(const std::vector<char>& code);
};

