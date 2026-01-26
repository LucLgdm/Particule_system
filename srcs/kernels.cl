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
	__global const struct GravityPoint* gPoint, const uint nGravityPoint) {
	
	float3 totalVel = (float3)(0.0f, 0.0f, 0.0f);
	
	for(uint i = 0; i < nGravityPoint; i++) {
		if (!gPoint[i].active) continue;
		
		float3 dir = gPoint[i]._Position.xyz - positions[gid].xyz;
		float dist = length(dir);
		
		if (dist < 0.2f) continue;
		
		float3 dirNorm = dir / dist;
		float orbitalSpeed = sqrt(gPoint[i]._Mass / dist);
		
		// Axe aléatoire pour la tangente
		float angle1 = hash(gid * 2u) * 2.0f * PI;
		float angle2 = hash(gid * 2u + 1u) * PI * 0.5f;
		
		float3 axis = (float3)(
			sin(angle2) * cos(angle1),
			cos(angle2),
			sin(angle2) * sin(angle1)
		);
		
		float3 tangent = normalize(cross(dirNorm, axis));
		
		// IMPORTANT : Ajouter une composante radiale aléatoire
		float radialFactor = (hash(gid * 3u) * 2.0f - 1.0f) * 0.3f; // -0.3 à +0.3
		float tangentialFactor = 0.7f + hash(gid * 4u) * 0.6f; // 0.7 à 1.3
		
		// Vitesse = composante tangentielle + composante radiale
		float3 velocity = tangent * orbitalSpeed * tangentialFactor 
		                + dirNorm * orbitalSpeed * radialFactor;
		
		totalVel += velocity;
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
	const uint nGravityPoint,
	const uint colorMode
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

	// Set colors

	float3 color;
	if (colorMode == 0) {
		float speed = length(vel);
		float speedNorm = clamp(speed / 7.0f, 0.0f, 1.0f);
		speedNorm = speedNorm * speedNorm;  // Courbe quadratique

		float3 coldColor = (float3)(0.3f, 0.0f, 0.8f);   // Violet foncé
		float3 midColor = (float3)(1.0f, 0.2f, 0.6f);    // Rose
		float3 hotColor = (float3)(1.0f, 0.8f, 0.0f);    // Jaune-orange

		
		if (speedNorm < 0.5f) {
			float t = speedNorm * 2.0f;
			color = mix(coldColor, midColor, t);
		} else {
			float t = (speedNorm - 0.5f) * 2.0f;
			color = mix(midColor, hotColor, t);
		}
	} else {
		float speed = length(vel);
		float speedNorm = clamp(speed / 8.0f, 0.0f, 1.0f);
		speedNorm = 1.0f - exp(-speedNorm * 3.0f);  // Courbe exponentielle inversée

		// Bleu profond → Bleu clair → Blanc → Jaune → Rouge
		if (speedNorm < 0.3f) {
			float t = speedNorm / 0.3f;
			color = mix((float3)(0.0f, 0.0f, 0.3f), (float3)(0.2f, 0.4f, 1.0f), t);
		} else if (speedNorm < 0.6f) {
			float t = (speedNorm - 0.3f) / 0.3f;
			color = mix((float3)(0.2f, 0.4f, 1.0f), (float3)(1.0f, 1.0f, 0.8f), t);
		} else {
			float t = (speedNorm - 0.6f) / 0.4f;
			color = mix((float3)(1.0f, 1.0f, 0.8f), (float3)(1.0f, 0.1f, 0.0f), t);
		}
	}

	colors[gid].xyz = color;
	positions[gid].xyz = pos;
	velocities[gid].xyz = vel;
}



