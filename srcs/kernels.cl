#define PI 3.14159265358979323846f
#define GOLDEN_ANGLE 2.399963229728653f

struct GravityPoint {
	float _Mass;
	float4 _Position;
	uint active;
};

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

void initSpeed(float4* positions, float4* velocities, size_t gid,
	__global const  struct GravityPoint* gPoint, const uint nGravityPoint) {
	
	float3 totalAccel = (float3)(0.0f, 0.0f, 0.0f);
	float3 totalVel = (float3)(0.0f, 0.0f, 0.0f);

	for(uint i = 0; i < nGravityPoint; i++) {
		//if (!gPoint[i].active) continue;

		float3 dir = gPoint[i]._Position.xyz - positions[gid].xyz;
		float dist = length(dir);
		
		if (dist < 0.1f) continue; // Trop proche, éviter division par zéro
		
		float3 dirNorm = dir / dist;
		
		// Vitesse orbitale circulaire : v = sqrt(GM/r)
		float orbitalSpeed = sqrt(gPoint[i]._Mass / dist);
		

		//		Sans variation orbitale
		// Vecteur perpendiculaire pour la vitesse tangentielle
		float3 upVector = (float3)(0.0f, 1.0f, 0.0f);
		if (fabs(dot(dirNorm, upVector)) > 0.9f) {
			upVector = (float3)(1.0f, 0.0f, 0.0f);
		}
		float3 tangent = normalize(cross(dirNorm, upVector));
		
		// Ajouter une composante aléatoire pour varier les orbites
		float randomFactor = 0.8f + hash(gid) * 0.4f; // Entre 0.8 et 1.2

		//		Variation d'inclinaison orbitale
		// Axe de rotation aléatoire pour chaque particule
		// float angle1 = hash(gid * 2u) * 2.0f * PI;
		// float angle2 = hash(gid * 2u + 1u) * PI * 0.3f; // Inclinaison max 30°
		// 
		// float3 axis = (float3)(
		// 	sin(angle2) * cos(angle1),
		// 	cos(angle2),
		// 	sin(angle2) * sin(angle1)
		// );
		// 
		// float3 tangent = normalize(cross(dirNorm, axis));
		// 
		// float randomFactor = 0.7f + hash(gid * 3u) * 0.6f; // 0.7 à 1.3
		
		totalVel += tangent * orbitalSpeed * randomFactor;
	}
	
	velocities[gid].xyz = totalVel;
}

__kernel void initShape(
	__global float4* positions,
	__global float4* velocities,
	__global float4* colors,
	const uint nbParticles,
	const float radius, const int flag,
	__global const struct GravityPoint* gPoint,
	const uint nGravityPoint)
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
	
	initSpeed(positions, velocities, gid, gPoint, nGravityPoint);
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
	__global const struct GravityPoint* gPoint,
	const uint nGravityPoint
)
{
	size_t gid = get_global_id(0);
	if (gid >= nbParticles) return;

	float3 pos = positions[gid].xyz;
	float3 vel = velocities[gid].xyz;
	float3 totalAccel = (float3)(0.0f, 0.0f, 0.0f);

	for (uint i = 0; i < nGravityPoint; i++) {
		if (!gPoint[i].active) continue;

		float3 dir = gPoint[i]._Position.xyz - pos;
		float dist2 = dot(dir, dir) + 0.01f; // epsilon
		float invDist = rsqrt(dist2);
		float invDist3 = invDist * invDist * invDist;

		float3 accel = gPoint[i]._Mass * dir * invDist3;
		totalAccel += accel;
	}
	vel += totalAccel * dt;
	pos += vel * dt;

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


