#include "ImportMesh.h"




ImportMesh::ImportMesh()
{
}

ImportMesh::ImportMesh(std::vector<Mesh> newMeshList, glm::mat4 inModelMat)
{
	meshList = newMeshList;
	model.model = inModelMat;
}

size_t ImportMesh::getMeshCount()
{
	return meshList.size();
}

Mesh* ImportMesh::getMesh(size_t index)
{
	if (index >= meshList.size())
	{
		throw std::runtime_error("Attempted to access invalid Mesh index!");
	}

	return &meshList[index];
}

Model ImportMesh::getModel()
{
	return model;
}

void ImportMesh::setModel(glm::mat4 newModel)
{
	model.model = newModel;
}

void ImportMesh::destroyImportMesh()
{
	for (auto& mesh : meshList)
	{
		mesh.destroyBuffers();
	}
}

// This returns the vector of fileName of DIFFUSE (albedo) texture for every material, duely noted that not all material has DIFFUSE, if there's no diffuse, the fileName will be ""
std::vector<std::string> ImportMesh::LoadMaterials(const aiScene* scene)
{
	// 1 texture to 1 material
	std::vector<std::string> textureList(scene->mNumMaterials);

	// Go through each material and copy its texture file name (if it exists)
	for (size_t i = 0; i < scene->mNumMaterials; i++)
	{
		// Get the material
		aiMaterial* material = scene->mMaterials[i];

		// Initialise the texture to empty string (will be replaced if texture exists)
		textureList[i] = "";

		// Check for a Diffuse Texture (standard detail texture)
		if (material->GetTextureCount(aiTextureType_DIFFUSE))
		{
			// GetTexture() will get the path of the texture defined in the .mtl file and save it in the asString
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				// get rid of the directory in the string, only leaves the fileName
				int idx = std::string(path.data).rfind("\\");
				std::string fileName = std::string(path.data).substr(idx + 1);

				textureList[i] = fileName;		// If there exist diffuse texture, change fileName; if there's not, do nothing, and the fileName remains as ""
			}
		}
	}

	return textureList;		
}

// In Assimp, Scene has the root nodes and meshList, Nodes has all the meshes index in aiScene and other nodes, and meshes has all the vertex/index data
// 1) recursively go into each node and load all the vertex data; 2) extract all the vertex data from different node, put them on the same level and push into a vector
std::vector<Mesh> ImportMesh::LoadNode(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, 
	VkQueue transferQueue, VkCommandPool transferCommandPool, aiNode* node, const aiScene* scene, 
	std::vector<int> materialToSamplerDescriptorSetId)
{
	std::vector<Mesh> meshList;

	// Go through each mesh at this node and create it, then add it to our meshList
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		meshList.push_back(
			LoadMesh(newPhysicalDevice, newDevice, transferQueue, transferCommandPool, 
				scene->mMeshes[node->mMeshes[i]], scene, materialToSamplerDescriptorSetId) // Access the actual aiMesh data in this way makes sence, since node doen't hold aiMesh, it only holds the index of the aiMesh in the aiMesh list in aiScene. 
		);	
	}

	// Go through each node attached to this node and load it, then append their meshes to this node's mesh list
	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		std::vector<Mesh> newList = LoadNode(newPhysicalDevice, newDevice, transferQueue, 
			transferCommandPool, node->mChildren[i], scene, materialToSamplerDescriptorSetId);
		meshList.insert(meshList.end(), newList.begin(), newList.end());
	}

	return meshList;
}

// aiMesh has all the vertex/index data
// 1) Joint the data held by aiMesh to our own vertex struct; 2) Use the vertices data to create mesh defined in Mesh.h
Mesh ImportMesh::LoadMesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, VkQueue transferQueue, 
	VkCommandPool transferCommandPool, aiMesh* mesh, const aiScene* scene, std::vector<int> materialToSamplerDescriptorSetId)
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	// Resize vertex list to hold all vertices for mesh
	vertices.resize(mesh->mNumVertices);

	// Go through each vertex and copy it across to our vertices struct
	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		// Set position
		vertices[i].pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

		// Set tex coords (if they exist)
		if (mesh->mTextureCoords[0])
		{
			vertices[i].uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
		}
		else
		{
			vertices[i].uv = { 0.0f, 0.0f };
		}

		// Set default color for imported mesh (just use white for now)
		vertices[i].col = { 0.8f, 0.8f, 0.8f };
	}

	// Iterate over indices through faces and copy across
	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		// Get a face
		aiFace face = mesh->mFaces[i];

		// Go through face's indices and add to list
		for (size_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Create new mesh with details and return it
	Mesh newMesh = Mesh(newPhysicalDevice, newDevice, transferQueue, transferCommandPool, 
		&vertices, &indices, materialToSamplerDescriptorSetId[mesh->mMaterialIndex]);

	return newMesh;
}


ImportMesh::~ImportMesh()
{
}
