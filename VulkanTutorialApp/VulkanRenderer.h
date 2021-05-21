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
	void draw();	//draw call
	void cleanup();

	~VulkanRenderer();

private:
	GLFWwindow* window;

	int currentFrame = 0; // keep track of the loop of frame, increment with each frame drawn, when it reaches 2, start from 0 again

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

	// [important]: commandBuffers[0] must correspond to swapChainFramebuffers[0], and then swapChainImages[0], index must be the same
	std::vector<SwapChainImage> swapChainImages;	//swap chain holds multiple images
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers;
	
	// - Pipeline
	VkPipeline graphicsPipeline;
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;

	// - Pools
	VkCommandPool graphicsCommandPool; //this pool is only used for graphics queue

	// - utility
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	// - synchronization
	std::vector<VkSemaphore> semaphoreImageAvailable;		//signal for image available
	std::vector<VkSemaphore> semaphoreFinishRender;			//signal for image done drawing
	std::vector<VkFence> drawFences;

	// - validation layers
	ValidationLayers validationLayers;

	// - setup debug messenger
	// -- create debug messenger ext
	// -- destroy debug messenger ext
	VkDebugUtilsMessengerEXT debugMessenger;

	// - Functions
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
	void createFramebuffer();
	void createCommandPool();
	void allocateCommandBuffers();
	void createSynchronization();

	// - Record commandBuffer
	void recordCommands();

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

