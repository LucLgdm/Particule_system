# Particule_system


ParticleSystem class
must:
	create OpenGL buffers
	allocate GPU memory
	allocate without any CPU-side data
	define usage hints
must not:
	use std::vector
	pass a pointer to CPU memory
	initialize particle values (that’s GPU kernel work)

