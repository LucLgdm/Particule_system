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

__kernel void initShape(
	__global float4* positions,
	__global float4* velocities,
	__global float4* colors,
	const uint nbParticles,
	const float radius, const int flag)
{
	size_t gid = get_global_id(0);
	if (gid >= nbParticles) return;

	if (flag) {
		// Fibonacci repartition on a sphere
		float u = (float)gid + 0.5f;
		float v = u / (float)nbParticles;

		float theta = GOLDEN_ANGLE * gid;
		float z = 1.0f - 2.0f * v;
		float r = sqrt(1.0f - z * z);

		positions[gid].x = radius * r * cos(theta);
		positions[gid].y = radius * r * sin(theta);
		positions[gid].z = radius * z;
		positions[gid].w = 1.0f;
	} else {
		float x = (hash(gid * 3u + 0u) * 2.0f - 1.0f) * radius / 2.0f;
		float y = (hash(gid * 3u + 1u) * 2.0f - 1.0f) * radius / 2.0f ;
		float z = (hash(gid * 3u + 2u) * 2.0f - 1.0f) * radius / 2.0f ;
		
		positions[gid] = (float4)(x, y, z, 1.0f);
	}

	velocities[gid] = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

	
	colors[gid] = (float4)(0.2f, 0.5, 1.0f, 1.0f);

}

