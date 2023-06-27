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
layout(std430, binding = 3) buffer NeighborCountSSBO
{
	uint neighborCounts[];
};

uniform float u_DeltaTime;
uniform float u_SeparationFactor;
uniform float u_AlignmentFactor;
uniform float u_CohesionFactor;

uint index = gl_GlobalInvocationID.x; // For some reason, this cannot be const??
const float VIEW_DIST = 5.0;
const float VIEW_DIST_SQUARED = VIEW_DIST * VIEW_DIST;
const float VIEW_ANGLE = 3.1415926f * 0.75f;
const float COSINE_VIEW_ANGLE = cos(VIEW_ANGLE);
const float BOUNDARY = 55.0;
const float SPEED = 10.0;

bool IsValidOther(uint i, uint j);
vec4 Separate();
vec4 Align();
vec4 Cohere();
vec4 AvoidBoundary();
void CountNeighbor();

void main()
{
	// Get data from SSBO's
	vec4 pos = positions[index];
	vec4 vel = velocities[index];
	vec4 acc = accelerations[index];

	// Calculate new values for each attribute
	pos += u_DeltaTime * vel;
	vel += u_DeltaTime * acc;
	vel = SPEED * normalize(vel);
	acc = u_SeparationFactor * Separate() + u_AlignmentFactor * Align() + u_CohesionFactor * Cohere() + 10 * AvoidBoundary();

	// Set new values to SSBO's 
	positions[index] = pos;
	velocities[index] = vel;
	accelerations[index] = acc;

	CountNeighbor();
}

bool IsValidOther(uint self, uint other)
{
	if (self == other) return false;
	vec4 offset = positions[other] - positions[self];
	float squaredDist = dot(offset, offset); // Dot product with itself is the square of its mag
	float cosViewAngle = dot(normalize(velocities[self]), normalize(offset));
	return squaredDist < VIEW_DIST_SQUARED && cosViewAngle > COSINE_VIEW_ANGLE;
}

vec4 Separate()
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

vec4 AvoidBoundary()
{
	vec4 pointAhead = positions[index] + velocities[index];
	vec4 desired = vec4(0);
	vec4 distToBoundary = vec4(BOUNDARY) - abs(positions[index]);
	int count = 0;
	
	if (pointAhead.x > BOUNDARY)
	{
		desired += vec4(-1.0, 0.0, 0.0, 0.0) / pow(distToBoundary.x, 4);
		count++;
	}
	else if (pointAhead.x < -BOUNDARY)
	{
		desired += vec4(1.0, 0.0, 0.0, 0.0) / pow(distToBoundary.x, 4);
		count++;
	}

	if (pointAhead.y > BOUNDARY)
	{
		desired += vec4(0.0, -1.0, 0.0, 0.0) / pow(distToBoundary.y, 4);
		count++;
	}
	else if (pointAhead.y < -BOUNDARY)
	{
		desired += vec4(0.0, 1.0, 0.0, 0.0) / pow(distToBoundary.y, 4);
		count++;
	}

	if (pointAhead.z > BOUNDARY)
	{
		desired += vec4(0.0, 0.0, -1.0, 0.0) / pow(distToBoundary.z, 4);
		count++;
	}
	else if (pointAhead.z < -BOUNDARY)
	{
		desired += vec4(0.0, 0.0, 1.0, 0.0) / pow(distToBoundary.z, 4);
		count++;
	}
	if (count != 0) desired /= count;

	vec4 steer = desired - velocities[index];
	return steer;
}

void CountNeighbor()
{
	uint count = 0;
	for (uint i = 0; i < gl_NumWorkGroups.x * gl_WorkGroupSize.x; i++)
	{
		if (IsValidOther(index, i)) count++;
	}
	neighborCounts[index] = count;
}
