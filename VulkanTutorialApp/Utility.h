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