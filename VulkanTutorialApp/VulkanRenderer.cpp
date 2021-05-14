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
	}
	catch (const std::runtime_error& e) {
		printf("ERROR: %s\n", e.what());
		return EXIT_FAILURE;
	}
	return 0;
}

void VulkanRenderer::cleanup()
{	//whenever vkCreate#() is called, has to call vkDestroy#()
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
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.validationLayers.data();
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
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;

	// Create instance
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance); //2nd argument is the customize allocator for memory management

	if (result != VK_SUCCESS){
		throw std::runtime_error("Failed to create a Vulkan Instance!");
	}
}

void VulkanRenderer::createLogicalDevice()
{
	//Get the queue family indices for the chosen Physical Device
	QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

	// Vector for queue creation information, and set for family indices
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> queueFamilyIndices = { indices.graphicsFamily, indices.presentationFamily };

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
	//// Queue the logical device needs to create and info to do so (Only 1 for now, will add more later!)
	//VkDeviceQueueCreateInfo queueCreateInfo = {};								//logical device has to be create with queues
	//queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	//queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;					// The index of the family to create a queue from
	//queueCreateInfo.queueCount = 1;												// Number of queues to create
	//float priority = 1.0f;
	//queueCreateInfo.pQueuePriorities = &priority;								// Vulkan needs to know how to handle multiple queues, so decide priority (1 = highest priority)

	// Information to create logical device (sometimes called "device")
	VkDeviceCreateInfo deviceCreateInfo = {};								//VkDeviceCreateInfo holds a VkDeviceQueueCreateInfo & a VkPhysicalDeviceFeatures
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());		// Number of Queue Create Infos
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();								// List of queue create infos so device can create required queues
	deviceCreateInfo.enabledExtensionCount = 0;													// Number of enabled logical device extensions
	deviceCreateInfo.ppEnabledExtensionNames = nullptr;											// List of enabled logical device extensions

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
	vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(mainDevice.logicalDevice, indices.presentationFamily, 0, &presentationQueue);
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
		if (checkDeviceSuitable(device))
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
	for (const auto& checkExtension : *checkExtensionsNeeded)
	{
		bool hasExtension = false;
		for (const auto& extensionProvided : instanceExtensionsProvided)
		{
			if (strcmp(checkExtension, extensionProvided.extensionName) == 0) // should add == 0
			{
				hasExtension = true;
				break;
			}
		}

		if (!hasExtension)
		{
			return false;
		}
	}

	return true;
}

bool VulkanRenderer::checkDeviceExtensionSupport(VkPhysicalDevice device)
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
	for (const auto& deviceExtensionNeeded : deviceExtensionsNeeded)
	{
		bool hasExtension = false;
		for (const auto& extensionProvided : extensionsProvided)
		{
			if (strcmp(deviceExtensionNeeded, extensionProvided.extensionName) == 0)
			{
				hasExtension = true;
				break;
			}
		}

		if (!hasExtension)
		{
			return false;
		}
	}

	return true;
}

bool VulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device)
{
	/*
	// Information about the device itself (ID, name, type, vendor, etc)
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	// Information about what the device can do (geo shader, tess shader, wide lines, etc)
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	*/

	QueueFamilyIndices indices = getQueueFamilies(device);

	return indices.isValid();
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	// Get all Queue Family Property info for the given device
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

	// Go through each queue family and check if it has at least 1 of the required types of queue
	int i = 0;
	for (const VkQueueFamilyProperties& queueFamily : queueFamilyList)
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


