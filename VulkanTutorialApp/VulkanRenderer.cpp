#include "VulkanRenderer.h"



VulkanRenderer::VulkanRenderer()
{
}

int VulkanRenderer::init(GLFWwindow* newWindow)
{
	window = newWindow;

	try {
		createInstance();
		setupDebugMessenger();
		createSuface();				// create surface 
		getPhysicalDevice();		
		createLogicalDevice();		// this will call getQueueFamily(), need instantce, physical device, and surface before we can get & check support of queuefamily 
		createSwapChain();
	}
	catch (const std::runtime_error& e) {
		printf("ERROR: %s\n", e.what());
		return EXIT_FAILURE;
	}
	return 0;
}

void VulkanRenderer::cleanup()
{	//whenever vkCreate#() is called, has to call vkDestroy#()
	for (auto image : swapChainImages) {
		vkDestroyImageView(mainDevice.logicalDevice, image.imageView, nullptr);
	}
	vkDestroySwapchainKHR(mainDevice.logicalDevice, swapchain, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyDevice(mainDevice.logicalDevice, nullptr);
	if (validationLayers.enableValidationLayers) {
		DestoryDebugUtilsMessengerEXT(instance, debugMessenger, nullptr); //destroy messenger ext first, then instance
	}
	vkDestroyInstance(instance, nullptr);
}

VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::createInstance()
{
	//validation layers
	if (validationLayers.enableValidationLayers && !validationLayers.checkValidationLayerSupport()) {
		throw std::runtime_error("Validation Layers requested, but not available!");
	}
	
	// Information about the application itself
	// Most data here doesn't affect the program and is for developer convenience
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Tutorial App";					// Custom name of the application
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);		// Custom version of the application
	appInfo.pEngineName = "No Engine";							// Custom engine name
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);			// Custom engine version
	appInfo.apiVersion = VK_API_VERSION_1_0;					// The Vulkan Version

	// Creation information for a VkInstance (Vulkan Instance)
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	//createInfo.pNext: is used when create info with an extension 
	//createInfo.flags: some flags to mark in vk

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	//if validationLayer is enabled
	if (validationLayers.enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.validationLayersRequired.size());
		createInfo.ppEnabledLayerNames = validationLayers.validationLayersRequired.data();
		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = static_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugCreateInfo);
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	// Create list to hold instance extensions
	std::vector<const char*> instanceExtensions = getRequiredExtensions();		//this extension list include all the glfw extension + validatation layer extension when 
	//[note]: instanceExtensions is holding ext to interact with surface, but instance will not work with surface, it's device's job. So, need to check if device support surface
	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());	
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();

	// TODO: Set up Validation Layers that Instance will use
	//createInfo.enabledLayerCount = 0;
	//createInfo.ppEnabledLayerNames = nullptr;

	// Create instance
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance); //2nd argument is the customize allocator for memory management

	if (result != VK_SUCCESS){
		throw std::runtime_error("Failed to create a Vulkan Instance!");
	}
}

void VulkanRenderer::createSuface()
{
	// Create Surface (creates a surface create info struct, runs the create surface function, returns result)
	VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
	//VkWin32SurfaceCreateInfoKHR createInfo; // orinally this is the way to create surface, but glfw has built-in function helps to do the same thing

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a surface!");
	}

}

