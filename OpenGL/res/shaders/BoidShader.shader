#shader vertex
#version 460 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in uint aModelIndex;
layout(std430, binding = 3) buffer TransMatrixSSBO
{
	mat4 transMatrices[];
};

uniform mat4 u_View;
uniform mat4 u_Projection;

out uint v_ModelIndex;

void main()
{
	gl_Position = u_Projection * u_View * transMatrices[aModelIndex] * aPos;
	v_ModelIndex = aModelIndex;
}

#shader fragment
#version 460 core

layout(location = 0) out vec4 color;
layout(std430, binding = 4) buffer NeighborCountSSBO
{
	uint neighborCounts[];
};

flat in uint v_ModelIndex;

void main()
{
	float lerpValue = smoothstep(0.0, 50.0, neighborCounts[v_ModelIndex]);
	color = mix(vec4(1.0, 0.0, 0.125, 1.0), vec4(0.039, 0.727, 0.707, 1.0), lerpValue);
}