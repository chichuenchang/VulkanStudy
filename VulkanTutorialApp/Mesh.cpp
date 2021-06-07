#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, VkQueue transferQueue,
	VkCommandPool transferCommandPool, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices,
	int inTextureIndex)
{
	vertexCount = vertices->size();
	indexCount = indices->size();
	physicalDevice = newPhysicalDevice;
	device = newDevice;
	createVertexBuffer(transferQueue, transferCommandPool, vertices);
	createIndexBuffer(transferQueue, transferCommandPool, indices);

	this->model.model = glm::mat4(1.0f);

	textureIndex = inTextureIndex;
}

int Mesh::getVertexCount()
{
	return vertexCount;
}

VkBuffer Mesh::getVertexBuffer()
{
	return vertexBuffer;
}

int Mesh::getIndexCount()
{
	return indexCount;
}

VkBuffer Mesh::getIndexBuffer()
{
	return indexBuffer;
}

void Mesh::setModel(glm::mat4 inModel)
{
	this->model.model = inModel;
}

void Mesh::setPushConstData(glm::vec3 inPushConst)
{
	this->pushConstData.pushConstData = inPushConst;
}

Model Mesh::getModel()
{
	return this->model;
}

PushConstBlock Mesh::getPushConstData()
{
	return this->pushConstData;
}

int Mesh::getTextureIndex()
{
	return this->textureIndex;
}

void Mesh::destroyBuffers()
{
	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);
	vkDestroyBuffer(device, indexBuffer, nullptr);
	vkFreeMemory(device, indexBufferMemory, nullptr);
}


Mesh::~Mesh()
{
}

void Mesh::createVertexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, 
	std::vector<Vertex>* vertices)
{
	VkDeviceSize bufferSize = sizeof(Vertex) * static_cast<uint64_t>(vertices->size());

	// Create staging buffer with TRANSFER SOURCE BIT which is ready to be copied
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, // VK_BUFFER_USAGE_TRANSFER_SRC_BIT indicate that this buffer is ready to be transferred
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&stagingBuffer, &stagingBufferMemory);

	// Copy date to staging buffer memory
	void* data;																	// 1. Create pointer to a point in normal memory
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);			// 2. "Map" the vertex buffer memory to that point
	memcpy(data, vertices->data(), (size_t)bufferSize);							// 3. Copy memory from vertices vector to the point
	vkUnmapMemory(device, stagingBufferMemory);									// 4. Unmap the vertex buffer memory

	// Create buffer with TRANSFER DESTINATION BIT as the recipient of data copied
	createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,	// the bit or '|' specifies this usage is defined by both of these types
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertexBuffer, &vertexBufferMemory);											// VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT specifies this buffer is only accessible by GPU

	// Copy staging buffer to vertex buffer on GPU
	copyBuffer(device, transferQueue, transferCommandPool, stagingBuffer, vertexBuffer, bufferSize);

	// Clean up staging buffer parts
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void Mesh::createIndexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool,
	std::vector<uint32_t>* indices)
{
	// Get size of buffer needed for indices
	VkDeviceSize bufferSize = sizeof(uint32_t) * static_cast<uint64_t>(indices->size());

	// Temporary buffer to "stage" index data before transferring to GPU
	VkBuffer stagingIndexBuffer;
	VkDeviceMemory stagingIndexBufferMemory;
	createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingIndexBuffer, &stagingIndexBufferMemory);

	// Copy index data to staging memory
	void* data;
	vkMapMemory(device, stagingIndexBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices->data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingIndexBufferMemory);

	// Create buffer for INDEX data on GPU access only area
	createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,	// Note the usage here is INDEX BUFFER
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &indexBuffer, &indexBufferMemory);

	// Copy from staging buffer to GPU access buffer
	copyBuffer(device, transferQueue, transferCommandPool, stagingIndexBuffer, indexBuffer, bufferSize);

	// Destroy + Release Staging Buffer resources
	vkDestroyBuffer(device, stagingIndexBuffer, nullptr);
	vkFreeMemory(device, stagingIndexBufferMemory, nullptr);
}



