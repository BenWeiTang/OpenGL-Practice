#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer PosSSBO
{
	vec3 positions[];
};

void main()
{
	uint index = gl_GlobalInvocationID.x;
	positions[index].x = 1.0;
	positions[index].y = 1.0;
	positions[index].z = 1.0;
}
