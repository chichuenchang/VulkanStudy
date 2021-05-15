#pragma once

const std::vector<const char*> deviceExtensionsNeeded = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME  //"VK_KHR_swapchain"
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