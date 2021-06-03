#pragma once
#include <fstream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

const int MAX_FRAME_DRAWS = 2; // this number should be less than or equal to the number of swapchain images
const int MAX_OBJECTS = 8;

const std::vector<const char*> deviceExtensionsNeeded = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME  //"VK_KHR_swapchain"
};

// Transformation Matrices
struct UboViewProjection {
	glm::mat4 projectsion;
	glm::mat4 view;
};

// Separate the model from the MVP as we want to update this particular one for every object
struct Model {
	glm::mat4 model;
};

// Push Constants
struct PushConstBlock {
	glm::vec3 pushConstData;
};

//vertes data representation
struct Vertex {
	glm::vec3 pos;		// vertex position
	glm::vec3 col;		// vertex color
};

// Indices (locations) of Queue Families (if they exist at all)
struct QueueFamilyIndices {
	int graphicsFamily = -1;			// Location of Graphics Queue Family
	int presentationFamily = -1;		

	// Check if queue families are valid
	bool isValid()
	{
		return graphicsFamily >= 0 && presentationFamily >= 0;
	}
};

struct SwapChainInfo {									//[note]: Swapchain holds the info of surface because vk doesn't interact with surface, we access surface through swapchain, so swapchain contains info of surface
	VkSurfaceCapabilitiesKHR surfaceCapabilities;		//surface property, (image size)
	std::vector<VkSurfaceFormatKHR> formats;			// Surface image formats, e.g. RGBA and size of each colour
	std::vector<VkPresentModeKHR> presentationModes;	// How images should be presented to screen
};

struct SwapChainImage {
	VkImage image;				//very similar to physical device, get image from raw image data
	VkImageView imageView;		//create imageview by setup interpretation of image
};

static std::vector<char> readFile(const std::string& filename) {
	// open stream from given file
	// std::ios::binary tells stream to read file as binary
	// std::ios::ate tells stream to start reading from end of file
	// spir-v is raw binary data and set the pointer to the end of file (to get the size)
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	//check successfully open
	if (!file.is_open()) {
		throw std::runtime_error("Faile to open file: " + filename);
	}

	// get current read position and resize file buffer
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> fileBuffer(fileSize);

	// move read position to the start of file
	file.seekg(0);

	// read the file data into the buffer 
	file.read(fileBuffer.data(), fileSize);

	// close stream
	file.close();

	return fileBuffer;
}

static float getDeltaTime() {
	static float deltaTime = 0.0f;
	static float lastTime = 0.0f;
	float now = glfwGetTime();
	deltaTime = now - lastTime;
	lastTime = now;
	return deltaTime;
}

static uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t allowedTypes, 
	VkMemoryPropertyFlags properties)
{
	// Get properties of physical device memory
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((allowedTypes & (1 << i))														// Index of memory type must match corresponding bit in allowedTypes
			&& (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)	// Desired property bit flags are part of memory type's property flags
		{
			// This memory type is valid, so return its index
			return i;
		}
	}
}

static void createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize bufferSize, 
	VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags bufferProperties, VkBuffer* outBuffer, 
	VkDeviceMemory* outBufferMemory)
{	// 1) Create buffer 2) allocate memory 3) and bind buffer & memory

	// CREATE VERTEX BUFFER
	// Information to create a buffer (doesn't include assigning memory)
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = bufferSize;								// Size of buffer (size of 1 vertex * number of vertices)
	bufferInfo.usage = bufferUsage;								// Multiple types of buffer possible
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;			// Similar to Swap Chain images, can share vertex buffers

	VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, outBuffer);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Vertex Buffer!");
	}

	// GET BUFFER MEMORY REQUIREMENTS
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, *outBuffer, &memRequirements);

	// ALLOCATE MEMORY TO BUFFER
	VkMemoryAllocateInfo memoryAllocInfo = {};
	memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocInfo.allocationSize = memRequirements.size;
	memoryAllocInfo.memoryTypeIndex = findMemoryTypeIndex(physicalDevice, memRequirements.memoryTypeBits,		// Index of memory type on Physical Device that has required bit flags
		bufferProperties);																						// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT	: CPU can interact with memory
																												// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT	: Allows placement of data straight into buffer after mapping (otherwise would have to specify manually)
																												// Allocate memory to VkDeviceMemory
	result = vkAllocateMemory(device, &memoryAllocInfo, nullptr, outBufferMemory);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate Vertex Buffer Memory!");
	}

	// Allocate memory to given vertex buffer
	vkBindBufferMemory(device, *outBuffer, *outBufferMemory, 0);
}

// A one time copy command, copy data from staging CPU visible buffer to GPU dedicated buffer; 1) allocate command buffer, 2) recorde command buffer, 3) submit command buffer to queue
static void copyBuffer(VkDevice device, VkQueue transferQueue, VkCommandPool transferCommandPool,
	VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize)
{
	// Command buffer to hold transfer commands
	VkCommandBuffer transferCommandBuffer;

	// Command Buffer details
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = transferCommandPool;
	allocInfo.commandBufferCount = 1;

	// Allocate command buffer from pool
	vkAllocateCommandBuffers(device, &allocInfo, &transferCommandBuffer);

	// Information to begin the command buffer record
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;	// only using the command buffer once, so set up for one time submit

	// Begin recording transfer commands
	vkBeginCommandBuffer(transferCommandBuffer, &beginInfo);

		// Region of data to copy from and to
		VkBufferCopy bufferCopyRegion = {};
		bufferCopyRegion.srcOffset = 0;			// Point to start copy from
		bufferCopyRegion.dstOffset = 0;			// Point to copy to
		bufferCopyRegion.size = bufferSize;		// How much to copy

		// Command to copy src buffer to dst buffer
		vkCmdCopyBuffer(transferCommandBuffer, srcBuffer, dstBuffer, 1, &bufferCopyRegion);

	// End commands
	vkEndCommandBuffer(transferCommandBuffer);

	// Queue submission information
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &transferCommandBuffer;

	// Submit transfer command to transfer queue and wait until it finishes
	vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(transferQueue);								// QueueWaitIdle() indicates CPU to wait until GPU has finished executing all the commands submitted to that paticular queue; [caveat] coding this way is assuming that there's not alot of meshes loading to the GPU
																// The reason to put vkQueueWaitIdle() here is that it could take some time for GPU to execute the command submitted to the queue, if we free the commandBuffer before GPU handle all the command in the queue, there will be a problem
	// Free temporary command buffer back to pool
	vkFreeCommandBuffers(device, transferCommandPool, 1, &transferCommandBuffer);
}