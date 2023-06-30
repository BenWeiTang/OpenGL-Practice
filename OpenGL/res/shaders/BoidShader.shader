#shader vertex
#version 460 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in uint aModelIndex;
layout(std430, binding = 0) buffer PosSSBO
{
	vec4 positions[];
};
layout(std430, binding = 1) buffer VelSSBO
{
	vec4 velocities[];
};
layout(std430, binding = 3) buffer NeighborCountSSBO
{
	uint neighborCounts[];
};

uniform mat4 u_View;
uniform mat4 u_Projection;

out uint v_NeighborCount;

vec4 GetQuaternion(vec3 axis, float angle)
{
	vec4 qr;
	float halfAngle = angle / 2.0;
	qr.x = axis.x * sin(halfAngle);
	qr.y = axis.y * sin(halfAngle);
	qr.z = axis.z * sin(halfAngle);
	qr.w = cos(halfAngle);
	return qr;
}

vec3 RotateVertex(vec3 position, vec3 axis, float angle)
{
	vec4 q = GetQuaternion(axis, angle);
	vec3 v = position.xyz;
	return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

void main()
{
	// Make a rotated version of the current vertex position
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 forward = normalize(velocities[aModelIndex].xyz);
	vec3 axis = cross(up, forward);
	float angle = acos(dot(up, forward));
	vec3 pos = RotateVertex(aPos.xyz, axis, angle);

	// Make a model matrix that scales and translate
	mat4 model = mat4(1.0);
	model[0][0] = 2.0;
	model[1][1] = 2.0;
	model[2][2] = 2.0;
	model[3] = vec4(positions[aModelIndex].xyz, 1.0);

	// position will be the rotated vertex position, scaled then translated
	gl_Position = u_Projection * u_View * model * vec4(pos, 1.0);

	// Passing neighbor count to frag shader
	v_NeighborCount = neighborCounts[aModelIndex];
}

#shader fragment
#version 460 core

layout(location = 0) out vec4 color;

flat in uint v_NeighborCount;

uniform vec4 u_MinColor;
uniform vec4 u_MaxColor;

void main()
{
	float lerpValue = smoothstep(0.0, 50.0, v_NeighborCount);
	color = mix(u_MinColor, u_MaxColor, lerpValue);
}