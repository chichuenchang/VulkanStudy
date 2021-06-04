#pragma once

//#define VK_USE_PLATFORM_WIN32_KHR //def this to create surface for win32 system
#define GLFW_INCLUDE_VULKAN // Tell GLFW to help include vulkan
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>
#include <vector>
#include <set>
#include <algorithm>
#include <array>

// A library to load in textures
#include <stb_image.h>

#include "Mesh.h"
#include "Utility.h"
#include "ValidationLayers.h"

class VulkanRenderer
{
public:
	VulkanRenderer();
	~VulkanRenderer();

	int init(GLFWwindow* newWindow);
	void cleanup();
	void draw();	//draw call

	// Get func
	VkExtent2D getSwapChainExtent();

	// Set Func
	void updateModel(int modelId, glm::mat4 ModelInput, glm::vec3 pushConstIn);
	void setViewProjection(const UboViewProjection& inVP);
	void setMeshList(std::vector<Mesh>& meshList);
	// - Set mesh data
	void setMeshVertexData(const std::vector<std::vector<Vertex>>& inMeshVertices);
	void setMeshIndicesData(const std::vector<std::vector<uint32_t>>& inMeshIndices);
	void setViewProjectionMat(const glm::mat4& viewMat, const glm::mat4& projectionMat);
	void addTextureFileName(const std::string& fileName);


private:
	GLFWwindow* window;

	int currentFrame = 0; // keep track of the loop of frame, increment with each frame drawn, when it reaches 2, start from 0 again

	// Scene Objects
	std::vector<std::vector<Vertex>> meshVertexData;
	std::vector<std::vector<uint32_t>> meshIndicesData;
	std::vector<Mesh> meshList;
	std::vector<std::string> textureFileNameList;
	// Transformation Matrices					// [note]: the reason to setup dynamic uniform buffer is because the number of descriptor sets provided by the physical device is limited. 
	UboViewProjection uboViewProjection;		// Also, for each obj drawn we want projection and view are the same but model can change		

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
	std::vector<SwapChainImage> swapChainImages;		// swap chain holds multiple images
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers;

	// Depth Buffer
	VkImage depthBufferImage;
	VkDeviceMemory depthBufferImageMemory;
	VkImageView depthBufferImageView;
	VkFormat depthBufferImageFormat;
	
	// - Descriptor Sets
	VkDescriptorSetLayout descriptorSetLayout;			// set the binding
	VkPushConstantRange pushConstantRange;				// This describes the size of the data passed to push constant
	VkDescriptorPool descriptorPool;					// to hold data of Descriptor Sets
	std::vector<VkDescriptorSet> descriptorSets;		// 1 descriptor set for one uniform buffer
	std::vector<VkBuffer> vpUniformBuffer;				// 1 uniformBuffer for each swapchain image
	std::vector<VkDeviceMemory> vpUniformBufferMemory;	
	std::vector<VkBuffer> mUniformBufferDynamic;
	std::vector<VkDeviceMemory> mUniformBufferMemory;
	// -- Dynamic Uniform Buffer
	VkDeviceSize minUniformBufferOffset;
	size_t modelUniformAlignment;
	Model* modelTransferSpace;

	// - Assets
	std::vector<VkImage> textureImages;
	std::vector<VkDeviceMemory> textureImageMemory;

	// - Pipeline
	VkPipeline graphicsPipeline;
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;

	// - Pools
	VkCommandPool graphicsCommandPool;			//this pool is only used for graphics queue

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
	void createDescriptorSetLayout();
	void createPushConstantRange();
	void createGraphicsPipeline();
	void createDepthBufferImage();
	void createFramebuffer();
	void createCommandPool();
	void allocateCommandBuffers();
	void createSynchronization();
		void createTestMesh();
	void createUniformBuffers();
	void createDescriptorPool();
	void allocateDescriptorSets();

	// - Record commandBuffer
	void recordCommands(uint32_t swapchainImageIndex);

	// - Update Uniform Buffer
	void updateUniformBuffers(uint32_t nextSwapChainImageIndex);

	// - Get Functions
	void getPhysicalDevice();

	// - Allocate
	void allocateDynamicBufferTransferSpace();

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
	VkFormat chooseSupportedFormat(const std::vector<VkFormat>& inFormats, VkImageTiling tiling, 
		VkFormatFeatureFlags featureFlags);

	// -- create 
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	VkShaderModule createShaderModule(const std::vector<char>& code);
	VkImage createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
		VkImageUsageFlags useFlags, VkMemoryPropertyFlags propertyFlags, VkDeviceMemory* outImageMemory);
	int createTexture(std::string fileName);

	// -- Loader Function
	stbi_uc* loadTextureFile(std::string fileName, int* outWidth, int* outHeight, VkDeviceSize* outImageSize);

};

