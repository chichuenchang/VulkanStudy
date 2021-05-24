#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, std::vector<Vertex>* vertices)
{
	vertexCount = vertices->size();
	physicalDevice = newPhysicalDevice;
	device = newDevice;
	createVertexBuffer(vertices);
}

int Mesh::getVertexCount()
{
	return vertexCount;
}

VkBuffer Mesh::getVertexBuffer()
{
	return vertexBuffer;
}

void Mesh::destroyVertexBuffer()
{
	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);
}

std::vector<Vertex> Mesh::testMeshData()
{
	return std::vector<Vertex>{
		{ {0.4, -0.4, 0.0}			,		{1.0f, 0.0f, 0.0f}	},
		{ {0.4, 0.4, 0.0}			,		{0.0f, 1.0f, 0.0f}	},
		{ {-0.4, 0.4, 0.0}			,		{0.0f, 0.0f, 1.0f}	},
															  
		{ { -0.4, 0.4, 0.0 }		,		{0.0f, 0.0f, 1.0f}	},
		{ { -0.4, -0.4, 0.0 }		,		{1.0f, 1.0f, 0.0f}	},
		{ { 0.4, -0.4, 0.0 }		,		{1.0f, 0.0f, 0.0f}	}
	};
}

Mesh::~Mesh()
{
}

VkBuffer Mesh::createVertexBuffer(std::vector<Vertex>* vertices)
{
	// CREATE VERTEX BUFFER
	// Information to create a buffer (doesn't include assigning memory), this buffer describes the data but does not hold data
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(Vertex) * vertices->size();		// Size of buffer (size of 1 vertex * number of vertices)
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;		// Multiple types of buffer, we want Vertex Buffer
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;			// Similar to Swap Chain images, can share vertex buffers

	VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Vertex Buffer!");
	}

	// GET BUFFER MEMORY REQUIREMENTS
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);

	// ALLOCATE MEMORY TO BUFFER
	VkMemoryAllocateInfo memoryAllocInfo = {};
	memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocInfo.allocationSize = memRequirements.size;
	memoryAllocInfo.memoryTypeIndex = findMemoryTypeIndex(memRequirements.memoryTypeBits,		// Index of memory type on Physical Device that has required bit flags
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);			// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT	: CPU can interact with memory
																								// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT	: Place data straight into buffer after mapping (otherwise would have to specify manually)
	// Allocate memory to VkDeviceMemory
	result = vkAllocateMemory(device, &memoryAllocInfo, nullptr, &vertexBufferMemory);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate Vertex Buffer Memory!");
	}

	// bind memory and buffer
	vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

	// MAP MEMORY TO VERTEX BUFFER
	void* data;																	// 1. Create pointer to a point in normal memory
	vkMapMemory(device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);		// 2. "Map" the vertex buffer memory to that point
	memcpy(data, vertices->data(), (size_t)bufferInfo.size);					// 3. Copy memory from vertices vector to the point
	vkUnmapMemory(device, vertexBufferMemory);									// 4. Unmap the vertex buffer memory
}

uint32_t Mesh::findMemoryTypeIndex(uint32_t allowedTypes, VkMemoryPropertyFlags properties){// mem type we want from physical device, this property flags indicates accessibility from cpu/ gpu
	// Get memory properties of physical device memory
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
	// Iterate over the memory property of physical device and return the index that match the required Memory Type and the property needed
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{		// [note]: the allowedTypes is expressed in a bit wise int not a regular binary, use '& (1 << i)' to find out which bit is '1'
		if ((allowedTypes & (1 << i))														// Index of memory type must match corresponding bit in allowedTypes
			&& (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)	// Desired property bit flags are part of memory type's property flags
		{
			// This memory type is valid, so return its index
			return i;
		}
	}
}
