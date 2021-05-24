#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "Utility.h"

class Mesh
{
public:
	Mesh();
	Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, std::vector<Vertex>* vertices); // constructor to create buffer

	int getVertexCount(); //get the number of vertex and pass to vkCmdDraw()
	VkBuffer getVertexBuffer();

	void destroyVertexBuffer();
	std::vector<Vertex> testMeshData();

	~Mesh();

private:
	int vertexCount;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	VkPhysicalDevice physicalDevice;
	VkDevice device;

	VkBuffer createVertexBuffer(std::vector<Vertex>* vertices);
	uint32_t findMemoryTypeIndex(uint32_t allowedTypes, VkMemoryPropertyFlags properties);
};

