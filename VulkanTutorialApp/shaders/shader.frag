#version 450

//layout(location = 0) in vec3 fragColour;	// Interpolated colour from vertex (location must match)

// INTPUT
layout (location = 0) in vec3 col_vsOut;

// OUTPUT
layout (location = 0) out vec4 outColour; 	// Final output colour (must also have location

void main() {
	outColour = vec4(col_vsOut, 1.0);
}