void VulkanRenderer::createSwapChain()
{
	// Get Swap Chain details so we can pick best settings
	SwapChainInfo swapChainDetails = getSwapChainInfo(mainDevice.physicalDevice);

	// Find optimal surface values for our swap chain
	VkSurfaceFormatKHR surfaceFormat = chooseBestSurfaceFormat(swapChainDetails.formats);
	VkPresentModeKHR presentMode = chooseBestPresentationMode(swapChainDetails.presentationModes);
	VkExtent2D extent = chooseSwapChainExtent(swapChainDetails.surfaceCapabilities);

	// How many images are in the swap chain? Get 1 more than the minimum to allow triple buffering
	// image count in swap chain depends on the number provided by surface, so get the imageCount from surface settings, and then set it to swap chain
	uint32_t imageCount = swapChainDetails.surfaceCapabilities.minImageCount + 1;

	// If imageCount higher than max, then clamp down to max
	// If maxImageCount = 0, then it's limitless
	if (swapChainDetails.surfaceCapabilities.maxImageCount > 0
		&& swapChainDetails.surfaceCapabilities.maxImageCount < imageCount)
	{
		imageCount = swapChainDetails.surfaceCapabilities.maxImageCount;
	}

	// Creation information for swap chain
	VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.surface = surface;														// Swapchain surface
	swapChainCreateInfo.imageFormat = surfaceFormat.format;										// Swapchain format
	swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;								// Swapchain colour space
	swapChainCreateInfo.presentMode = presentMode;												// Swapchain presentation mode
	swapChainCreateInfo.imageExtent = extent;													// Swapchain image extents
	swapChainCreateInfo.minImageCount = imageCount;												// Minimum images in swapchain
	swapChainCreateInfo.imageArrayLayers = 1;													// Number of layers for each image in chain
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;						// What attachment images will be used as
	swapChainCreateInfo.preTransform = swapChainDetails.surfaceCapabilities.currentTransform;	// Transform to perform on swap chain images
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;						// How to handle blending images with external graphics (e.g. other windows)
	swapChainCreateInfo.clipped = VK_TRUE;														// Whether to clip parts of image not in view (e.g. behind another window, off screen, etc)

	// [note]: graphics queue is used to render while presentation queue is used to submit imaged rendered to surface, setup swapchain to let it know which queue does it access
	// Get Queue Family Indices

	// If Graphics and Presentation families are different, then swapchain must let images be shared between families
	if (this->queueFamilyIndices.graphicsFamily != this->queueFamilyIndices.presentationFamily)
	{
		// Queues to share between
		uint32_t queueFamilyIndices[] = {
			(uint32_t)this->queueFamilyIndices.graphicsFamily,
			(uint32_t)this->queueFamilyIndices.presentationFamily
		};

		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;		// Image share handling
		swapChainCreateInfo.queueFamilyIndexCount = 2;							// Number of queues to share images between
		swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;			// Array of queues to share between
	}
	else
	{
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCreateInfo.queueFamilyIndexCount = 0;
		swapChainCreateInfo.pQueueFamilyIndices = nullptr;
	}

	// IF old swap chain been destroyed and this one replaces it, then link old one to quickly hand over responsibilities
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE; //this is used when resizing window

	// Create Swapchain
	VkResult result = vkCreateSwapchainKHR(mainDevice.logicalDevice, &swapChainCreateInfo, nullptr, &swapchain);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Swapchain!");
	}

	//// Store for later reference
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	// Get swap chain images (first count, then values)
	uint32_t swapChainImageCount;
	vkGetSwapchainImagesKHR(mainDevice.logicalDevice, swapchain, &swapChainImageCount, nullptr);
	std::vector<VkImage> images(swapChainImageCount);
	vkGetSwapchainImagesKHR(mainDevice.logicalDevice, swapchain, &swapChainImageCount, images.data());

	for (VkImage image : images)
	{
		// Store image handle
		SwapChainImage swapChainImage = {};
		swapChainImage.image = image;
		swapChainImage.imageView = createImageView(image, swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

		// Add to swapchain image list
		swapChainImages.push_back(swapChainImage);
	}


}

void VulkanRenderer::createLogicalDevice()
{
	// Get the queue family indices for the chosen Physical Device

	// Vector for queue creation information, and set for family indices
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> queueFamilyIndices = { this->queueFamilyIndices.graphicsFamily, this->queueFamilyIndices.presentationFamily };

	// Queues the logical device needs to create and info to do so
	for (int queueFamilyIndex : queueFamilyIndices)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;						// The index of the family to create a queue from
		queueCreateInfo.queueCount = 1;												// Number of queues to create
		float priority = 1.0f;
		queueCreateInfo.pQueuePriorities = &priority;								// Vulkan needs to know how to handle multiple queues, so decide priority (1 = highest priority)

		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Information to create logical device (sometimes called "device")
	VkDeviceCreateInfo deviceCreateInfo = {};								//VkDeviceCreateInfo holds a VkDeviceQueueCreateInfo & a VkPhysicalDeviceFeatures
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());			// Number of Queue Create Infos
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();									// List of queue create infos so device can create required queues, ckeck queue compatability by getQueueFamiliy() before assign here
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensionsNeeded.size());	// Number of enabled logical device extensions, check compatability in getPhysicalDevice() before assign here
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensionsNeeded.data();						// List of enabled logical device extensions

	// Physical Device Features the Logical Device will be using
	VkPhysicalDeviceFeatures deviceFeatures = {};

	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;			// Physical Device features Logical Device will use

	// Create the logical device for the given physical device						//allocator
	VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Logical Device!");
	}

	// Queues are created at the same time as the device...
	// So we want handle to queues
	// From given logical device, of given Queue Family, of given Queue Index (0 since only one queue), place reference in given VkQueue
	vkGetDeviceQueue(mainDevice.logicalDevice, this->queueFamilyIndices.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(mainDevice.logicalDevice, this->queueFamilyIndices.presentationFamily, 0, &presentationQueue);
}

