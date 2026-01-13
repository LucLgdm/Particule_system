#define PI 3.14159265358979323846f
#define GOLDEN_ANGLE 2.399963229728653f

float hash(uint x) {
    x ^= x >> 16;
    x *= 0x7feb352d;
    x ^= x >> 15;
    x *= 0x846ca68b;
    x ^= x >> 16;
    return (float)x / (float)UINT_MAX;
}

void createSphere(float radius, float4* positions, size_t gid, uint n) {
	// Fibonacci repartition on a sphere
		float u = (float)gid + 0.5f;
		float v = u / (float)n;

		float theta = GOLDEN_ANGLE * gid;
		float z = 1.0f - 2.0f * v;
		float r = sqrt(1.0f - z * z);

		positions[gid].x = radius * r * cos(theta);
		positions[gid].y = radius * r * sin(theta);
		positions[gid].z = radius * z;
		positions[gid].w = 1.0f;
}

void createCube(float4* positions, size_t gid) {
	float baseCube = 3.0;
	float x = (hash(gid * 3u + 0u) * 2.0f - 1.0f) * baseCube / 2.0f;
	float y = (hash(gid * 3u + 1u) * 2.0f - 1.0f) * baseCube / 2.0f ;
	float z = (hash(gid * 3u + 2u) * 2.0f - 1.0f) * baseCube / 2.0f ;
	
	positions[gid] = (float4)(x, y, z, 1.0f);
}

void createPyramid(float4* positions, size_t gid, const uint n) {
	uint layers = 20;
	float baseSize = 1.0;
	float height = 5.0;

	uint particlesPerLayer = n / layers;
	uint layer = gid / particlesPerLayer;
	if (layer >= layers) layer = layers - 1;

	// Taille du carré à cette couche
	float t = 1.0f - (float)layer / (float)(layers - 1);
	float size = baseSize * t;

	// Index local dans la couche
	uint localIdx = gid % particlesPerLayer;
	uint side = (uint)sqrt((float)particlesPerLayer);
	if (side < 1) side = 1;

	uint xId = localIdx % side;
	uint zId = localIdx / side;

	float x = ((float)xId / (float)(side - 1) - 0.5f) * size;
	float z = ((float)zId / (float)(side - 1) - 0.5f) * size;
	float y = ((float)layer / (float)(layers - 1)) * height;

	positions[gid] = (float4)(x, y, z, 1.0f);
}

void initSpeed(float4* positions, float4* velocities, size_t gid) {
	float gravityMass = 1.0f * 10.0f;
	float4 gravityPos = {0.9f, 0.0f, 0.0f, 0.0f};

	float3 dir = normalize(positions[gid].xyz - gravityPos.xyz);
	
	// vecteur perpendiculaire arbitraire
	float3 upVector = (float3)(0.f, 1.f, 0.f);
    if (fabs(dot(dir, upVector)) > 0.9f) {
        upVector = (float3)(1.f, 0.f, 0.f);
    }
	float3 tangent = normalize(cross(dir, upVector));
	
	float speed = sqrt(gravityMass / length(positions[gid].xyz - gravityPos.xyz));
	
	velocities[gid].xyz = tangent * speed;
}

__kernel void initShape(
	__global float4* positions,
	__global float4* velocities,
	__global float4* colors,
	const uint nbParticles,
	const float radius, const int flag)
{
	size_t gid = get_global_id(0);
	if (gid >= nbParticles) return;

	if (flag == 0) { // Sphere
		createSphere(radius, positions, gid, nbParticles);
	} else if (flag == 1) { // Cube
		createCube(positions, gid);
	} else if (flag == 2) { // Pyramide
		createPyramid(positions, gid, nbParticles);
	}
	
	initSpeed(positions, velocities, gid);
	// velocities[gid] = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

	
	// colors[gid] = (float4)(1.0f, 0.5f, 1.0f, 1.0f);
}

// Gravity in space
__kernel void updateSpace(
	__global float4* positions,
	__global float4* velocities,
	__global float4* colors,
	const uint nbParticles,
	const float dt,
	const float4 gravityPos,
	const float gravityMass,
	const int gravityEnable
)
{
	size_t gid = get_global_id(0);
	if (gid >= nbParticles) return;

	float3 pos = positions[gid].xyz;
	float3 vel = velocities[gid].xyz;

	if (gravityEnable) {
		float3 dir = gravityPos.xyz - pos;
		float dist2 = dot(dir, dir) + 0.01f; // epsilon
		float invDist = rsqrt(dist2);
		float invDist3 = invDist * invDist * invDist;

		float3 accel = gravityMass * dir * invDist3;
		vel += accel * dt;
		pos += vel * dt;
	}

	float speed = length(vel);
	float scale = 5.0f;
	float speedNorm = clamp(log(1.0f + speed) / log(1.0f + scale), 0.0f, 1.0f);
	colors[gid].xyz = (float3)(1.0f - speedNorm, 0.0f, speedNorm);



	positions[gid].xyz = pos;
	velocities[gid].xyz = vel;
}

// Classic gravity on earth
__kernel void updateEarth(
	__global float4* positions,
	__global float4* velocities,
	const uint nbParticles,
	const float dt,
	const float4 gravity	
)
{
	float speed = 1.0f;

	size_t gid = get_global_id(0);
	if (gid >= nbParticles) return;

	// Apply gravity
	velocities[gid] += gravity * dt;

	// update position
	positions[gid] += velocities[gid] * dt * speed;

	// Rebound on the ground
	if (positions[gid].y < -0.9f) {
        positions[gid].y = -0.9f;
        velocities[gid].y *= -0.8f;
    }
}


