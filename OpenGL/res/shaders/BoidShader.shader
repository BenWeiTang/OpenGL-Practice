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

void main()
{
	gl_Position = u_Projection * u_View * transMatrices[aModelIndex] * aPos;
}

#shader fragment
#version 460 core

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(1.0f);
}