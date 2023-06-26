#version 460 core

layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer PosSSBO
{
	vec4 positions[];
};
layout(std430, binding = 1) buffer VelSSBO
{
	vec4 velocities[];
};
layout(std430, binding = 2) buffer AccSSBO
{
	vec4 accelerations[];
};
layout(std430, binding = 3) buffer TransMatrixSSBO
{
	mat4 transMatrices[];
};

uniform float u_DeltaTime;
uniform float u_SeperationFactor;
uniform float u_AlignmentFactor;
uniform float u_CohesionFactor;

uint index = gl_GlobalInvocationID.x; // For some reason, this cannot be const??
const float VIEW_DIST = 5.0;
const float INVERSE_VIEW_DIST = 1.0 / VIEW_DIST;
const float VIEW_DIST_SQUARED = VIEW_DIST * VIEW_DIST;
const float VIEW_ANGLE = 3.1415926f * 0.75f;
const float BOUNDARY = 50.0;
const float SPEED = 10.0;

bool IsValidOther(uint i, uint j);
vec4 Seperate();
vec4 Align();
vec4 Cohere();
void Wrap();

void main()
{
	index = gl_GlobalInvocationID.x;
	Wrap();

	// Get data from SSBO's
	vec4 pos = positions[index];
	vec4 vel = velocities[index];
	vec4 acc = accelerations[index];

	// Calculate new values for each attribute
	pos += u_DeltaTime * vel;
	vel += u_DeltaTime * acc;
	vel = SPEED * normalize(vel);
	acc = u_SeperationFactor * Seperate() + u_AlignmentFactor * Align() + u_CohesionFactor * Cohere();

	// Set new values to SSBO's 
	positions[index] = pos;
	velocities[index] = vel;
	accelerations[index] = acc;

	// Update transformation matrix
	/*
	*  | 1 0 0 x |
	*  | 0 1 0 y |
	*  | 0 0 1 z |
	*  | 0 0 0 1 |
	*/
	transMatrices[index][3] = vec4(positions[index].xyz, 1.0);
	transMatrices[index][0][0] = 1.0;
	transMatrices[index][1][1] = 1.0;
	transMatrices[index][2][2] = 1.0;
}

bool IsValidOther(uint self, uint other)
{
	if (self == other) return false;
	vec4 offset = positions[other] - positions[self];
	float squaredDist = dot(offset, offset); // Dot product with itself is the square of its mag
	vec4 forward = normalize(velocities[self]);
	vec4 towardOther = normalize(offset);
	float angle = acos(dot(forward, towardOther));
	return squaredDist < VIEW_DIST_SQUARED && angle < VIEW_ANGLE;
}

vec4 Seperate()
{
	vec4 desired = vec4(0);
	int count = 0;

	for (uint i = 0; i < gl_NumWorkGroups.x * gl_WorkGroupSize.x; i++)
	{
		if (!IsValidOther(index, i)) continue;
		desired += (VIEW_DIST / distance(positions[index], positions[i]) - vec4(1.0, 1.0, 1.0, 0.0)) * normalize(positions[index] - positions[i]);
		count++;
	}
	if (count != 0) desired /= count;

	vec4 steer = desired - velocities[index];
	return steer;
}

vec4 Align()
{
	vec4 forward = vec4(0);
	int count = 0;

	for (uint i = 0; i < gl_NumWorkGroups.x * gl_WorkGroupSize.x; i++)
	{
		if (!IsValidOther(index, i)) continue;
		forward += velocities[i];
		count++;
	}
	if (count != 0) forward /= count;

	vec4 steer = forward - velocities[index];
	return steer;
}

vec4 Cohere()
{
	vec4 center = vec4(0);
	int count = 0;

	for (uint i = 0; i < gl_NumWorkGroups.x * gl_WorkGroupSize.x; i++)
	{
		if (!IsValidOther(index, i)) continue;
		center += positions[i];
		count++;
	}
	if (count != 0) center /= count;

	vec4 desired = center - positions[index];
	vec4 steer = desired - velocities[index];

	return steer;
}

void Wrap()
{
	// X component
	if (positions[index].x > BOUNDARY) positions[index].x = -BOUNDARY;
	else if (positions[index].x < -BOUNDARY) positions[index].x = BOUNDARY;

	// Y component
	if (positions[index].y > BOUNDARY) positions[index].y = -BOUNDARY;
	else if (positions[index].y < -BOUNDARY) positions[index].y = BOUNDARY;

	// Z component
	if (positions[index].z > BOUNDARY) positions[index].z = -BOUNDARY;
	else if (positions[index].z < -BOUNDARY) positions[index].z = BOUNDARY;
}
