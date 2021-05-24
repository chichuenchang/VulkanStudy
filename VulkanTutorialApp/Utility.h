#pragma once
#include <fstream>
#include <glm/glm.hpp>


const int MAX_FRAME_DRAWS = 2; // this number should be less than or equal to the number of swapchain images

const std::vector<const char*> deviceExtensionsNeeded = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME  //"VK_KHR_swapchain"
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