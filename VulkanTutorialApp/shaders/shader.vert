#version 450 		// Use GLSL 4.5

// INPUT
// attributes
layout (location = 0) in vec3 pos;					// attribute input should match the binding value in the vkVertexInputBindingDescription and location should match in vkVertexInputAttributeDescription
layout (location = 1) in vec3 col;
// uniform											// vulkan cannot pass uniform like gl, it passes the uniform buffer object
layout (binding = 0) uniform UboViewProjection{		// Binding should match VkDescriptorSetLayoutBinding 
	mat4 projection;
	mat4 view;
}uboViewProjection;
layout (binding = 1) uniform UboModel{
	mat4 model;
}uboModel;

// OUTPUT
layout (location = 0) out vec3 col_vsOut;

void main() { //main() could be renamed whatever in vk, since we can specify the function to call in shader, but for a good practice better stick with convention
	gl_Position = uboViewProjection.projection * uboViewProjection.view* uboModel.model * vec4(pos, 1.0);
	col_vsOut = col;
}