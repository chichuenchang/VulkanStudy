#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <cstring>

class ValidationLayers
{
public:											  //not "VK_LAYER_CHRONOS_validation" 
	const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" }; 
#ifdef NDEBUG // use this marcro to check if is in debug mode
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	bool checkValidationLayerSupport();

private:



};

