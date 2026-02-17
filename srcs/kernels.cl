#define PI 3.14159265358979323846f
#define GOLDEN_ANGLE 2.399963229728653f

struct GravityPoint {
	float _Mass;
	float4 _Position;
	uint active;
	uint type;
};

float hash(uint x) {
	x ^= x >> 16;
	x *= 0x7feb352d;
	x ^= x >> 15;
	x *= 0x846ca68b;
	x ^= x >> 16;
	return (float)x / (float)UINT_MAX;
}

void createSphere(float radius, __global float4* positions, size_t gid, uint n) {
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

void createCube(float baseCube, __global float4* positions, size_t gid) {
	float x = (hash(gid * 3u + 0u) * 2.0f - 1.0f) * baseCube / 2.0f;
	float y = (hash(gid * 3u + 1u) * 2.0f - 1.0f) * baseCube / 2.0f ;
	float z = (hash(gid * 3u + 2u) * 2.0f - 1.0f) * baseCube / 2.0f ;
	
	positions[gid] = (float4)(x, y, z, 1.0f);
}

void createPyramid(__global float4* positions, size_t gid, const uint n, float baseSize) {
	uint layers = 20;
	float height = baseSize;

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

float3 getSurfaceNormal(float4 position, size_t gid, const uint nbParticles, const int shapeFlag) {
	float3 normal = (float3)(0.0f, 0.0f, 0.0f);
	
	if (shapeFlag == 0) {
		// Sphère : normale = direction depuis le centre
		normal = normalize(position.xyz);
		
	} else if (shapeFlag == 1) {
		// Cube : normale = face la plus proche
		float3 p = position.xyz;
		float3 absP = fabs(p);
		
		if (absP.x > absP.y && absP.x > absP.z) {
			normal = (float3)(sign(p.x), 0.0f, 0.0f);
		} else if (absP.y > absP.z) {
			normal = (float3)(0.0f, sign(p.y), 0.0f);
		} else {
			normal = (float3)(0.0f, 0.0f, sign(p.z));
		}
		
	} else if (shapeFlag == 2) {
		// Pyramide : normale dépend de la face
		float3 p = position.xyz;
		
		if (p.y < 0.1f) {
			// Base de la pyramide
			normal = (float3)(0.0f, -1.0f, 0.0f);
		} else {
			// Faces inclinées : normale pointe vers l'extérieur et vers le haut
			float3 toCenter = -normalize((float3)(p.x, 0.0f, p.z));
			normal = normalize((float3)(toCenter.x, 0.5f, toCenter.z));
		}
	}
	
	return normal;
}

void initSpeed(__global float4* positions, __global float4* velocities, size_t gid,
	__global const struct GravityPoint* gPoint, const uint nGravityPoint,
	const uint nbParticles, const int shapeFlag, uint speed) {
	
	if (speed == 1) {
		// Obtenir la normale de surface
		float3 normal = getSurfaceNormal(positions[gid], gid, nbParticles, shapeFlag);
		
		// Vitesse de base selon la normale
		float baseSpeed = 1.0f + hash(gid) * 5.0f; // Vitesse entre 1 et 3
		float3 normalVel = normal * baseSpeed;
		
		// Option 1 : Uniquement normale
		// velocities[gid].xyz = normalVel;
		
		// Option 2 : Normale + composante orbitale (mix)
		float3 orbitalVel = (float3)(0.0f, 0.0f, 0.0f);
		for(uint i = 0; i < nGravityPoint; i++) {
			if (!gPoint[i].active) continue;
			
			float3 dir = gPoint[i]._Position.xyz - positions[gid].xyz;
			float dist = length(dir);
			if (dist < 0.2f) continue;
			
			float3 dirNorm = dir / dist;
			float orbitalSpeed = sqrt(gPoint[i]._Mass / dist);
			
			// float3 up = (fabs(dirNorm.y) < 0.9f) ? (float3)(0,1,0) : (float3)(1,0,0);
			// float3 tangent = normalize(cross(dirNorm, up));
			// 
			// orbitalVel += tangent * orbitalSpeed;

			float angle = hash(gid ^ (i * 2654435761u)) * 2.0f * PI;
			float3 randAxis = normalize((float3)(
				cos(angle),
				sin(angle * 0.7f + 1.0f),
				sin(angle)
			));
			float3 tangent = normalize(cross(dirNorm, randAxis));
			
			orbitalVel += tangent * orbitalSpeed;
		}
		velocities[gid].xyz = normalVel * 0.7f + orbitalVel * 0.3f;
	} else if (speed == 2) {
		float3 totalVel = (float3)(0.0f, 0.0f, 0.0f);
		
		for(uint i = 0; i < nGravityPoint; i++) {
			if (!gPoint[i].active) continue;
			
			float3 dir = gPoint[i]._Position.xyz - positions[gid].xyz;
			float dist = length(dir);
			
			if (dist < 0.2f) continue;
			
			float3 dirNorm = dir / dist;
			float orbitalSpeed = sqrt(gPoint[i]._Mass / dist);
			
			// Axe aléatoire pour la tangente
			float angle1 = hash(gid * 2u ^ i) * 2.0f * PI;
			float angle2 = hash(gid * 3u ^ i) * PI;
			
			float3 axis  = normalize((float3)(
				sin(angle2) * cos(angle1),
				cos(angle2),
				sin(angle2) * sin(angle1)
			));
			
			float3 tangent = normalize(cross(dirNorm, axis));
			
			// Excentricité légère pour orbites elliptiques (±20%)
			float eccFactor = 0.85f + hash(gid * 5u ^ i) * 0.3f; // 0.85 à 1.15
			
			totalVel += tangent * orbitalSpeed * eccFactor;
		}
		velocities[gid].xyz = totalVel;
	}
}

__kernel void initShape(
	__global float4* positions,
	__global float4* velocities,
	__global float4* colors,
	const uint nbParticles,
	const float radius, const int flag,
	__global const struct GravityPoint* gPoint,
	const uint nGravityPoint,
	const uint speed)
{
	size_t gid = get_global_id(0);
	if (gid >= nbParticles) return;

	if (flag == 0) { // Sphere
		createSphere(radius, positions, gid, nbParticles);
	} else if (flag == 1) { // Cube
		createCube(radius, positions, gid);
	} else if (flag == 2) { // Pyramide
		createPyramid(positions, gid, nbParticles, radius);
	}
	
	initSpeed(positions, velocities, gid, gPoint, nGravityPoint, nbParticles, flag, speed);
}

// Curl noise helper
float3 curlNoise(float3 p, float t) {
	float eps = 0.01f;
	float3 curl = (float3)(
		(sin((p.y + eps) * 1.3f + t) * cos(p.z * 0.9f) - sin(p.y * 1.3f + t) * cos((p.z + eps) * 0.9f)) / eps,
		(sin(p.z * 1.1f + t) * cos((p.x + eps) * 1.2f) - sin((p.z + eps) * 1.1f + t) * cos(p.x * 1.2f)) / eps,
		(sin((p.x + eps) * 0.8f + t) * cos(p.y * 1.4f) - sin(p.x * 0.8f + t) * cos((p.y + eps) * 1.4f)) / eps
	);
	return curl;
}

// Force in space
__kernel void updateSpace(
	__global float4* positions,
	__global float4* velocities,
	__global float4* colors,
	const uint nbParticles,
	const float dt,
	const float time,
	__global const struct GravityPoint* gPoint,
	const uint nGravityPoint,
	const uint colorMode
)
{
	size_t gid = get_global_id(0);
	if (gid >= nbParticles) return;

	float3 pos        = positions[gid].xyz;
	float3 vel        = velocities[gid].xyz;
	float3 totalForce = (float3)(0.0f, 0.0f, 0.0f);

	#define SOFTENING       0.2f
	#define MAX_SPEED       30.0f
	#define CAPTURE_RADIUS  0.5f

	for (uint i = 0; i < nGravityPoint; i++) {
		if (!gPoint[i].active) continue;

		float3 dir  = gPoint[i]._Position.xyz - pos;
		float  dist = length(dir);
		// float3 dirNorm = (dist > 0.0001f) ? (dir / dist) : (float3)(0.0f, 1.0f, 0.0f);
		float3 dirNorm = dir / dist;

		if (gPoint[i].type == 0) {
			// Gravité classique
			if (dist < CAPTURE_RADIUS) {
				vel *= 0.80f;
				continue;
			}

			// float dist2    = dist * dist + SOFTENING * SOFTENING;
			// float dist2    = dist * dist + 0.001f;
			// float invDist  = rsqrt(dist2);
			// float invDist3 = invDist * invDist * invDist;
			// totalForce    += gPoint[i]._Mass * dirNorm * invDist3;

			float dist2    = dot(dir, dir) + 0.01f; // epsilon
			float invDist  = rsqrt(dist2);
			float invDist3 = invDist * invDist * invDist;
			totalForce    += gPoint[i]._Mass * dir * invDist3;

		} else if (gPoint[i].type == 1) {
			// Lorentz : champ magnétique centré sur le point
			if (dist < CAPTURE_RADIUS) {
				vel *= 0.80f;
				continue;
			}
			float3 B    = dirNorm * gPoint[i]._Mass / (dist * dist + SOFTENING);
			totalForce += cross(vel, B);

		} else if (gPoint[i].type == 2) {
			// Turbulence / Curl noise centré sur le point
			float3 localPos = (pos - gPoint[i]._Position.xyz) * 0.5f;
			float3 curl     = curlNoise(localPos, time);
			float  falloff  = gPoint[i]._Mass / (dist + 1.0f); // Diminue avec la distance
			totalForce     += curl * falloff;

		} else if (gPoint[i].type == 3) {
			// Répulsion pure
			if (dist < CAPTURE_RADIUS) {
				vel *= 0.80f;
				continue;
			}
			float dist2    = dist * dist + SOFTENING * SOFTENING;
			float invDist  = rsqrt(dist2);
			float invDist3 = invDist * invDist * invDist;
			totalForce    -= gPoint[i]._Mass * dirNorm * dist * invDist3;
		}
	}

	vel += totalForce * dt;
	pos += vel * dt;

	// Set colors

	float3 color;
	switch (colorMode) {
		case 0: {
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
			break;
		}
		case 1: {
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
			break;
		}
		case 2: {
			switch (gid % 3) {
				case 0: color = (float3)(0.3f, 0.6f, 0.9f); break;
				case 1: color = (float3)(0.9f, 0.3f, 0.3f); break;
				case 2: color = (float3)(0.3f, 0.9f, 0.3f); break;
				// case 3: color = (float3)(0.9f, 0.6f, 0.1f); break;
				// case 4: color = (float3)(0.6f, 0.3f, 0.9f); break;
			}
			break;
		}

	}
	colors[gid].xyz = color;
	positions[gid].xyz = pos;
	velocities[gid].xyz = vel;
}
