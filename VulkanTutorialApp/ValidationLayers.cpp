#include "ValidationLayers.h"

bool ValidationLayers::checkValidationLayerSupport()
{
	uint32_t layerCount; 
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> layersAvailable(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layersAvailable.data());
	
	int foundCount = 0;
	for (const char* layerNameRequired : validationLayersRequired) {

		for (const auto& layerAvailable : layersAvailable) {
			if (strcmp(layerNameRequired, layerAvailable.layerName) == 0) {
				
				foundCount++;
				if (foundCount == validationLayersRequired.size()) {
					return true;
				}
			}
		}
	}
	return false;
}

VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayers::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
	VkDebugUtilsMessageTypeFlagsEXT messageType, 
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		//message is important enough to show
	}

	return VK_FALSE;
}

