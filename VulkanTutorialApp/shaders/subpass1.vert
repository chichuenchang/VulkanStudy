#version 450

// Array for triangle that fills screen
vec2 positions[3] = vec2[](
	vec2(3.0f, -1.0f),
	vec2(-1.0f, -1.0f),
	vec2(-1.0f, 3.0f)
);

void main()
{
	gl_Position = vec4(positions[gl_VertexIndex], 0.0f, 1.0f);
}