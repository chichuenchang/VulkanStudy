#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "Utility.h"

class Mesh
{
public:
	Mesh();
	Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, VkQueue transferQueue, 
		VkCommandPool transferCommandPool, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices,
		int inTextureIndex); // constructor to create buffer
	void destroyBuffers();

	int getVertexCount(); //get the number of vertex and pass to vkCmdDraw()
	VkBuffer getVertexBuffer();
	int getIndexCount();
	VkBuffer getIndexBuffer();
	Model getModel();
	PushConstBlock getPushConstData();
	int getTextureIndex();

	void setModel(glm::mat4 inModel);
	void setPushConstData(glm::vec3 inPushConst);

	~Mesh();

private:
	int vertexCount;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	int indexCount;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	VkPhysicalDevice physicalDevice;
	VkDevice device;

	Model model;
	PushConstBlock pushConstData;
	int textureIndex;

	void createVertexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, 
		std::vector<Vertex>* vertices);
	void createIndexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool,
		std::vector<uint32_t>* indices);
};