void VulkanRenderer::getPhysicalDevice()
{
	// Enumerate Physical devices the vkInstance can access
	// - [note]: just like create instance, the function is called twice, first time to get the size of the list, second time to populate the data
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	// If no devices available, then none support Vulkan!
	if (deviceCount == 0)
	{
		throw std::runtime_error("Can't find GPUs that support Vulkan Instance!");
	}

	// Get list of Physical Devices
	std::vector<VkPhysicalDevice> deviceList(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());

	for (const auto& device : deviceList)
	{
		if (checkPhysicalDeviceSuitable(device))
		{
			mainDevice.physicalDevice = device; 
			break;
		}
	}
}

bool VulkanRenderer::checkInstanceExtensionSupport(std::vector<const char*>* checkExtensionsNeeded)
{
	// Need to get number of extensions to create array of correct size to hold extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	// Create a list of VkExtensionProperties using count
	std::vector<VkExtensionProperties> instanceExtensionsProvided(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instanceExtensionsProvided.data());
	//[note]: same funcion is called twice, first time to get the size, init an array with that size and populate the array by calling the same function second time

	// Check if given extensions are in list of available extensions
	int checkExtensionCount = 0;
	for (const auto& checkExtensionNeeded : *checkExtensionsNeeded)
	{
		//bool hasExtension = false;
		for (const auto& extensionProvided : instanceExtensionsProvided)
		{
			if (strcmp(checkExtensionNeeded, extensionProvided.extensionName) == 0) // should add == 0
			{
				checkExtensionCount++;
				continue;
			}
		}
	}

	if (checkExtensionCount < checkExtensionsNeeded->size()) {
		return false;
	}

	return true;
}

bool VulkanRenderer::checkPhysicalDeviceExtensionSupport(VkPhysicalDevice device, 
	const std::vector<const char*> &deviceExtensionsNeeded)
{
	// Get device extension count
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	// If no extensions found, return failure
	if (extensionCount == 0)
	{
		return false;
	}

	// Populate list of extensions
	std::vector<VkExtensionProperties> extensionsProvided(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensionsProvided.data());

	// Check for extension
	int extensionFoundCount = 0;
	for (const auto& deviceExtensionNeeded : deviceExtensionsNeeded)
	{
		for (const auto& extensionProvided : extensionsProvided)
		{
			if (strcmp(deviceExtensionNeeded, extensionProvided.extensionName) == 0)
			{
				extensionFoundCount++;
				continue;
			}
		}
	}
	if (extensionCount < deviceExtensionsNeeded.size())
	{
		return false;
	}
	return true;
}

bool VulkanRenderer::checkPhysicalDeviceSuitable(VkPhysicalDevice device)
{
	/*
	// Information about the device itself (ID, name, type, vendor, etc)
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	// Information about what the device can do (geo shader, tess shader, wide lines, etc)
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	*/

	queueFamilyIndices = getQueueFamilies(device);			// get queue family info from physical device and store
	//QueueFamilyIndices indices = getQueueFamilies(device);
	bool extensionsSupported = checkPhysicalDeviceExtensionSupport(device, deviceExtensionsNeeded);

	bool swapChainValid = false;
	SwapChainInfo swapChainInfo = getSwapChainInfo(device);
	swapChainValid = !swapChainInfo.presentationModes.empty() && !swapChainInfo.formats.empty();

	return queueFamilyIndices.isValid() && extensionsSupported && swapChainValid;
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	// Get all Queue Family Property info for the given device
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyListProvided(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyListProvided.data());

	// Go through each queue family and check if it has at least 1 of the required types of queue
	int i = 0;
	for (const VkQueueFamilyProperties& queueFamily : queueFamilyListProvided)
	{
		// First check if queue family has at least 1 queue in that family (could have no queues)
		// Queue can be multiple types defined through bitfield. Need to bitwise AND with VK_QUEUE_*_BIT to check if has required type
		// in the case here we bitwise with VK_QUEUE_GRAPHICS_BIT which means we check graphics queuefamily validity
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;		// If queue family is valid, then get index
		}
		// Check if Queue Family supports presentation
		VkBool32 presentationSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupport);

		// Check if queue is presentation type (can be both graphics and presentation)
		if (queueFamily.queueCount > 0 && presentationSupport)
		{
			indices.presentationFamily = i;
		}
		// Check if queue family indices are in a valid state, stop searching if so
		if (indices.isValid())
		{
			break;
		}
		i++;
	}
	return indices;
}

