#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "Mesh.h"

class ImportMesh
{
public:
	ImportMesh();
	ImportMesh(std::vector<Mesh> newMeshList, glm::mat4 inModelMat);

	size_t getMeshCount();
	Mesh* getMesh(size_t index);

	Model getModel();
	void setModel(glm::mat4 newModel);

	void destroyImportMesh();

	static std::vector<std::string> LoadMaterials(const aiScene* scene);
	static std::vector<Mesh> LoadNode(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, 
		VkQueue transferQueue, VkCommandPool transferCommandPool,
		aiNode* node, const aiScene* scene, std::vector<int> materialToSamplerDescriptorSetId);
	static Mesh LoadMesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, VkQueue transferQueue, 
		VkCommandPool transferCommandPool,
		aiMesh* mesh, const aiScene* scene, std::vector<int> materialToSamplerDescriptorSetId);

	~ImportMesh();

private:
	std::vector<Mesh> meshList;
	Model model;
	//glm::mat4 model;
};

