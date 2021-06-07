#version 450

//layout(location = 0) in vec3 fragColour;	// Interpolated colour from vertex (location must match)

// INTPUT
// - Varying
layout (location = 0) in vec3 col_vsOut;
layout (location = 9) in vec3 pushData_vsOut;
layout (location = 1) in vec2 uv_vsOut;
// - Uniform
layout (set = 1, binding = 0) uniform sampler2D textureSampler;


// OUTPUT
layout (location = 0) out vec4 outColour; 	// Final output colour (must also have location

void main() {
	//outColour = vec4(col_vsOut * pushData_vsOut.x, 1.0);
	outColour = texture(textureSampler, uv_vsOut);
}