SwapChainInfo VulkanRenderer::getSwapChainInfo(VkPhysicalDevice device)
{
	SwapChainInfo swapChainInfo;

	// -- CAPABILITIES --
	// Get the surface capabilities for the given surface on the given physical device
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapChainInfo.surfaceCapabilities);

	// -- FORMATS --
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	// If formats returned, get list of formats
	if (formatCount != 0)
	{
		swapChainInfo.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, swapChainInfo.formats.data());
	}

	// -- PRESENTATION MODES --
	uint32_t presentationCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, nullptr);

	// If presentation modes returned, get list of presentation modes
	if (presentationCount != 0)
	{
		swapChainInfo.presentationModes.resize(presentationCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, swapChainInfo.presentationModes.data());
	}

	return swapChainInfo;
}

// format		:	VK_FORMAT_R8G8B8A8_UNORM (VK_FORMAT_B8G8R8A8_UNORM as backup)
// colorSpace	:	VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
VkSurfaceFormatKHR VulkanRenderer::chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
{
	// If only 1 format available and is undefined, then this means ALL formats are available (no restrictions)
	if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	// If restricted, search for optimal format
	for (const auto& format : formats)
	{
		if ((format.format == VK_FORMAT_R8G8B8A8_UNORM || format.format == VK_FORMAT_B8G8R8A8_UNORM)
			&& format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return format;
		}
	}

	// If can't find optimal format, then just return first format
	return formats[0];
}

VkPresentModeKHR VulkanRenderer::chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& modes)
{
	// Look for Mailbox presentation mode
	for (const auto& presentationMode : modes)
	{
		if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return presentationMode;
		}
	}

	// If can't find, use FIFO as Vulkan spec says it must be present
	return VK_PRESENT_MODE_FIFO_KHR; //this is always available by default
}

VkExtent2D VulkanRenderer::chooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{// VkExtent2D is part of VkSurfaceCapabilitiesKHR, this is what we want no the whole struct

	// If current extent is at numeric limits, then extent can vary. Otherwise, it is the size of the window.
	if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return surfaceCapabilities.currentExtent;
	}
	else
	{
		// If value can vary, need to set manually

		// Get window size
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		// Create new extent using window size
		VkExtent2D newExtent = {};
		newExtent.width = static_cast<uint32_t>(width);
		newExtent.height = static_cast<uint32_t>(height);

		// Surface also defines max and min, so make sure within boundaries by clamping value
		newExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, newExtent.width));
		newExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, newExtent.height));

		return newExtent;
	}


}

VkImageView VulkanRenderer::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewCreateInfo = {};
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.image = image;											// Image to create view for
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;						// Type of image (1D, 2D, 3D, Cube, etc)
	viewCreateInfo.format = format;											// Format of image data
	viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;			// Allows remapping of rgba components to other rgba values
	viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	// Subresources allow the view to view only a part of an image
	viewCreateInfo.subresourceRange.aspectMask = aspectFlags;				// Which aspect of image to view (e.g. COLOR_BIT for viewing colour)
	viewCreateInfo.subresourceRange.baseMipLevel = 0;						// Start mipmap level to view from
	viewCreateInfo.subresourceRange.levelCount = 1;							// Number of mipmap levels to view
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;						// Start array level to view from
	viewCreateInfo.subresourceRange.layerCount = 1;							// Number of array levels to view

	// Create image view and return it
	VkImageView imageView;
	VkResult result = vkCreateImageView(mainDevice.logicalDevice, &viewCreateInfo, nullptr, &imageView);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create an Image View!");
	}

	return imageView;
}

void VulkanRenderer::setupDebugMessenger()
{
	if (!validationLayers.enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to setup debug messenger!");
	}

}

void VulkanRenderer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity =
		//VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = validationLayers.debugCallback;
	createInfo.pUserData = nullptr; //optional
}


VkResult VulkanRenderer::CreateDebugUtilsMessengerEXT(VkInstance instance,// create messenger EXT requires a valid instance first
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, VkAllocationCallbacks* pAllocator, 
	VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VulkanRenderer::DestoryDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, 
	const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

std::vector<const char*> VulkanRenderer::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount); // use glfw lib to get the list of extension and count number
	
	//copy all the data allocated at **glfwExtension to the vector
	std::vector<const char*> instanceExtensionsRequired(glfwExtensions, glfwExtensions + glfwExtensionCount);
	
	//push additional extension if validation layer enabled
	if (validationLayers.enableValidationLayers) {
		instanceExtensionsRequired.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	if (!checkInstanceExtensionSupport(&instanceExtensionsRequired)) {
		throw std::runtime_error("VkInstance does not support required extensions!");
	}

	return instanceExtensionsRequired;
}


